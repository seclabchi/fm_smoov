/*
 * CommandHandler.cpp
 *
 *  Created on: Sep 12, 2021
 *      Author: zaremba
 */

#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <CommandHandler.h>
#include <command.pb.h>

CommandHandler* CommandHandler::the_instance = NULL;

CommandHandler::CommandHandler() {
	// TODO Auto-generated constructor stub
//	ofstream testostream("/tmp/command_protoenc.bin");
//	fmsmoov::Command testcmd;
//	testcmd.set_type(fmsmoov::Command_CommandType_DELAY_GET);
//	testcmd.add_vals(0.6);
//	testcmd.add_vals(2.1);
//	testcmd.SerializeToOstream(&testostream);
//	testostream.close();
	inQ = new queue<Command*>();
	outQ = new queue<Command*>();
	mutex_inQ = new mutex();
	condvar_inQ = new condition_variable();
}

CommandHandler::~CommandHandler() {
	// TODO Auto-generated destructor stub
	clear_queues();
	delete mutex_inQ;
	delete condvar_inQ;
	delete inQ;
	delete outQ;
}

void CommandHandler::go()
{
	pthread_attr_init(&thread_cmd_listener_attrs);
	pthread_create(&thread_cmd_listener, &thread_cmd_listener_attrs, CommandHandler::thread_cmd_listener_func_wrapper, this);
	pthread_attr_destroy(&thread_cmd_listener_attrs);
}

void CommandHandler::stop()
{
	void* thread_result = NULL;
	cout << "Stopping command handler..." << endl;
	shutdown = true;
	this->publish_web_cmd("SHUTDOWN", 8);
	pthread_join(thread_cmd_listener, &thread_result);
	cout << "command handler stopped." << endl;
}

void CommandHandler::add_subscriber(SUBSCRIBER sub)
{
	subscribers.push_back(sub);
}

void CommandHandler::publish_web_cmd(char* msg, int len)
{
	if(!strncmp(msg, "SHUTDOWN", 8))
	{
		this->shutdown = true;
		inQ->push(new Command());
	}
	else
	{
		Command *command = new Command();

		printf("RAW_PROTO_RX: ");
		for(int i = 0; i < len; i++)
		{
			printf("0x%02X ", msg[i]);
		}

		printf("\n");

		if (!command->ParseFromArray(msg,len)) {
		  cerr << "Failed to parse command." << endl;
		}
		else
		{
			{
				lock_guard<mutex> lock(*mutex_inQ);
				printf("Pushing ws request onto inQ...\n");
				inQ->push(command);
			}
			condvar_inQ->notify_one();
		}
	}
}

Command* CommandHandler::decode(Command *cmd)
{
	string cmdString = Command::CommandType_Name(cmd->type());
	printf("Received web command %s\n", cmdString.c_str());
	if(!cmdString.compare("DELAY_SET"))
	{
		SUBSCRIBER sub;
		stringstream cmdStr;
		vector<SUBSCRIBER>::iterator it;
		for(it = subscribers.begin(); it != subscribers.end(); it++)
		{
			sub = *it;
			cmdStr << "DELAY," << cmd->vals(0);
			sub.cb(sub.arg, (char*)cmdStr.str().c_str());
		}
	}
	delete cmd;
	return new Command();
}

void CommandHandler::clear_queues()
{
	while(false == inQ->empty())
	{
		Command* c = inQ->front();
		delete c;
		inQ->pop();
	}
	while(false == outQ->empty())
	{
		Command *c = outQ->front();
		delete c;
		outQ->pop();
	}
}

void* CommandHandler::thread_cmd_listener_func_wrapper(void* args)
{
	CommandHandler *cmdhndlr = reinterpret_cast<CommandHandler*>(args);
	return cmdhndlr->thread_cmd_listener_thread_func(args);
}

void* CommandHandler::thread_cmd_listener_thread_func(void* args)
{
	Command *cmdReq = NULL;
	Command *cmdRsp = NULL;

	while(false == shutdown)
	{
		unique_lock<mutex> lock(*mutex_inQ);
		condvar_inQ->wait(lock);
		while(false == inQ->empty())
		{
			printf("Pulling command from inQ\n");
			cmdReq = inQ->front();
			inQ->pop();
			cmdRsp = this->decode(cmdReq);
			delete cmdRsp;
		}
		printf("inQ is empty.\n");
		lock.unlock();
		condvar_inQ->notify_one();
	}
}

CommandHandler* CommandHandler::get_instance()
{
	if(NULL == the_instance)
	{
		the_instance = new CommandHandler();
	}

	return the_instance;
}
