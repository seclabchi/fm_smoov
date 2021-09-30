/*
 * CommandHandler.h
 *
 *  Created on: Sep 12, 2021
 *      Author: zaremba
 */

#ifndef INCLUDE_COMMANDHANDLER_H_
#define INCLUDE_COMMANDHANDLER_H_

#include <cstddef>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <command.pb.h>

using namespace std;
using namespace fmsmoov;

typedef int(*COMMAND_HANDLER_CALLBACK)(void*, char*);

typedef struct
{
	void* arg;
	COMMAND_HANDLER_CALLBACK cb;
} SUBSCRIBER;

class CommandHandler {
public:
	CommandHandler();
	virtual ~CommandHandler();
	static CommandHandler* get_instance();
	void add_subscriber(SUBSCRIBER sub);
	void publish_web_cmd(char* msg, int len);
	void go();
	void stop();
private:
	static CommandHandler *the_instance;
	vector<SUBSCRIBER> subscribers;
	Command* decode(Command *cmd);
	void clear_queues();
	pthread_t thread_cmd_listener;
	pthread_attr_t thread_cmd_listener_attrs;
	static void* thread_cmd_listener_func_wrapper(void* args);
	void* thread_cmd_listener_thread_func(void* args);
	bool shutdown;
	queue<Command*> *inQ;
	queue<Command*> *outQ;
	mutex *mutex_inQ;
	condition_variable *condvar_inQ;
};


#endif /* INCLUDE_COMMANDHANDLER_H_ */
