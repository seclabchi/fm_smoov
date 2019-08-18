var fms_sock = null;
var srq_timer = null;

$( document ).ready( setup_page );

function setup_page()
{
  $( "#slider-linpeak" ).slider({
      orientation: "vertical",
      range: "min",
      min: -30,
      max: 0,
      value: -30,
      change: slider_changed
  });
  
  $( "#slider-linpeak-val" ).val(-30);
  
  
  $( "#slider-rinpeak" ).slider({
      orientation: "vertical",
      range: "min",
      min: -30,
      max: 0,
      value: -30,
      change: slider_changed
  });
  
  $( "#slider-rinpeak-val" ).val(-30);
  
  
  $( "#slider-linrms" ).slider({
      orientation: "vertical",
      range: "min",
      min: -30,
      max: 0,
      value: -30,
      change: slider_changed
  });
  
  $( "#slider-linrms-val" ).val(-30);
  
  
  $( "#slider-rinrms" ).slider({
      orientation: "vertical",
      range: "min",
      min: -30,
      max: 0,
      value: -30,
      change: slider_changed
  });
  
  $( "#slider-rinrms-val" ).val(-30);
  
  
  $( "#slider-loutpeak" ).slider({
      orientation: "vertical",
      range: "min",
      min: -30,
      max: 0,
      value: -30,
      change: slider_changed
  });
  
  $( "#slider-loutpeak-val" ).val(-30);
  
  
  $( "#slider-routpeak" ).slider({
      orientation: "vertical",
      range: "min",
      min: -30,
      max: 0,
      value: -30,
      change: slider_changed
  });
  
  $( "#slider-routpeak-val" ).val(-30);
  
  
  $( "#slider-loutrms" ).slider({
      orientation: "vertical",
      range: "min",
      min: -30,
      max: 0,
      value: -30,
      change: slider_changed
  });
  
  $( "#slider-loutrms-val" ).val(-30);
  
  
  $( "#slider-routrms" ).slider({
      orientation: "vertical",
      range: "min",
      min: -30,
      max: 0,
      value: -30,
      change: slider_changed
  });
  
  $( "#slider-routrms-val" ).val(-30);
}

function slider_changed(ui,event)
{
  slider_id = ui.target.id;
  
  value = $("#" + slider_id).slider('option','value');
  var color;
  
  if(value < -15)
  {
    color = "darkgreen";
  }
  else if(value >= -15 && value < -6)
  {
    color = "green";
  }
  else if(value >= -6 && value < -2)
  {
    color = "lightgreen";
  }
  else if(value >= -2)
  {
    color = "red";
  }
  
  $( "#" + slider_id + " .ui-slider-range" ).css( "background", color);
}
/*function vert_slider_changed( event, ui ) 
{
  $( "#vert_slider_val" ).val( ui.value );
  
  if(null != fms_sock)
  {
    fms_sock.send("Vertical slider change to " + ui.value + " received by FM SMOOV.");
  }
}
* */

function open_cb()
{
	console.log("open_cb");
	$("#status_text").text("Connected to FM SMOOV");
	$("#button_connect_disconnect").attr("value", "DISCONNECT FROM FM SMOOV");
	$("#button_connect_disconnect").attr("onclick","disconnect()");
	fms_sock.send("HLO"); 
	$("#button_get_status").prop("disabled", false);
	srq_timer = setInterval(send_srq, 250);
}

function close_cb(event)
{
	console.log("close_cb");
	$("#status_text").text("Disconnected from FM SMOOV");
	$("#button_connect_disconnect").attr("value", "CONNECT TO FM SMOOV");
	$("#button_connect_disconnect").attr("onclick","connect()");
	fms_sock = null;
	$("#button_get_status").prop("disabled", true);
}

function onmsg_cb(event)
{
//  console.log("onmsg_cb");
//  $("#recvdata").text(event.data);
  
  msg_comp = event.data.split(" ");
  
  if(msg_comp[0] == 'INL')
  {
    process_in_level(msg_comp);
  }
  if(msg_comp[0] == 'OUL')
  {
    process_out_level(msg_comp);
  }
  
  /*if(typeof event.data === String) 
  {
    console.log("Received data string");
    $("#recvdata").text($("#recvdata").text() + "\n" + event.data);
  }

  if(event.data instanceof ArrayBuffer){
    var buffer = event.data;
    console.log("Received arraybuffer");
    $("#recvdata").text($("#recvdata").text() + "\n" + event.data);
  }
  * */
}
	
function connect()
{
	fms_sock = new WebSocket("ws://lab.tonekids.com/fmsmoov_frontend/");
	fms_sock.onopen = open_cb;
	fms_sock.onclose = close_cb;
	fms_sock.onmessage = onmsg_cb;
}

function disconnect() 
{
	clearInterval(srq_timer);
	fms_sock.send("XIT"); 
	fms_sock.close();
	fms_sock = null;
}

function send_srq()
{
  if(null != fms_sock)
  {
    fms_sock.send("SRQ"); 
  }
}

function process_in_level(msg)
{
  lpeak = msg[1];
  lrms = msg[2];
  rpeak = msg[3];
  rrms = msg[4];
  
  $( "#slider-linpeak" ).slider('value', lpeak);
  $( "#slider-linpeak-val" ).val(lpeak);
  $( "#slider-rinpeak" ).slider('value', rpeak);
  $( "#slider-rinpeak-val" ).val(rpeak);
  $( "#slider-linrms" ).slider('value', lrms);
  $( "#slider-linrms-val" ).val(lrms);
  $( "#slider-rinrms" ).slider('value', rrms);
  $( "#slider-rinrms-val" ).val(rrms);
}

function process_out_level(msg)
{
  lpeak = msg[1];
  lrms = msg[2];
  rpeak = msg[3];
  rrms = msg[4];
  
  $( "#slider-loutpeak" ).slider('value', lpeak);
  $( "#slider-loutpeak-val" ).val(lpeak);
  $( "#slider-routpeak" ).slider('value', rpeak);
  $( "#slider-routpeak-val" ).val(rpeak);
  $( "#slider-loutrms" ).slider('value', lrms);
  $( "#slider-loutrms-val" ).val(lrms);
  $( "#slider-routrms" ).slider('value', rrms);
  $( "#slider-routrms-val" ).val(rrms);
}
