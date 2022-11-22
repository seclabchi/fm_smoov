/*
 * ProcessorMain.cpp
 *
 *  Created on: Jun 13, 2022
 *      Author: zaremba
 */

#include "common_defs.h"
#include "ProcessorMain.h"
#include <iostream>
#include <sstream>
#include <unistd.h>

#include "spdlog/sinks/stdout_color_sinks.h"
#include "command_server.h"

ProcessorMain::ProcessorMain(std::mutex& _mutex_startup, std::condition_variable& _cv_startup, bool& _jack_started) :
		mutex_startup(_mutex_startup), cv_startup(_cv_startup), m_jack_started(_jack_started)
{
	log = spdlog::stdout_color_mt("PROCESSOR");
	log->set_pattern("%^[%H%M%S.%e][%s:%#][%n][%l] %v%$");
	log->set_level(spdlog::level::trace);
	LOGT("ProcessorMain CTOR");
	m_shutdown_signalled = false;
	m_jack_shutdown_complete = false;
	m_jackbufs_in = NULL;
	m_jackbufs_out = NULL;
	m_proc_count = 0;
	m_cmd_server = nullptr;
	m_core = nullptr;
}

ProcessorMain::~ProcessorMain() {
	LOGT("ProcessorMain DTOR");

	for(AudioBuf* ab : *m_jackbufs_in) {
		delete ab;
	}

	delete m_jackbufs_in;

	for(AudioBuf* ab : *m_jackbufs_out) {
		delete ab;
	}

	delete m_jackbufs_out;
}

void ProcessorMain::set_cmd_server(CommandServer* cmd_server) {
	m_cmd_server = cmd_server;
}

void ProcessorMain::stop() {

	/* Set shutdown flag and notify thread */
	{
		std::lock_guard lk(mutex_shutdown);
		m_shutdown_signalled = true;
		LOGD("ProcessorMain shutdown signal is set.");
	}
	cv_shutdown.notify_one();

	/* Wait for thread to finish shutting down jack */
	{
		LOGD("Waiting for ProcessorMain shutdown...");
		std::unique_lock lk(mutex_shutdown);
		cv_shutdown.wait(lk, [&]{return m_jack_shutdown_complete;});
	}

	LOGD("ProcessorMain shutdown is complete.");

}

void ProcessorMain::operator ()(string params) {
	std::unique_lock lk(mutex_shutdown);

	{
		std::lock_guard lkstartup(mutex_startup);
		LOGD("ProcessorMain thread starting jack...");
		start_jack();
		m_jack_started = true;
		LOGD("Jack started.  Notifying thread parent...");
	}
	cv_startup.notify_one();

	LOGD("ProcessorMain waiting for shutdown signal...");

	cv_shutdown.wait(lk, [&]{return m_shutdown_signalled;});

	LOGD("ProcessorMain got shutdown signal.  Shutting down jack...");

	stop_jack();

	m_jack_shutdown_complete = true;
	LOGD("ProcessorMain jack shutdown complete.  Notifying thread parent...");

	lk.unlock();
	cv_shutdown.notify_one();

	LOGD("Exiting ProcessorMain thread func");
	return;
}

//for debug purposes, to see if the main buffers are all aligned
void ProcessorMain::dump_buffer_addrs() {
	std::vector<AudioBuf*> inbuf;
	std::vector<AudioBuf*> outbuf;
}

void ProcessorMain::start_jack() {
	LOGI("Starting jack...");
	jack_set_error_function(jack_error_log_function_wrapper);
	jack_set_info_function(jack_error_log_function_wrapper);

	client = jack_client_open (client_name, options, &status);
	if (client == NULL) {
		LOGC("jack_client_open() failed, status = 0x{%2.0x}", status);
		if (status & JackServerFailed) {
			LOGC("Unable to connect to JACK server");
		}
		exit (1);
	}
	if (status & JackServerStarted) {
		LOGI("JACK server started");
	}
	if (status & JackNameNotUnique) {
		client_name = jack_get_client_name(client);
		LOGI("unique name '{}' assigned", client_name);
	}

	/* tell the JACK server to call `process()' whenever
	   there is work to be done.
	*/

	LOGI("jack client is open, setting process callback...");

	jack_set_process_callback (client, jack_process_callback_wrapper, this);

	/* tell the JACK server to call `jack_shutdown()' if
	   it ever shuts down, either entirely, or if it
	   just decides to stop calling us.
	*/

	jack_on_shutdown(client, jack_shutdown_wrapper, this);

	/* display the current sample rate.
	 */

	m_jack_sample_rate = jack_get_sample_rate(client);
	LOGI("engine sample rate: {}", jack_get_sample_rate(client));
	m_jack_buffer_size = jack_get_buffer_size(client);
	LOGI("engine buffer size: {}", m_jack_buffer_size);
	LOGI("jack version: {}", jack_get_version_string());
	LOGI("jack current system time: {}", jack_get_time());

	/* create the processor core */
	m_core = new ProcessorCore(m_jack_sample_rate, m_jack_buffer_size);

	/* create two ports */

	input_port_L = jack_port_register (client, "fmsmoov_inputL",
					 "32 bit float mono audio",
					 JackPortIsInput, 0);
	input_port_R = jack_port_register (client, "fmsmoov_inputR",
					 "32 bit float mono audio",
					 JackPortIsInput, 0);
	output_port_L = jack_port_register (client, "fmsmoov_outputL",
					 "32 bit float mono audio",
					  JackPortIsOutput, 0);
	output_port_R = jack_port_register (client, "fmsmoov_outputR",
					 "32 bit float mono audio",
					  JackPortIsOutput, 0);

	if ((input_port_L == NULL) || (output_port_L == NULL) || (input_port_R == NULL)|| (output_port_R == NULL)) {
		LOGC("no more JACK ports available\n");
		exit (1);
	}

	/* Initialize the master jack buffer interface to the processor */
	m_jackbufs_in = new vector<AudioBuf*>();
	m_jackbufs_out = new vector<AudioBuf*>();

	std::stringstream bufname;

	for(uint32_t i = 0; i < JACK_INTERFACE_CHANNELS; i++) {
		bufname.str("");
		bufname << "JACK_IN_" << i;
		m_jackbufs_in->push_back(new AudioBuf(bufname.str(), AudioBuf::AUDIO_BUF_TYPE::REFERENCE, m_jack_buffer_size, NULL));
		bufname.str("");
		bufname << "JACK_OUT_" << i;
		m_jackbufs_out->push_back(new AudioBuf(bufname.str(), AudioBuf::AUDIO_BUF_TYPE::REFERENCE, m_jack_buffer_size, NULL));
	}

	/* Tell the JACK server that we are ready to roll.  Our
	 * process() callback will start running now. */

	if (jack_activate (client)) {
		LOGC("cannot activate client");
		exit (1);
	}

	/* Connect the ports.  You can't do this before the client is
	 * activated, because we can't make connections to clients
	 * that aren't running.  Note the confusing (but necessary)
	 * orientation of the driver backend ports: playback ports are
	 * "input" to the backend, and capture ports are "output" from
	 * it.
	 */

	ports = jack_get_ports (client, NULL, NULL,
				JackPortIsPhysical|JackPortIsOutput);
	if (ports == NULL) {
		LOGC("no physical capture ports\n");
		exit (1);
	}

	if (jack_connect (client, ports[0], jack_port_name (input_port_L))) {
		LOGC("cannot connect input port 0\n");
	}

	if (jack_connect (client, ports[1], jack_port_name (input_port_R))) {
		LOGC("cannot connect input port 1\n");
	}


	jack_free (ports);

	ports = jack_get_ports (client, NULL, NULL,
				JackPortIsPhysical|JackPortIsInput);
	if (ports == NULL) {
		LOGC("no physical playback ports\n");
		exit (1);
	}

	if (jack_connect (client, jack_port_name (output_port_L), ports[0])) {
		LOGC("cannot connect output port 0\n");
	}

	if (jack_connect (client, jack_port_name (output_port_R), ports[1])) {
		LOGC("cannot connect output port 1\n");
	}

	jack_free (ports);

	LOGI("Jack is started.");
}

void ProcessorMain::stop_jack()
{
	LOGI("Disconnecting from and shutting down jack client...");
	jack_port_disconnect(client, input_port_L);
	jack_port_disconnect(client, input_port_R);
	jack_port_disconnect(client, output_port_L);
	jack_port_disconnect(client, output_port_R);

	jack_deactivate(client);
	jack_client_close(client);
	LOGI("jack is shutdown.");
}

int ProcessorMain::jack_process_callback_wrapper(jack_nframes_t nframes, void *arg)
{
	ProcessorMain* p = reinterpret_cast<ProcessorMain*>(arg);
	return p->jack_process_callback(nframes, arg);
}

/**
 * Process callback when an audio buffer is ready for processing.  Called on a
 * realtime thread for each buffer of frames available.
 */
int ProcessorMain::jack_process_callback(jack_nframes_t nframes, void *arg)
{
	if(1024 != nframes)
	{
		//lazy programmer hack.
		//TODO: handle different buffer sizes!  This will require a bounce of
		//the plugin stack.  I've added enough infrastructure where I can
		//probably pull this off.
		LOGC("Getting {} frames from JACK but I can only do 1024. This will get fixed in future versions.", nframes);
		exit (1);
	}

	//jack_default_audio_sample_t *inL, *inR, *outL, *outR;

	m_jackbufs_in->at(0)->setptr((jack_default_audio_sample_t*)jack_port_get_buffer (input_port_L, nframes), m_jack_buffer_size);
	m_jackbufs_out->at(0)->setptr((jack_default_audio_sample_t*)jack_port_get_buffer (output_port_L, nframes), m_jack_buffer_size);

	m_jackbufs_in->at(1)->setptr((jack_default_audio_sample_t*)jack_port_get_buffer (input_port_R, nframes), m_jack_buffer_size);
	m_jackbufs_out->at(1)->setptr((jack_default_audio_sample_t*)jack_port_get_buffer (output_port_R, nframes), m_jack_buffer_size);

	return process();
}

int ProcessorMain::process()
{
	int retval = 0;

	if(nullptr != m_core) {
		m_core->set_jack_inbufs(m_jackbufs_in);
		m_core->set_jack_outbufs(m_jackbufs_out);
		m_core->process();
		m_core->get_live_data(m_pld);
	}
	else {
		LOGE("m_core is NULL");
		retval = -1;
	}

	if(nullptr != m_cmd_server) {
		m_cmd_server->publish_live_data(m_pld);
	}
	else {
		LOGE("m_cmd_server is NULL");
		retval = -2;
	}

	m_proc_count++;

#ifdef CAPTURE_TAP
	fwrite(fftL, sizeof(float), samps/2, capture_tap);
#endif

	return retval;
}

void ProcessorMain::jack_shutdown_wrapper(void* arg)
{
	ProcessorMain* p = reinterpret_cast<ProcessorMain*>(arg);
	p->jack_shutdown_callback(arg);
}

void ProcessorMain::jack_shutdown_callback(void* arg)
{
	cout << "JACK SHUTDOWN CALLBACK:  THIS IS ABNORMAL." << endl;
	exit (1);
}

void ProcessorMain::jack_error_log_function_wrapper(const char* msg) {
	SPDLOG_ERROR(msg);
}

void ProcessorMain::jack_info_log_function_wrapper(const char* msg) {
	SPDLOG_INFO(msg);
}

void ProcessorMain::get_audio_params(uint32_t& _sample_rate, uint32_t& _bufsize) {
	_sample_rate = m_jack_sample_rate;
	_bufsize = m_jack_buffer_size;
}

void ProcessorMain::handle_command(const fmsmoov::ProcessorCommand& cmd) {
	fmsmoov::ResponseCode rsp = fmsmoov::ResponseCode::OK;
	fmsmoov::ProcessorResponse rspproto;

	if(cmd.has_internal_shutdown_cmd()) {
		rspproto.set_response_msg("Goodbye!");
	}
	else if(cmd.has_master_bypass_set()) {
		fmsmoov::MasterBypassSetResponse rsp = m_core->set_master_bypass(cmd.master_bypass_set());
		rspproto.mutable_master_bypass_set_rsp()->CopyFrom(rsp);
	}
	else if(cmd.has_master_bypass_get()) {
		fmsmoov::MasterBypassGetResponse rsp = m_core->get_master_bypass(cmd.master_bypass_get());
		rspproto.mutable_master_bypass_get_rsp()->CopyFrom(rsp);
	}
	else if(cmd.has_gain_set()) {
		fmsmoov::GainSetResponse rsp = m_core->set_gain(cmd.gain_set());
		rspproto.mutable_gain_set_rsp()->CopyFrom(rsp);
	}
	else if(cmd.has_gain_get()) {
		fmsmoov::GainGetResponse rsp = m_core->get_gain(cmd.gain_get());
		rspproto.mutable_gain_get_rsp()->CopyFrom(rsp);
	}
	else if(cmd.has_agc_set()) {
		fmsmoov::AGCSetResponse rsp = m_core->set_agc(cmd.agc_set());
		rspproto.mutable_agc_set_rsp()->CopyFrom(rsp);
	}
	else if(cmd.has_agc_get()) {
		fmsmoov::AGCGetResponse rsp = m_core->get_agc(cmd.agc_get());
		rspproto.mutable_agc_get_rsp()->CopyFrom(rsp);
	}
	else if(cmd.has_phase_rotator_set()) {
		fmsmoov::PhaseRotatorSetResponse rsp = m_core->set_phase_rotator(cmd.phase_rotator_set());
		rspproto.mutable_phase_rotator_set_rsp()->CopyFrom(rsp);
	}
	else if(cmd.has_phase_rotator_get()){
		fmsmoov::PhaseRotatorGetResponse rsp = m_core->get_phase_rotator(cmd.phase_rotator_get());
		rspproto.mutable_phase_rotator_get_rsp()->CopyFrom(rsp);
	}
	else if(cmd.has_stereo_enhance_set()) {
		fmsmoov::StereoEnhanceSetResponse rsp = m_core->set_stereo_enhance(cmd.stereo_enhance_set());
		rspproto.mutable_stereo_enhance_set_rsp()->CopyFrom(rsp);
	}
	else if(cmd.has_stereo_enhance_get()) {
		fmsmoov::StereoEnhanceGetResponse rsp = m_core->get_stereo_enhance(cmd.stereo_enhance_get());
		rspproto.mutable_stereo_enhance_get_rsp()->CopyFrom(rsp);
	}
	else if(cmd.has_bass_enhancer_set()) {
		fmsmoov::BassEnhancerSetResponse rsp = m_core->set_bass_enhancer(cmd.bass_enhancer_set());
		rspproto.mutable_bass_enhancer_set_rsp()->CopyFrom(rsp);
	}
	else if(cmd.has_bass_enhancer_get()) {
		fmsmoov::BassEnhancerGetResponse rsp = m_core->get_bass_enhancer(cmd.bass_enhancer_get());
		rspproto.mutable_bass_enhancer_get_rsp()->CopyFrom(rsp);
	}
	else if(cmd.has_complim_set()) {
		fmsmoov::CompLimSetResponse rsp = m_core->set_compressor_limiter(cmd.complim_set());
		rspproto.mutable_complim_set_rsp()->CopyFrom(rsp);
	}
	else if(cmd.has_complim_get()) {
		fmsmoov::CompLimGetResponse rsp = m_core->get_compressor_limiter(cmd.complim_get());
		rspproto.mutable_complim_get_rsp()->CopyFrom(rsp);
	}
	else if(cmd.has_delay_set()) {
		fmsmoov::DelaySetResponse rsp = m_core->set_delay(cmd.delay_set());
		rspproto.mutable_delay_set_rsp()->CopyFrom(rsp);
	}
	else {
		LOGW("Received unknown command from smoovcontrol.");
		rsp = fmsmoov::ResponseCode::ERROR;
		rspproto.set_response_msg("Unknown command.");
	}

	rspproto.set_response(rsp);
	m_cmd_server->send_reply(rspproto);

}

