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


using namespace std;

class ProcessorMain {
public:
	ProcessorMain(std::mutex& _mutex_startup, std::condition_variable& _cv_startup, bool& _jack_started);
	virtual ~ProcessorMain();
	void stop();
	void operator ()(string params);
private:
	void start_jack();
	void stop_jack();
	int process(float* inL, float* inR, float* outL, float* outR, uint32_t samps);

	static int jack_process_callback_wrapper(jack_nframes_t nframes, void *arg);
	static void jack_shutdown_wrapper(void* arg);
	int jack_process_callback(jack_nframes_t nframes, void *arg);
	void jack_shutdown_callback(void* arg);

	static void jack_error_log_function_wrapper(const char* msg);
	static void jack_info_log_function_wrapper(const char* msg);


private:
	std::shared_ptr<spdlog::logger> log;
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
