/*
 * websocket_server.cpp
 *
 *  Created on: Apr 20, 2020
 *      Author: zaremba
 */

#include <unistd.h>
#include <string.h>

#include "websocket_server.h"

char client_ip[64];

struct payload
{
	//unsigned char data[LWS_SEND_BUFFER_PRE_PADDING + EXAMPLE_RX_BUFFER_BYTES + LWS_SEND_BUFFER_POST_PADDING];
	unsigned char data[LWS_SEND_BUFFER_PRE_PADDING + EXAMPLE_RX_BUFFER_BYTES];
	size_t len;
} WebsocketServer::received_payload;

WebsocketServer::WebsocketServer()
{
	shutdown = false;
}

WebsocketServer::~WebsocketServer()
{

}

void WebsocketServer::go()
{
	/* The first protocol must always be the HTTP handler */
	myprotocols = new struct lws_protocols[3] {
		{
			"http-only",   /* name */
			callback_http_wrapper, /* callback */
			0,             /* No per session data. */
			0,             /* max frame size / rx buffer */
			0,             /* id */
			this,             /* user_ptr */
			0,             /* tx_packet_size */
		},
		{
			"example-protocol",
			callback_example_wrapper,
			0,
			EXAMPLE_RX_BUFFER_BYTES,
			0,             /* id */
			this,             /* user_ptr */
			0,             /* tx_packet_size */
		},
		{ NULL, NULL, 0, 0 }}; /* terminator */

	pthread_attr_init(&thread_attrs);
	pthread_create(&the_thread, &thread_attrs, WebsocketServer::thread_func_wrapper, this);
	pthread_attr_destroy(&thread_attrs);
}

void WebsocketServer::stop()
{
	void* thread_result = NULL;
	cout << "Stopping websocket server..." << endl;
	shutdown = true;
	pthread_join(the_thread, &thread_result);
	cout << "websocket server stopped." << endl;
}

void* WebsocketServer::thread_func(void* args)
{
	struct lws_context_creation_info info;
	memset( &info, 0, sizeof(info) );

	info.port = 8000;
	info.protocols = (const struct lws_protocols*)myprotocols;
	info.gid = -1;
	info.uid = -1;
	info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	info.ssl_cert_filepath = "/opt/fmsmoov/fmsmoov.crt";
	info.ssl_private_key_filepath = "/opt/fmsmoov/fmsmoov.key";

	struct lws_context *context = lws_create_context( &info );

	while(false == shutdown)
	{
		lws_service( context, /* timeout_ms = */ 1000000 );
		//cout << "Servicing websockets client:" << endl;
	}

	lws_context_destroy( context );

	return NULL;
}

void* WebsocketServer::thread_func_wrapper(void* args)
{
	WebsocketServer* the_instance = reinterpret_cast<WebsocketServer*>(args);
	return the_instance->thread_func(args);
}

int WebsocketServer::callback_http_wrapper(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
	WebsocketServer* me = reinterpret_cast<WebsocketServer*>(user);
	return me->callback_http(wsi, reason, user, in, len);
}

int WebsocketServer::callback_http( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len )
{
	char* http_req_url = (char*)in;

//	cout << "callback_http with reason " << reason << endl;

	switch( reason )
	{
		case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
			lws_get_peer_simple_fd	((uint32_t)in, client_ip, 64);
			cout << "Client " << client_ip << " connected fd= " << (uint32_t) in << endl;
			break;
		case LWS_CALLBACK_HTTP:
			cout << "http request: " << (char*)in << endl;

			if(!strcmp(http_req_url, "/"))
			{
				lws_serve_http_file( wsi, "/tmp/html/fmsmoov.html", "text/html", NULL, 0 );
			}
			else if(!strcmp(http_req_url, "/fmsmoov.css"))
			{
				lws_serve_http_file(wsi, "/tmp/html/fmsmoov.css", "text/css", NULL, 0);
			}
			else if(!strcmp(http_req_url, "/fmsmoov.js"))
			{
				lws_serve_http_file(wsi, "/tmp/html/fmsmoov.js", "text/javascript", NULL, 0);
			}
			else if(!strcmp(http_req_url, "/favicon.ico") || !strcmp(http_req_url, "/error.css"))
			{
				lws_return_http_status(wsi, 404, "");
			}
			else
			{
				lws_return_http_status(wsi, 401, "<h2>I'm not sure what you're trying to do, but knock it off, pal.</h2>");
			}

			break;
		default:
			break;
	}

	return 0;
}

int WebsocketServer::callback_example_wrapper(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
	WebsocketServer* me = reinterpret_cast<WebsocketServer*>(user);
	return me->callback_example(wsi, reason, user, in, len);
}

int WebsocketServer::callback_example( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len )
{
	switch( reason )
	{
		case LWS_CALLBACK_RECEIVE:
			if(len <= EXAMPLE_RX_BUFFER_BYTES)
			{
				memcpy( &received_payload.data[LWS_SEND_BUFFER_PRE_PADDING], in, len );
				//memcpy( received_payload.data, in, len );
				received_payload.len = len;
				cout << "Received WS msg of length " << len << endl;
			}
			else
			{
				cout << "ERROR! RX data too big for buffer size " << EXAMPLE_RX_BUFFER_BYTES << endl;
				strcpy( (char*)received_payload.data, "ERROR: TOO BIG");
				received_payload.len = strlen("ERROR: TOO BIG");
			}
			lws_callback_on_writable_all_protocol( lws_get_context( wsi ), lws_get_protocol( wsi ) );
			break;

		case LWS_CALLBACK_SERVER_WRITEABLE:
			lws_write( wsi, &received_payload.data[LWS_SEND_BUFFER_PRE_PADDING], received_payload.len, LWS_WRITE_TEXT );
			//lws_write( wsi, received_payload.data, received_payload.len, LWS_WRITE_TEXT );
			break;

		default:
			break;
	}

	return 0;
}
