/*
 * fm_smoov.h
 *
 *  Created on: Jun 5, 2020
 *      Author: zaremba
 */

#ifndef INCLUDE_FM_SMOOV_H_
#define INCLUDE_FM_SMOOV_H_

#include <jack/jack.h>
#include "agc.h"
#include "gain.h"
#include "crossover_twoband.h"
#include "hpf_30hz.h"
#include "lpf_15khz.h"
#include "compressor.h"
#include "limiter_6_band.h"
#include "stereo_enhancer.h"
#include "fft.h"
#include "websocket_server.h"
#include "phase_rotator.h"
#include "signal_generator.h"
#include "agc_2_band.h"
#include "delay_line.h"

#include "CommandHandler.h"

class FMSmoov
{
public:
	FMSmoov();
	virtual ~FMSmoov();
	static int jack_process_callback_wrapper(jack_nframes_t nframes, void *arg);
	static void jack_shutdown_wrapper(void* arg);
	static int command_handler_callback_wrapper(void* arg, char* msg);
	void go();
	void stop();
protected:
	int jack_process_callback(jack_nframes_t nframes, void *arg);
	void jack_shutdown_callback(void* arg);
	int command_handler_callback(char* msg);
private:
	void start_jack();
	void stop_jack();
	int process(float* inL, float* inR, float* outL, float* outR, uint32_t samps);

	bool master_bypass;
	bool hpf30Hz_bypass;
	bool phase_rotator_bypass;
	bool stereo_enh_bypass;
	bool ws_agc_bypass;
	bool agc2b_bypass;
	bool limiter6b_bypass;
	bool lpf15kHz_bypass;
	bool delay_line_bypass;

	float* tempaL, *tempbL;
	float* tempaR, *tempbR;

	LimiterSettings** limiter_settings;
	AGCParams** agc_params;

	const char **ports;
	jack_port_t *input_port_L;
	jack_port_t *input_port_R;
	jack_port_t *output_port_L;
	jack_port_t *output_port_R;
	jack_client_t *client;
	const char *client_name = "FMsmoov";
	const char *server_name = NULL;
	jack_options_t options = JackNullOption;
	jack_status_t status;

	PhaseRotator* phase_rotator;
	AGC* ws_agc;
	AGC2band* agc2b;
	HPF30Hz* hpf30Hz;
	LPF15kHz* lpf15kHz;
	Limiter6band* limiter6b;
	StereoEnhancer* stereo_enh;
	Compressor* final_lim;
	FFT* fft_out;
	Gain* gain_final;
	DelayLine* delay_line;

	WebsocketServer* ws_server;
	SignalGenerator* sig_gen;

	CommandHandler* cmd_handler;
};



#endif /* INCLUDE_FM_SMOOV_H_ */
