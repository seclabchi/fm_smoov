/*
 * cmd_server.h
 *
 *  Created on: Aug 11, 2019
 *      Author: zaremba
 */

#ifndef __CMD_SERVER_H__
#define __CMD_SERVER_H__

#include <stdexcept>
#include <string.h>
#include <pthread.h>

using namespace std;

#define PIPE_BUF 512

class CmdServer {
public:
	CmdServer();
	int start();
	virtual ~CmdServer();
	static void* thread_func_wrapper(void* args);
	void shutdown();
private:
	void write_to_client(char* buf);
	void process_command(char* cmd);

	int m_port;
	bool m_shutting_down;
	void* thread_func();
	pthread_t m_thread;
	int ws_fifo_in;
	int ws_fifo_out;
	const char* ws_in = "/tmp/ws_in.fifo";
    const char* ws_out = "/tmp/ws_out.fifo";
};

#endif /* INCLUDE_CMD_SERVER_H_ */
