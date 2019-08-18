/*
 * cmd_server.cpp
 *
 *  Created on: Aug 11, 2019
 *      Author: zaremba
 */

#include <cmd_server.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <cstdlib>

#include "audio_buffer.h"


CmdServer::CmdServer()
{
	m_shutting_down = false;
}

int CmdServer::start()
{
	pthread_attr_t attr;
	int retval = 0;

	retval = pthread_attr_init(&attr);
	if(retval)
	{
		throw runtime_error(strerror(retval));
	}

	retval = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	if(retval)
	{
		throw runtime_error(strerror(retval));
	}

	retval = pthread_create(&m_thread, &attr, CmdServer::thread_func_wrapper, this);
	if(retval)
	{
		throw runtime_error(strerror(retval));
	}

	retval = pthread_attr_destroy(&attr);
	if(retval)
	{
		throw runtime_error(strerror(retval));
	}
}

void* CmdServer::thread_func()
{
	fd_set fifo_set;

	ws_fifo_in = open(ws_in, O_RDONLY);

	if(ws_fifo_in < 0)
	{
		throw runtime_error(strerror(ws_fifo_in));
	}

	ws_fifo_out = open(ws_out, O_WRONLY);

	if(ws_fifo_out < 0)
	{
		throw runtime_error(strerror(ws_fifo_out));
	}

	int bytes = 0;
	char buf[PIPE_BUF] = { '\0' };
	char msg[512] = { '\0' };

	while(!m_shutting_down)
	{
		FD_ZERO (&fifo_set);
		FD_SET (ws_fifo_in, &fifo_set);

		if((select (ws_fifo_in + 1, &fifo_set, NULL, NULL, NULL)) < 1)
		{
			throw runtime_error(strerror(ws_fifo_out));
		}
		if(!FD_ISSET (ws_fifo_in, &fifo_set))
		{
			continue;
		}

		uint32_t client_id, msg_type, payload_len;
		//read header
		if(read(ws_fifo_in, buf, 12) > 0)
		{
			client_id = ntohl(*(uint32_t*)buf);
			msg_type = ntohl(*(uint32_t*)(buf+4));
			payload_len = ntohl(*(uint32_t*)(buf+8));
			//printf("CMD MSG client_id 0x%08X, msg_type 0x%08X, payload_len 0x%08X\n",
			//		client_id, msg_type, payload_len);
		}
		if(read(ws_fifo_in, buf, payload_len) > 0)
		{
			memset(msg, 0, 512);
			strncpy(msg, buf, payload_len);
			process_command(msg);
		}
	}
}

void CmdServer::write_to_client(char* buf)
{
	uint32_t client_id, msg_type, payload_len;
	//write header
	client_id = htonl(0);
	msg_type = htonl(1);
	payload_len = htonl(strlen(buf));

	char* hdr = (char*)calloc (sizeof(uint32_t) * 3, sizeof(char));
	memcpy(hdr, &client_id, 4);
	memcpy(hdr+4, &msg_type, 4);
	memcpy(hdr+8, &payload_len, 4);

	write(ws_fifo_out, hdr, 12);
	write(ws_fifo_out, buf, strlen(buf));
}

void CmdServer::process_command(char* cmd)
{
	char outbuf[512];
	if(!strcmp(cmd, "SRQ"))
	{
		//printf("Status request received.\n");
		sprintf(outbuf, "INL %0.1f %0.1f %0.1f %0.1f", AudioBuffer::peak_lin_db, AudioBuffer::rms_lin_db, AudioBuffer::peak_rin_db, AudioBuffer::rms_rin_db);
		write_to_client(outbuf);
		sprintf(outbuf, "OUL %0.1f %0.1f %0.1f %0.1f", AudioBuffer::peak_lout_db, AudioBuffer::rms_lout_db, AudioBuffer::peak_rout_db, AudioBuffer::rms_rout_db);
		write_to_client(outbuf);
	}
}

void* CmdServer::thread_func_wrapper(void* args)
{
	CmdServer* me = (CmdServer*) args;
	me->thread_func();
}

CmdServer::~CmdServer() {
	// TODO Auto-generated destructor stub
}

