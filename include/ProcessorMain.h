/*
 * ProcessorMain.h
 *
 *  Created on: Jun 13, 2022
 *      Author: zaremba
 */

#ifndef PROCESSORMAIN_H_
#define PROCESSORMAIN_H_

#include <string>
#include <mutex>
#include <condition_variable>

#include "common_defs.h"
#include "spdlog/spdlog.h"
#include <jack/jack.h>

#include "ProcessorPlugin.h"
#include "processor_live_data.h"
#include "plugin_config.h"
#include "audiobuf.h"

#include "fmsmoov.pb.h"

class CommandServer;

using namespace std;

class ProcessorMain {
public:
	ProcessorMain(std::mutex& _mutex_startup, std::condition_variable& _cv_startup, bool& _jack_started);
	virtual ~ProcessorMain();
	void stop();
	void operator ()(string params);

	void set_cmd_server(CommandServer* cmd_server);

	void setMasterBypass(bool bypass);
	bool getMasterBypass();

	void get_audio_params(uint32_t& _sample_rate, uint32_t& bufsize);

private:
	void start_jack();
	void stop_jack();
	int process();

	static int jack_process_callback_wrapper(jack_nframes_t nframes, void *arg);
	static void jack_shutdown_wrapper(void* arg);
	int jack_process_callback(jack_nframes_t nframes, void *arg);
	void jack_shutdown_callback(void* arg);

	static void jack_error_log_function_wrapper(const char* msg);
	static void jack_info_log_function_wrapper(const char* msg);

	//for debug purposes, to see if the main buffers are all aligned
	void dump_buffer_addrs();


private:
	static const uint32_t JACK_INTERFACE_CHANNELS = 2; //TODO: magic number fix?
	std::shared_ptr<spdlog::logger> log;

	CommandServer* m_cmd_server;
	fmsmoov::ProcessorLiveData m_pld;
	ProcessorPlugin* m_plug_meter_in;
	ProcessorPlugin* m_plug_meter_out;
	bool m_master_bypass;

	vector<AudioBuf*>* m_jackbufs_in;
	vector<AudioBuf*>* m_jackbufs_out;
	uint32_t m_requested_sample_rate;
	uint32_t m_requested_bufsize;
	uint32_t m_jack_sample_rate;
	uint32_t m_jack_buffer_size;

	const char **ports;
	jack_port_t *input_port_L;
	jack_port_t *input_port_R;
	jack_port_t *output_port_L;
	jack_port_t *output_port_R;
	jack_client_t *client;
	const char *client_name = "FMsmoov";
	const char *server_name = "default";
	jack_options_t options = JackUseExactName;
	jack_status_t status;

	bool m_shutdown_signalled;
	bool m_jack_shutdown_complete;
	std::mutex mutex_shutdown;
	std::condition_variable cv_shutdown;

	std::mutex& mutex_startup;
	std::condition_variable& cv_startup;
	bool& m_jack_started;
};

#endif /* PROCESSORMAIN_H_ */
