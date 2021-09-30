import $ from 'jquery';
window.jQuery = $;
window.$ = $;

import 'jquery-ui';
//import './person_pb.js';

require("jquery-ui/ui/widgets/spinner");
var protobuf = require("protobufjs");

var websock_sub = null;
var websock_pub = null;
var logtext = "";

import { load } from "protobufjs"; // respectively "./node_modules/protobufjs"

var message, buffer, decoded;
var CommandMessage;

  $( function() {
    var spinner = $( "#spinner" ).spinner({
	val: 0.0,
	min: 0.0,
	max: 240.0,
	numberFormat: "n",
	page: 0.1,
	step: 0.1
	});
	
	spinner.val(0.0);
 
    $( "#disable" ).on( "click", function() {
      if ( spinner.spinner( "option", "disabled" ) ) {
        spinner.spinner( "enable" );
      } else {
        spinner.spinner( "disable" );
      }
    });
    $( "#destroy" ).on( "click", function() {
      if ( spinner.spinner( "instance" ) ) {
        spinner.spinner( "destroy" );
      } else {
        spinner.spinner();
      }
    });
    $( "#getvalue" ).on( "click", function() {
      alert( spinner.spinner( "value" ) );
    });
    $( "#setvalue" ).on( "click", function() {
      spinner.spinner( "value", 5 );
    });
 
    $( "button" ).button();
    
    $( "#set_delay" ).on( "click", function() {
      if ( spinner.spinner( "instance" ) ) {
        //send_msg(websock_pub, "DELAY," + spinner.spinner("value"));
        send_msg(websock_pub, create_msg_delay_set(spinner.spinner("value")));
      } else {
        spinner.spinner();
      }
    });
  } );

function log_msg(msg)
{
	logtext += msg + "\n";
	$("#logwindow").val(logtext);
	$("#logwindow").scrolltop = $("#logwindow").scrollheight;
	console.log(msg);
}

function get_appropriate_ws_url(extra_url)
{
	var pcol;
	var u = document.URL;

	/*
	 * We open the websocket encrypted if this page came on an
	 * https:// url itself, otherwise unencrypted
	 */

	if (u.substring(0, 5) === "https") {
		pcol = "wss://";
		u = u.substr(8);
	} else {
		pcol = "ws://";
		if (u.substring(0, 4) === "http")
			u = u.substr(7);
	}

	u = u.split("/");

	/* + "/xxx" bit is for IE10 workaround */

	return pcol + u[0] + "/" + extra_url;
}

function ws_opened(event)
{
	var ws = event.target;
	log_msg("Websocket opened to URL " + ws.url);
	if(ws.name === "websock_pub") {
		send_msg(ws, create_ping_req());	
	}
}

function ws_msgrcvd(event)
{
	var ws = event.target;
    var rxBuf = new Uint8Array(event.data);
    decoded = CommandMessage.decode(rxBuf);
    log_msg(`RX msg decoded = ${JSON.stringify(decoded)}`);
    
}

function ws_error(event)
{
	var ws = event.target;
	log_msg("WS Error from" + ws.name + ": " + event.data);
}

function new_ws(name, urlpath, protocol)
{   
	var ws = new WebSocket(urlpath, protocol);
	ws.binaryType = "arraybuffer";
	ws.name = name;
	ws.onopen = ws_opened;
	ws.onmessage = ws_msgrcvd;
	ws.onerror = ws_error;
	
	return ws;
}

function send_msg(ws, msg)
{
	var txbuf = CommandMessage.encode(msg).finish();
	ws.send(txbuf);
}

function create_ping_req()
{
	var msg = CommandMessage.create({type: CommandMessage.CommandType.PING_REQ});
	return msg;
}

function create_msg_delay_set(delay)
{
	var msg = CommandMessage.create({type: CommandMessage.CommandType.DELAY_SET, vals: [delay]});
	return msg;
}

$(document).ready( function() {
	websock_sub = new_ws("websock_sub", get_appropriate_ws_url(""), "lws-minimal-broker");
	websock_pub = new_ws("websock_pub", get_appropriate_ws_url("publisher"), "lws-minimal-broker");
	load("command.proto", function(err, root) {
		  if (err)
			throw err;
		  CommandMessage = root.lookupType("fmsmoov.Command");
    });
		
/*		message = CommandMessage.create();
		message.type = CommandMessage.CommandType.PING_REQ;
		message.vals.push(0.6);
		message.vals.push(2.1);
		console.log(`message = ${JSON.stringify(message)}`);

		buffer = CommandMessage.encode(message).finish();
		//console.log(`buffer = ${Array.prototype.toString.call(buffer)}`);
		var hexstr = "";
		for(var i = 0; i < buffer.length; i++)
		{
		  hexstr += "0x" + buffer[i].toString(16) + " ";
		}
		console.log("buffer = " + hexstr);

		decoded = CommandMessage.decode(buffer);
		console.log(`decoded = ${JSON.stringify(decoded)}`);

		var err = CommandMessage.verify(buffer);
		if (err)
			throw Error(err);
	});
	*/
});

