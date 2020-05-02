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

	enum protocols
	{
		PROTOCOL_HTTP = 0,
		PROTOCOL_EXAMPLE,
		PROTOCOL_COUNT
	};

	struct lws_protocols* myprotocols;
	static struct payload received_payload;
	static struct payload push_payload;

	/* Periodic push thread related */
	pthread_t push_thread;
	pthread_attr_t push_thread_attrs;
	static void* push_thread_func_wrapper(void* args);
	void* push_thread_func(void* args);

};

/* one of these created for each message in the ringbuffer */

struct msg {
	void *payload; /* is malloc'd */
	size_t len;
};

/*
 * One of these is created for each client connecting to us.
 *
 * It is ONLY read or written from the lws service thread context.
 */

struct per_session_data__minimal {
	struct per_session_data__minimal *pss_list;
	struct lws *wsi;
	uint32_t tail;
};

/* one of these is created for each vhost our protocol is used with */

struct per_vhost_data__minimal {
	struct lws_context *context;
	struct lws_vhost *vhost;
	const struct lws_protocols *protocol;

	struct per_session_data__minimal *pss_list; /* linked-list of live pss*/
	pthread_t pthread_spam[2];

	pthread_mutex_t lock_ring; /* serialize access to the ring buffer */
	struct lws_ring *ring; /* {lock_ring} ringbuffer holding unsent content */

	const char *config;
	char finished;
};



#endif /* INCLUDE_WEBSOCKET_SERVER_H_ */
