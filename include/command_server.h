/*
 * command_server.h
 *
 *  Created on: Jul 7, 2022
 *      Author: zaremba
 */

#ifndef COMMAND_SERVER_H_
#define COMMAND_SERVER_H_

#include <string>
#include <mutex>
#include <condition_variable>

#include "common_defs.h"
#include "spdlog/spdlog.h"
#include "fmsmoov.pb.h"

#include <zmqpp/context.hpp>
#include <zmqpp/socket.hpp>
#include <zmqpp/message.hpp>

class ProcessorMain;

using namespace std;

class CommandServer {
public:
	CommandServer(std::mutex& _mutex_startup, std::condition_variable& _cv_startup, bool& _cmdserver_started);
	virtual ~CommandServer();
	void stop();
	void operator ()(string params);
	void set_processor(ProcessorMain* processor);
	void publish_live_data(const fmsmoov::ProcessorLiveData& pld);

private:
	std::shared_ptr<spdlog::logger> log;

	ProcessorMain* m_processor;
	fmsmoov::ProcessorLiveData m_pld;

	zmqpp::message_t *msg;
	zmqpp::context* m_context;
	zmqpp::socket* m_socket_pub;

	bool m_shutdown_signalled;
	std::mutex mutex_shutdown;
	std::condition_variable cv_shutdown;

	std::mutex& mutex_startup;
	std::condition_variable& cv_startup;
	bool& m_cmdserver_started;
	bool m_cmdserver_shutdown_complete;
};

#endif /* COMMAND_SERVER_H_ */
