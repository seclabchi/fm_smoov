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


CommandServer::CommandServer(std::mutex& _mutex_startup, std::condition_variable& _cv_startup, bool& _cmdserver_started) :
		mutex_startup(_mutex_startup), cv_startup(_cv_startup), m_cmdserver_started(_cmdserver_started)
{
	log = spdlog::stdout_color_mt("COMMAND_SERVER");
	log->set_pattern("%^[%H%M%S.%e][%s:%#][%n][%l] %v%$");
	log->set_level(spdlog::level::trace);
	LOGT("CommandServer CTOR");
	m_shutdown_signalled = false;
	m_cmdserver_shutdown_complete = false;

	m_context = new zmqpp::context();
	m_socket_pub = new zmqpp::socket(*m_context, zmqpp::socket_type::publish);


}

CommandServer::~CommandServer() {
	LOGT("CommandServer DTOR");
}

void CommandServer::set_processor(ProcessorMain* processor) {
	m_processor = processor;
}

void CommandServer::stop() {

	/* Set shutdown flag and notify thread */
	{
		std::lock_guard lk(mutex_shutdown);
		m_shutdown_signalled = true;
		LOGD("CommandServer shutdown signal is set.");
	}
	cv_shutdown.notify_one();

	/* Wait for thread to finish shutting down */
	{
		LOGD("Waiting for CommandServer shutdown...");
		std::unique_lock lk(mutex_shutdown);
		cv_shutdown.wait(lk, [&]{return m_cmdserver_shutdown_complete;});
	}

	LOGD("ProcessorMain shutdown is complete.");

}

void CommandServer::publish_live_data(const fmsmoov::ProcessorLiveData& pld) {
	m_pld.CopyFrom(pld);
}

void CommandServer::operator ()(string params) {
	std::unique_lock lk(mutex_shutdown);


	static float tmpval = 0.0;

	{
		std::lock_guard lkstartup(mutex_startup);
		LOGD("CommandServer thread starting up...");
		m_cmdserver_started = true;
		m_socket_pub->bind("tcp://0.0.0.0:5555");

		LOGD("CommandServer started.  Notifying thread parent...");
	}
	cv_startup.notify_one();

	LOGD("CommandServer waiting for shutdown signal...");

	while(false == m_shutdown_signalled) {
		auto now = std::chrono::system_clock::now();
		if(cv_shutdown.wait_until(lk, now + 100ms, [&](){return m_shutdown_signalled;})) {
			LOGD("CommandServer got shutdown signal.  Shutting down...");
		}
		else {
			LOGI("CommandServer publish live data...");
			msg = new zmqpp::message_t(m_pld.SerializeAsString(), m_pld.ByteSizeLong());
			m_socket_pub->send(*msg);
			delete msg;
		}
	}

	//cv_shutdown.wait(lk, [&]{return m_shutdown_signalled;});



	m_cmdserver_shutdown_complete = true;
	LOGD("CommandServer shutdown complete.  Notifying thread parent...");

	lk.unlock();
	cv_shutdown.notify_one();

	LOGD("Exiting CommandServer thread func");
	return;
}
