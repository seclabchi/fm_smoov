
var socket; 

function update(msg) { svr_msg_rcvd(msg); }

function websocket_opened() { 
	console.log("socket open"); update("connection opened")
	//$("#connect_button").html("DISCONNECT");
	connected = true;
}

function websocket_closed() { 
	console.log("socket close"); update("connection closed");
	//$("#connect_button").html("CONNECT"); 
	connected = false;
}

function svr_msg_rcvd(msg) {
}

function sync_socket_msg(msg) {
	console.log("socket message: " + msg.data); 
	update(msg.data);
}
  
function connection_button_pressed() {
	console.log("Connection button pressed.");
	if(false == connected)
	{
		socket = new WebSocket( "wss://lab.tonekids.com:2324", "example-protocol" );
//		pushsocket = new WebSocket("wss://lab.tonekids.com:2324", "server-push-protocol");
//		pushsocket.onopen = pushsocket_opened;
//		pushsocket.onclose = pushsocket_closed;
//		pushsocket.onmessage = function(msg) { push_msg_received(msg.data);}
		socket.onopen = websocket_opened;
		socket.onclose = websocket_closed;
		socket.onmessage = function(msg) {sync_socket_msg(msg.data);}
	}
	else
	{
		socket.close();
//		pushsocket.close();
	}
}