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
#include "websocket_server.h"


using namespace std;


char client_ip[64];
uint32_t example_counter = 0;
bool send_push = false;
struct lws *g_wsi = NULL;

struct payload
{
	//unsigned char data[LWS_SEND_BUFFER_PRE_PADDING + EXAMPLE_RX_BUFFER_BYTES + LWS_SEND_BUFFER_POST_PADDING];
	unsigned char data[LWS_SEND_BUFFER_PRE_PADDING + EXAMPLE_RX_BUFFER_BYTES];
	size_t len;
} WebsocketServer::received_payload, WebsocketServer::push_payload;

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
	myprotocols = new struct lws_protocols[4] {
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
		{
			"server-push-protocol",
			callback_server_push_wrapper,
			sizeof(struct per_session_data__minimal),
			128,
			0,             /* id */
			this,          /* user_ptr */
			0,             /* tx_packet_size */
		},
		{ NULL, NULL, 0, 0 }}; /* terminator */

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
	pthread_join(the_thread, &thread_result);
	pthread_join(push_thread, &thread_result);
	cout << "websocket server stopped." << endl;
}


/*
 * This demonstrates how to pass a pointer into a specific protocol handler
 * running on a specific vhost.  In this case, it's our default vhost and
 * we pass the pvo named "config" with the value a const char * "myconfig".
 *
 * This is the preferred way to pass configuration into a specific vhost +
 * protocol instance.
 */

static const struct lws_protocol_vhost_options pvo_ops = {
	NULL,
	NULL,
	"config",		/* pvo name */
	"myconfig"	/* pvo value */
};

static const struct lws_protocol_vhost_options pvo = {
	NULL,		/* "next" pvo linked-list */
	&pvo_ops,	/* "child" pvo linked-list */
	"server-push-protocol",	/* protocol name we belong to on this vhost */
	""		/* ignored */
};

void* WebsocketServer::thread_func(void* args)
{
	struct lws_context_creation_info info;
	memset( &info, 0, sizeof(info) );

	info.port = 8000;
	info.protocols = (const struct lws_protocols*)myprotocols;
	info.gid = -1;
	info.uid = -1;
	info.pvo = &pvo;
	info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	info.ssl_cert_filepath = "/opt/fmsmoov/fmsmoov.crt";
	info.ssl_private_key_filepath = "/opt/fmsmoov/fmsmoov.key";

	struct lws_context *context = lws_create_context( &info );

	while(false == shutdown)
	{
		lws_service( context, /* timeout_ms = */ 1000000 );
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


/*
 * This runs under both lws service and "spam threads" contexts.
 * Access is serialized by vhd->lock_ring.
 */

static void
__minimal_destroy_message(void *_msg)
{
	struct msg *msg = (struct msg *)_msg;

	free(msg->payload);
	msg->payload = NULL;
	msg->len = 0;
}

/*
 * This runs under the "spam thread" thread context only.
 *
 * We spawn two threads that generate messages with this.
 *
 */

static void *
thread_spam(void *d)
{
	struct per_vhost_data__minimal *vhd =
			(struct per_vhost_data__minimal *)d;
	struct msg amsg;
	int len = 128, index = 1, n, whoami = 0;

	for (n = 0; n < (int)LWS_ARRAY_SIZE(vhd->pthread_spam); n++)
		if (pthread_equal(pthread_self(), vhd->pthread_spam[n]))
			whoami = n + 1;

	do {
		/* don't generate output if nobody connected */
		if (!vhd->pss_list)
			goto wait;

		pthread_mutex_lock(&vhd->lock_ring); /* --------- ring lock { */

		/* only create if space in ringbuffer */
		n = (int)lws_ring_get_count_free_elements(vhd->ring);
		if (!n) {
			lwsl_user("dropping!\n");
			goto wait_unlock;
		}

		amsg.payload = malloc(LWS_PRE + len);
		if (!amsg.payload) {
			lwsl_user("OOM: dropping\n");
			goto wait_unlock;
		}
		n = lws_snprintf((char *)amsg.payload + LWS_PRE, len,
			         "%s: tid: %d, msg: %d", vhd->config,
			         whoami, index++);
		amsg.len = n;
		n = lws_ring_insert(vhd->ring, &amsg, 1);
		if (n != 1) {
			__minimal_destroy_message(&amsg);
			lwsl_user("dropping!\n");
		} else
			/*
			 * This will cause a LWS_CALLBACK_EVENT_WAIT_CANCELLED
			 * in the lws service thread context.
			 */
			lws_cancel_service(vhd->context);

wait_unlock:
		pthread_mutex_unlock(&vhd->lock_ring); /* } ring lock ------- */

wait:
		usleep(100000);

	} while (!vhd->finished);

	lwsl_notice("thread_spam %d exiting\n", whoami);

	pthread_exit(NULL);

	return NULL;
}


int WebsocketServer::callback_server_push_wrapper(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
	WebsocketServer* me = reinterpret_cast<WebsocketServer*>(user);
	return me->callback_server_push(wsi, reason, user, in, len);
}

int WebsocketServer::callback_server_push( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len )
{
	try
	{
	const char* push_msg = "SERVER_PUSH";
	string msgstr("Server computed push message: " + to_string(example_counter));

	cout << "Push socket callback reason: " << reason << endl;
	struct per_session_data__minimal *pss =
			(struct per_session_data__minimal *)user;
	struct per_vhost_data__minimal *vhd =
			(struct per_vhost_data__minimal *) lws_protocol_vh_priv_get(lws_get_vhost(wsi), lws_get_protocol(wsi));
	const struct lws_protocol_vhost_options *pvo;
	const struct msg *pmsg;
	void *retval;
	int n, m, r = 0;

	switch( reason )
	{
		case LWS_CALLBACK_PROTOCOL_INIT:
			cout << "Push protocol init" << endl;
			/* create our per-vhost struct */
			vhd = (struct per_vhost_data__minimal*)lws_protocol_vh_priv_zalloc(lws_get_vhost(wsi),
					lws_get_protocol(wsi),
					sizeof(struct per_vhost_data__minimal));
			if (!vhd)
				return 1;

			pthread_mutex_init(&vhd->lock_ring, NULL);

			/* recover the pointer to the globals struct */
			pvo = lws_pvo_search(
				(const struct lws_protocol_vhost_options *)in,
				"config");
			if (!pvo || !pvo->value) {
				lwsl_err("%s: Can't find \"config\" pvo\n", __func__);
				return 1;
			}
			vhd->config = pvo->value;

			vhd->context = lws_get_context(wsi);
			vhd->protocol = lws_get_protocol(wsi);
			vhd->vhost = lws_get_vhost(wsi);

			vhd->ring = lws_ring_create(sizeof(struct msg), 8,
							__minimal_destroy_message);
			if (!vhd->ring) {
				lwsl_err("%s: failed to create ring\n", __func__);
				return 1;
			}

			/* start the content-creating threads */

			for (n = 0; n < (int)LWS_ARRAY_SIZE(vhd->pthread_spam); n++)
				if (pthread_create(&vhd->pthread_spam[n], NULL,
						   thread_spam, vhd)) {
					lwsl_err("thread creation failed\n");
					r = 1;
					goto init_fail;
				}
			break;
		case LWS_CALLBACK_PROTOCOL_DESTROY:
			cout << "Push protocol destroy" << endl;
			init_fail:
				vhd->finished = 1;
				for (n = 0; n < (int)LWS_ARRAY_SIZE(vhd->pthread_spam); n++)
					if (vhd->pthread_spam[n])
						pthread_join(vhd->pthread_spam[n], &retval);

				if (vhd->ring)
					lws_ring_destroy(vhd->ring);

				pthread_mutex_destroy(&vhd->lock_ring);
				break;
		case LWS_CALLBACK_ESTABLISHED:
			cout << "Push callback established" << endl;
			/* add ourselves to the list of live pss held in the vhd */
			lws_ll_fwd_insert(pss, pss_list, vhd->pss_list);
			pss->tail = lws_ring_get_oldest_tail(vhd->ring);
			pss->wsi = wsi;
			break;

		case LWS_CALLBACK_CLOSED:
			cout << "Push callback closed" << endl;
			/* remove our closing pss from the list of live pss */
			lws_ll_fwd_remove(struct per_session_data__minimal, pss_list,
					  pss, vhd->pss_list);
			break;
		case LWS_CALLBACK_SERVER_WRITEABLE:
			cout << "Push callback server writeable" << endl;
			pthread_mutex_lock(&vhd->lock_ring); /* --------- ring lock { */

			pmsg = (const msg*)lws_ring_get_element(vhd->ring, &pss->tail);
			if (!pmsg) {
				pthread_mutex_unlock(&vhd->lock_ring); /* } ring lock ------- */
				break;
			}

			/* notice we allowed for LWS_PRE in the payload already */
			m = lws_write(wsi, ((unsigned char *)pmsg->payload) + LWS_PRE,
					  pmsg->len, LWS_WRITE_TEXT);
			if (m < (int)pmsg->len) {
				pthread_mutex_unlock(&vhd->lock_ring); /* } ring lock ------- */
				lwsl_err("ERROR %d writing to ws socket\n", m);
				return -1;
			}

			lws_ring_consume_and_update_oldest_tail(
				vhd->ring,	/* lws_ring object */
				struct per_session_data__minimal, /* type of objects with tails */
				&pss->tail,	/* tail of guy doing the consuming */
				1,		/* number of payload objects being consumed */
				vhd->pss_list,	/* head of list of objects with tails */
				tail,		/* member name of tail in objects with tails */
				pss_list	/* member name of next object in objects with tails */
			);

			/* more to do? */
			if (lws_ring_get_element(vhd->ring, &pss->tail))
				/* come back as soon as we can write more */
				lws_callback_on_writable(pss->wsi);

			pthread_mutex_unlock(&vhd->lock_ring); /* } ring lock ------- */
			break;
		case LWS_CALLBACK_EVENT_WAIT_CANCELLED:
			cout << "Push callback event wait cancelled" << endl;
			if (!vhd)
				break;
			/*
			 * When the "spam" threads add a message to the ringbuffer,
			 * they create this event in the lws service thread context
			 * using lws_cancel_service().
			 *
			 * We respond by scheduling a writable callback for all
			 * connected clients.
			 */
			lws_start_foreach_llp(struct per_session_data__minimal **, ppss, vhd->pss_list)
			{
				lws_callback_on_writable((*ppss)->wsi);
			} lws_end_foreach_llp(ppss, pss_list);
			break;
		default:
			break;
	}
	}
	catch (const exception& ex)
	{
		cout << ex.what() << endl;
	}

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
		example_counter++;
	}
}
