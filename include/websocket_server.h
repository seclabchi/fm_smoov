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
	static int callback_server_push_wrapper( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len );
	int callback_server_push( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len );
	static int callback_http_wrapper(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len );
	int callback_http( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len );
private:
	/* libwebsockets related */
	pthread_t the_thread;
	pthread_attr_t thread_attrs;
	static void* thread_func_wrapper(void* args);
	void* thread_func(void* args);
	bool shutdown;

	/* Periodic push thread related */
	pthread_t push_thread;
	pthread_attr_t push_thread_attrs;
	static void* push_thread_func_wrapper(void* args);
	void* push_thread_func(void* args);

	struct lws_context_creation_info info;
	struct lws_context *context;
	const char *p;
	int n = 0;
	int logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;
	/* for LLL_ verbosity above NOTICE to be built into lws,
						 * lws must have been configured and built with
						 * -DCMAKE_BUILD_TYPE=DEBUG instead of =RELEASE */
						/* | LLL_INFO */ /* | LLL_PARSER */ /* | LLL_HEADER */
						/* | LLL_EXT */ /* | LLL_CLIENT */ /* | LLL_LATENCY */
						/* | LLL_DEBUG */;

};


#endif /* INCLUDE_WEBSOCKET_SERVER_H_ */
