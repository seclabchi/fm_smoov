var connected = false;  
var socket; 
var specan;
var specan_ctx;

function update(msg) { svr_msg_rcvd(msg); }

function websocket_opened() { 
	console.log("socket open"); update("connection opened")
	$("#connect_button").html("DISCONNECT");
	connected = true;
}

function websocket_closed() { 
	console.log("socket close"); update("connection closed");
	$("#connect_button").html("CONNECT"); 
	connected = false;
}

$( function() {
    $( "#tabs" ).tabs({
      event: "mouseover",
      activate: function(event,ui){
      	console.log(event.currentTarget.text + " tab activated.");
      	if(true == connected)
      	{
      		socket.send(event.currentTarget.text + " tab activated.");
      	}
      }
    });
    specan = document.getElementById("specan_window");
    specan_ctx = specan.getContext("2d");
    specan_ctx.fillRect(0,0,specan.width, specan.height);
    specan_ctx.strokeStyle = "#00FF00";
    $( "#specan_window" ).mousemove(specan_mousemove);
    specan_ctx.clearRect(0,0,specan.width,specan.height);
	specan_ctx.fillRect(0,0,specan.width, specan.height);
} );

$( function() {
	$("#tabs").tabs("option", {
		heightStyle: "fill"
	});
});

$( function() {
    $( "#stereo-enh-slider" ).slider({
      orientation: "vertical",
      range: false,
      min: 1,
      max: 4.0,
      value: 2.0,
      step: 0.1,
      
      slide: function( event, ui ) {
        $( "#amount" ).val( Number.parseFloat(ui.value).toFixed(1) );
      }
    });
    var value = $( "#stereo-enh-slider" ).slider( "option", "value" );
	$( "#amount" ).val( Number.parseFloat(value).toFixed(1) );
});

function specan_mousemove(event)
{
	specan_ctx.fillRect(0,0,specan.width, specan.height);
	X = event.offsetX;
	Y = event.offsetY;
	specan_ctx.beginPath();
	specan_ctx.moveTo(0,0);
	specan_ctx.lineTo(X,Y);
	specan_ctx.stroke();
}

function draw_spectrum(tox, toy)
{
	ctx.moveTo(0, 0);
	ctx.lineTo(tox, toy);
	ctx.stroke();
}

function svr_msg_rcvd(msg) {
  	
}
  
function connection_button_pressed() {
	console.log("Connection button pressed.");
	if(false == connected)
	{
		socket = new WebSocket( "wss://lab.tonekids.com:2324", "example-protocol" );
		socket.onopen = websocket_opened;
		socket.onclose = websocket_closed;
		socket.onmessage = function(msg) { console.log("socket message: " + msg.data); update(msg.data); }
	}
	else
	{
		socket.close();
	}
}