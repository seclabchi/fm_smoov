/*
 * command_server.h
 *
 *  Created on: Jul 7, 2022
 *      Author: zaremba
 */

#ifndef COMMAND_SERVER_H_
#define COMMAND_SERVER_H_

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "common_defs.h"
#include "spdlog/spdlog.h"
#include "fmsmoov.pb.h"

#include <zmqpp/context.hpp>
#include <zmqpp/socket.hpp>
#include <zmqpp/message.hpp>
#include <zmqpp/poller.hpp>

class ProcessorMain;

using namespace std;

class CommandServer {
public:
	CommandServer();
	virtual ~CommandServer();
	void start();
	void stop();
	//void operator ()(string params);
	void set_processor(ProcessorMain* processor);
	void publish_live_data(const fmsmoov::ProcessorLiveData& pld);
	void send_reply(const fmsmoov::ProcessorResponse& rsp);
private:
	class Publisher {
	public:
		Publisher(std::mutex& _mutex_startup, std::condition_variable& _cv_startup, bool& _cmdserver_started);
		virtual ~Publisher();
		void operator ()(string params);
		void stop();
		void refresh_live_data(const fmsmoov::ProcessorLiveData& pld);
	private:
		std::shared_ptr<spdlog::logger> log;
		bool m_shutdown_signalled;
		std::mutex mutex_shutdown;
		std::condition_variable cv_shutdown;

		std::mutex& mutex_startup;
		std::condition_variable& cv_startup;
		bool& m_publisher_started;
		bool m_publisher_shutdown_complete;

		fmsmoov::ProcessorLiveData m_pld;

		zmqpp::message_t *msg;
		zmqpp::context* m_context;
		zmqpp::socket* m_socket_pub;

	};

	class Responder {
	public:
		Responder(std::mutex& _mutex_startup, std::condition_variable& _cv_startup, bool& _cmdserver_started);
		virtual ~Responder();
		void operator ()(string params);
		void stop();
		void set_processor(ProcessorMain* processor);
		void send_reply(const fmsmoov::ProcessorResponse& rsp);
	private:
		std::shared_ptr<spdlog::logger> log;
		bool m_shutdown_signalled;
		std::mutex mutex_shutdown;
		std::condition_variable cv_shutdown;

		std::mutex& mutex_startup;
		std::condition_variable& cv_startup;
		bool& m_responder_started;
		bool m_responder_shutdown_complete;

		ProcessorMain* m_processor;

		zmqpp::context* m_context;
		zmqpp::socket* m_socket_req;
		zmqpp::message_t *req_msg;
		zmqpp::message_t *rsp_msg;
	};
private:
	std::shared_ptr<spdlog::logger> log;

	Publisher* m_publisher;
	std::thread* m_thread_pub;
	Responder* m_responder;
	std::thread* m_thread_rep;

	ProcessorMain* m_processor;

	zmqpp::message_t *msg;
	zmqpp::context* m_context;
	zmqpp::socket* m_socket_pub;

	zmqpp::message_t *req_msg;
	zmqpp::message_t *rsp_msg;
	zmqpp::socket* m_socket_req;


};

#endif /* COMMAND_SERVER_H_ */
