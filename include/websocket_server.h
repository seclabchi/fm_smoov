/*
 * websocket_server.h
 *
 *  Created on: Apr 20, 2020
 *      Author: zaremba
 */

#ifndef INCLUDE_WEBSOCKET_SERVER_H_
#define INCLUDE_WEBSOCKET_SERVER_H_

#include <pthread.h>
#include <iostream>
#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>


#define EXAMPLE_RX_BUFFER_BYTES (128)

using namespace std;

class WebsocketServer
{
public:
	WebsocketServer();
	virtual ~WebsocketServer();
	void go();
	void stop();
	static int callback_example_wrapper( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len );
	int callback_example( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len );
	static int callback_http_wrapper(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len );
	int callback_http( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len );
private:
	pthread_t the_thread;
	pthread_attr_t thread_attrs;
	static void* thread_func_wrapper(void* args);
	void* thread_func(void* args);
	bool shutdown;

	enum protocols
	{
		PROTOCOL_HTTP = 0,
		PROTOCOL_EXAMPLE,
		PROTOCOL_COUNT
	};

	struct lws_protocols* myprotocols;
	static struct payload received_payload;

};


#endif /* INCLUDE_WEBSOCKET_SERVER_H_ */
