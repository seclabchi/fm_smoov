/*
 * CommandServer.cpp
 *
 *  Created on: Jul 7, 2022
 *      Author: zaremba
 */

#include "common_defs.h"
#include "command_server.h"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <atomic>
#include <chrono>

#include "spdlog/sinks/stdout_color_sinks.h"

#include "ProcessorMain.h"

CommandServer::CommandServer()
{
	log = spdlog::stdout_color_mt("COMMAND_SERVER");
	log->set_pattern("%^[%H%M%S.%e][%s:%#][%n][%l] %v%$");
	log->set_level(spdlog::level::trace);
	LOGT("CommandServer CTOR");
}

CommandServer::~CommandServer() {
	LOGT("CommandServer DTOR");
}

void CommandServer::set_processor(ProcessorMain* processor) {
	m_processor = processor;
	m_responder->set_processor(processor);
}

void CommandServer::send_reply(const fmsmoov::ProcessorResponse& rsp) {
	m_responder->send_reply(rsp);
}

void CommandServer::start() {
	/* Start publisher */
	bool publisher_started = false;
	std::mutex mutex_startup_publisher;
	std::condition_variable cv_startup_publisher;
	std::unique_lock lk_publisher(mutex_startup_publisher);

	LOGD("Constructing Publisher...");
	m_publisher = new Publisher(mutex_startup_publisher, cv_startup_publisher, publisher_started);

	LOGD("Starting publisher...");
	m_thread_pub = new std::thread(std::ref(*m_publisher), "5678");
	auto handle_publisher = m_thread_pub->native_handle();
	pthread_setname_np(handle_publisher, "publisher");
	LOGD("Waiting for startup confirmation from publisher...");
	cv_startup_publisher.wait(lk_publisher, [&]{return publisher_started;}); // @suppress("Invalid arguments")

	/* Start responder */
	bool responder_started = false;
	std::mutex mutex_startup_responder;
	std::condition_variable cv_startup_responder;
	std::unique_lock lk_responder(mutex_startup_responder);

	LOGD("Constructing responder...");
	m_responder = new Responder(mutex_startup_responder, cv_startup_responder, responder_started);

	LOGD("Starting responder...");
	m_thread_rep = new std::thread(std::ref(*m_responder), "5678");
	auto handle_responder = m_thread_rep->native_handle();
	pthread_setname_np(handle_responder, "responder");
	LOGD("Waiting for startup confirmation from responder...");
	cv_startup_responder.wait(lk_responder, [&]{return responder_started;}); // @suppress("Invalid arguments")
}

void CommandServer::stop() {
	LOGD("Stopping responder...");
	m_responder->stop();
	LOGD("Waiting to join responder...");
	m_thread_rep->join();
	LOGD("Joined.  Continuing...");
	delete m_thread_rep;

	LOGD("Stopping publisher...");
	m_publisher->stop();
	LOGD("Waiting to join publisher...");
	m_thread_pub->join();
	LOGD("Joined.  Continuing...");
	delete m_thread_pub;

}

void CommandServer::publish_live_data(const fmsmoov::ProcessorLiveData& pld) {
	m_publisher->refresh_live_data(pld);
}

CommandServer::Publisher::Publisher(std::mutex& _mutex_startup, std::condition_variable& _cv_startup, bool& _publisher_started) :
		mutex_startup(_mutex_startup), cv_startup(_cv_startup), m_publisher_started(_publisher_started)
{
	log = spdlog::stdout_color_mt("PUBLISHER");
	log->set_pattern("%^[%H%M%S.%e][%s:%#][%n][%l] %v%$");
	log->set_level(spdlog::level::debug);
	LOGT("Publisher CTOR");
	m_shutdown_signalled = false;
	m_publisher_shutdown_complete = false;

	//m_context = new zmqpp::context();
	//m_socket_pub = new zmqpp::socket(*m_context, zmqpp::socket_type::publish);
	//m_socket_req = new zmqpp::socket(*m_context, zmqpp::socket_type::reply);

}

CommandServer::Publisher::~Publisher() {
	LOGT("Publisher DTOR");
}

void CommandServer::Publisher::refresh_live_data(const fmsmoov::ProcessorLiveData& pld) {
	m_pld.CopyFrom(pld);
}

void CommandServer::Publisher::operator ()(string params) {
	std::unique_lock lk(mutex_shutdown);


	static float tmpval = 0.0;

	{
		std::lock_guard lkstartup(mutex_startup);
		LOGD("Publisher thread starting up...");
		m_publisher_started = true;
		m_context = new zmqpp::context();
		m_socket_pub = new zmqpp::socket(*m_context, zmqpp::socket_type::publish);
		m_socket_pub->bind("tcp://0.0.0.0:5555");

		LOGD("Publisher started.  Notifying thread parent...");
	}
	cv_startup.notify_one();

	LOGD("Publisher waiting for shutdown signal...");

	while(false == m_shutdown_signalled) {
		auto now = std::chrono::system_clock::now();
		if(cv_shutdown.wait_until(lk, now + 100ms, [&](){return m_shutdown_signalled;})) {
			LOGD("Publisher got shutdown signal.  Shutting down...");
		}
		else {
			LOGT("Publisher publish live data...");
			msg = new zmqpp::message_t(m_pld.SerializeAsString(), m_pld.ByteSizeLong());
			m_socket_pub->send(*msg);
			delete msg;
		}
	}

	LOGD("Publisher got shutdown signal.  Exiting Responder thread func");
	return;
}

void CommandServer::Publisher::stop() {
	m_shutdown_signalled = true;
}

CommandServer::Responder::Responder(std::mutex& _mutex_startup, std::condition_variable& _cv_startup, bool& _responder_started) :
		mutex_startup(_mutex_startup), cv_startup(_cv_startup), m_responder_started(_responder_started)
{
	log = spdlog::stdout_color_mt("RESPONDER");
	log->set_pattern("%^[%H%M%S.%e][%s:%#][%n][%l] %v%$");
	log->set_level(spdlog::level::trace);
	LOGT("Responder CTOR");
	m_shutdown_signalled = false;
	m_responder_shutdown_complete = false;
}

CommandServer::Responder::~Responder() {

}

void CommandServer::Responder::operator ()(string params) {
	std::unique_lock lk(mutex_shutdown);
	zmqpp::poller req_poller;

	static float tmpval = 0.0;

	{
		std::lock_guard lkstartup(mutex_startup);
		LOGD("Responder thread starting up...");
		m_responder_started = true;
		m_context = new zmqpp::context();
		m_socket_req = new zmqpp::socket(*m_context, zmqpp::socket_type::reply);
		m_socket_req->bind("tcp://0.0.0.0:5556");

		req_poller.add(*m_socket_req, zmqpp::poller::poll_in);

		LOGD("Responder started.  Notifying thread parent...");
	}
	cv_startup.notify_one();

	LOGD("Responder waiting for shutdown signal...");

	while(false == m_shutdown_signalled) {
		LOGI("Responder wait for request...");
		if(req_poller.poll()) {
			req_msg = new zmqpp::message_t();
			m_socket_req->receive(*req_msg);
			string request;
			*req_msg >> request;
			delete req_msg;
			fmsmoov::ProcessorCommand cmdproto;
			cmdproto.ParseFromString(request);
			LOGD("Recieved request of length: {}", cmdproto.ByteSizeLong());
			m_processor->handle_command(cmdproto);
		}
	}

	LOGD("Responder got shutdown signal.  Exiting Responder thread func");
	return;
}

void CommandServer::Responder::stop() {
	m_shutdown_signalled = true;
}

void CommandServer::Responder::set_processor(ProcessorMain* processor) {
	m_processor = processor;
}

void CommandServer::Responder::send_reply(const fmsmoov::ProcessorResponse& rsp) {
	rsp_msg = new zmqpp::message_t(rsp.SerializeAsString(), rsp.ByteSizeLong());
	m_socket_req->send(*rsp_msg);
	delete rsp_msg;
}

