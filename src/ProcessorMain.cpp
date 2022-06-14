/*
 * ProcessorMain.cpp
 *
 *  Created on: Jun 13, 2022
 *      Author: zaremba
 */

#include "common_defs.h"
#include "ProcessorMain.h"
#include <iostream>
#include <unistd.h>

#include "spdlog/sinks/stdout_color_sinks.h"


ProcessorMain::ProcessorMain(std::mutex& _mutex_startup, std::condition_variable& _cv_startup, bool& _jack_started) :
		mutex_startup(_mutex_startup), cv_startup(_cv_startup), m_jack_started(_jack_started)
{
	log = spdlog::stdout_color_mt("PROCESSOR");
	log->set_pattern("%^[%H%M%S.%e][%s:%#][%n][%l] %v%$");
	log->set_level(spdlog::level::trace);
	LOGT("ProcessorMain CTOR");
	m_shutdown_signalled = false;
	m_jack_shutdown_complete = false;
}

ProcessorMain::~ProcessorMain() {
	// TODO Auto-generated destructor stub
	LOGT("ProcessorMain DTOR");
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

	LOGI("engine sample rate: {}", jack_get_sample_rate(client));
	LOGI("engine buffer size: {}", jack_get_buffer_size(client));
	LOGI("jack version: {}", jack_get_version_string());
	LOGI("jack current system time: {}", jack_get_time());

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
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 *
 * This client does nothing more than copy data from its input
 * port to its output port. It will exit when stopped by
 * the user (e.g. using Ctrl-C on a unix-ish operating system)
 */
int ProcessorMain::jack_process_callback(jack_nframes_t nframes, void *arg)
{
	if(1024 != nframes)
	{
		//lazy programmer hack.
		//TODO: handle different buffer sizes!
		LOGC("Getting {} frames from JACK but I can only do 1024. This will get fixed in future versions.", nframes);
		exit (1);
	}

	jack_default_audio_sample_t *inL, *inR, *outL, *outR;

	inL = (jack_default_audio_sample_t*)jack_port_get_buffer (input_port_L, nframes);
	outL = (jack_default_audio_sample_t*)jack_port_get_buffer (output_port_L, nframes);

	inR = (jack_default_audio_sample_t*)jack_port_get_buffer (input_port_R, nframes);
	outR = (jack_default_audio_sample_t*)jack_port_get_buffer (output_port_R, nframes);

	return process(inL, inR, outL, outR, nframes);
}

int ProcessorMain::process(float* inL, float* inR, float* outL, float* outR, uint32_t samps)
{
/*	if(true == master_bypass)
	{
		memcpy(outL, inL, samps * sizeof(float));
		memcpy(outR, inR, samps * sizeof(float));
	}
	else
	{
		memcpy(tempaL, inL, samps * sizeof(float));
		memcpy(tempaR, inR, samps * sizeof(float));

		if(false == hpf30Hz_bypass)
		{
			hpf30Hz->process(tempaL, tempaR, tempbL, tempbR, samps);
			memcpy(tempaL, tempbL, samps * sizeof(float));
			memcpy(tempaR, tempbR, samps * sizeof(float));
		}

		if(false == phase_rotator_bypass)
		{
			phase_rotator->process(tempaL, tempaR, tempbL, tempbR, samps);
			memcpy(tempaL, tempbL, samps * sizeof(float));
			memcpy(tempaR, tempbR, samps * sizeof(float));
		}

		if(false == stereo_enh_bypass)
		{
			stereo_enh->process(tempaL, tempaR, tempbL, tempbR, samps);
			memcpy(tempaL, tempbL, samps * sizeof(float));
			memcpy(tempaR, tempbR, samps * sizeof(float));
		}

		if(false == ws_agc_bypass)
		{
			ws_agc->process(tempaL, tempaR, tempbL, tempbR, samps);
			memcpy(tempaL, tempbL, samps * sizeof(float));
			memcpy(tempaR, tempbR, samps * sizeof(float));
		}

		if(false == agc2b_bypass)
		{
			agc2b->process(tempaL, tempaR, tempbL, tempbR, samps);
			memcpy(tempaL, tempbL, samps * sizeof(float));
			memcpy(tempaR, tempbR, samps * sizeof(float));
		}

		if(false == limiter6b_bypass)
		{
			limiter6b->process(tempaL, tempaR, tempbL, tempbR, samps);
			memcpy(tempaL, tempbL, samps * sizeof(float));
			memcpy(tempaR, tempbR, samps * sizeof(float));
		}

		if(false == lpf15kHz_bypass)
		{
			lpf15kHz->process(tempaL, tempaR, tempbL, tempbR, samps);
			memcpy(tempaL, tempbL, samps * sizeof(float));
			memcpy(tempaR, tempbR, samps * sizeof(float));
		}

		if(false == delay_line_bypass)
		{
			delay_line->process(tempaL, tempaR, tempbL, tempbR, samps);
			memcpy(tempaL, tempbL, samps * sizeof(float));
			memcpy(tempaR, tempbR, samps * sizeof(float));
		}

		gain_final->process(tempaL, tempaR, tempaL, tempaR, samps);

		memcpy(outL, tempaL, samps * sizeof(float));
		memcpy(outR, tempaR, samps * sizeof(float));

	//	sig_gen->process(outL, outR, samps);

	//	fft_out->process(outL, outR, fftL, fftR, samps);
	}

#ifdef CAPTURE_TAP
	fwrite(fftL, sizeof(float), samps/2, capture_tap);
#endif
*/
	return 0;
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

