/*
 * websocket_server.cpp
 *
 *  Created on: Apr 20, 2020
 *      Author: zaremba
 */

#include <unistd.h>
#include <string.h>
#include <iostream>
#include <string>
#include <stdexcept>

#define LWS_PLUGIN_STATIC
#include <libwebsockets.h>
#include "websocket_server.h"

#include "protocol_lws_minimal.cpp"

using namespace std;

static const struct lws_protocol_vhost_options pvo_hsbph[] = {{
		NULL, NULL,		"referrer-policy:", "no-referrer"
		}, {
			&pvo_hsbph[0], NULL,	"x-xss-protection:", "1; mode=block"
		}, {
			&pvo_hsbph[1], NULL,	"x-content-type-options:", "nosniff"
		}, {
			&pvo_hsbph[2], NULL,	"content-security-policy:",
						"default-src 'self'; "
						"img-src https://www.gravatar.com 'self' data: ; "
						"script-src 'self' 'unsafe-eval'; "
						"font-src 'self'; "
						"style-src 'self'; "
						"connect-src 'self'; "
						"frame-ancestors 'self'; "
						"base-uri 'none'; "
						"form-action  'self';"
}};

static const struct lws_protocol_vhost_options pvo_mime = {
        NULL,				/* "next" pvo linked-list */
        NULL,				/* "child" pvo linked-list */
        ".proto",				/* file suffix to match */
        ".map"		/* mimetype to use */
};

static struct lws_protocols protocols[] = {
		{"http", lws_callback_http_dummy, 0, 0, 0, NULL, 0},
		LWS_PLUGIN_PROTOCOL_MINIMAL,
		{ NULL, NULL, 0, 0 } /* terminator */
};

static const struct lws_http_mount mount = {
		/* .mount_next */				NULL,		/* linked-list "next" */
		/* .mountpoint */				"/",		/* mountpoint URL */
		/* .origin */					"/tmp/html/dist", /* serve from dir */
		/* .def */						"index.html",	/* default filename */
		/* .protocol */					NULL,
		/* .cgienv */					NULL,
		/* .extra_mimetypes */			&pvo_mime,
		/* .interpret */				NULL,
		/* .cgi_timeout */				0,
		/* .cache_max_age */			0,
		/* .auth_mask */				0,
		/* .cache_reusable */			0,
		/* .cache_revalidate */			0,
		/* .cache_intermediaries */		0,
		/* .origin_protocol */			LWSMPRO_FILE,	/* files in a dir */
		/* .mountpoint_len */			1,		/* char count */
		/* .basic_auth_login_file */	NULL,
};



char client_ip[64];
uint32_t example_counter = 0;
bool send_push = false;
struct lws *g_wsi = NULL;

WebsocketServer::WebsocketServer()
{
	shutdown = false;
}

WebsocketServer::~WebsocketServer()
{

}

void WebsocketServer::go()
{

	pthread_attr_init(&thread_attrs);
	pthread_create(&the_thread, &thread_attrs, WebsocketServer::thread_func_wrapper, this);
	pthread_attr_destroy(&thread_attrs);

	pthread_attr_init(&push_thread_attrs);
	pthread_create(&push_thread, &push_thread_attrs, WebsocketServer::push_thread_func_wrapper, this);
	pthread_attr_destroy(&push_thread_attrs);
}

void WebsocketServer::stop()
{
	void* thread_result = NULL;
	cout << "Stopping websocket server..." << endl;
	shutdown = true;
	lws_cancel_service(context);
	pthread_join(the_thread, &thread_result);
	pthread_join(push_thread, &thread_result);
	cout << "websocket server stopped." << endl;
}


void* WebsocketServer::thread_func(void* args)
{
	lws_set_log_level(logs, NULL);
	lwsl_user("LWS minimal ws broker | visit http://localhost:7681\n");

	memset(&info, 0, sizeof info); /* otherwise uninitialized garbage */
	info.port = 7681;
	info.mounts = &mount;
	info.protocols = protocols;
	//info.options =
	//		LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;
	info.headers = &pvo_hsbph[3];
	context = lws_create_context(&info);
	if (!context) {
			lwsl_err("lws init failed\n");
			//return 1;
	}

	while (n >= 0 && !shutdown)
	{
		n = lws_service(context, 0);
	}

	lws_context_destroy(context);

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
	//return me->callback_http(wsi, reason, user, in, len);
	return 0;
}


int WebsocketServer::callback_example_wrapper(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
	WebsocketServer* me = reinterpret_cast<WebsocketServer*>(user);
	return me->callback_example(wsi, reason, user, in, len);
}

int WebsocketServer::callback_example( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len )
{
	return 0;
}

void* WebsocketServer::push_thread_func_wrapper(void* args)
{
	WebsocketServer* the_instance = reinterpret_cast<WebsocketServer*>(args);
	return the_instance->push_thread_func(args);
}

void* WebsocketServer::push_thread_func(void* args)
{
	while(!shutdown)
	{
		sleep(1);
	}

	return NULL;
}
