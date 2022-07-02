/*
 * fm_smoov.h
 *
 *  Created on: Jun 5, 2020
 *      Author: zaremba
 */

#ifndef INCLUDE_FM_SMOOV_H_
#define INCLUDE_FM_SMOOV_H_

#include <thread>

#include "common_defs.h"
#include "spdlog/spdlog.h"

#include "plugin_meter_passthrough.h"
#include "plugin_gain.h"

#include "ProcessorMain.h"
#include "agc.h"
#include "gain.h"
#include "crossover_twoband.h"
#include "hpf_30hz.h"
#include "lpf_15khz.h"
#include "compressor.h"
#include "limiter_6_band.h"
#include "stereo_enhancer.h"
//#include "fft.h"
//#include "websocket_server.h"
#include "phase_rotator.h"
#include "signal_generator.h"
#include "agc_2_band.h"
#include "delay_line.h"

//#include "CommandHandler.h"

class FMSmoov
{
public:
	FMSmoov();
	virtual ~FMSmoov();

	//static int command_handler_callback_wrapper(void* arg, char* msg);
	void go();
	void stop();
protected:

	//int command_handler_callback(char* msg);
private:

	std::shared_ptr<spdlog::logger> log;

	std::thread* m_thread_audioproc;
	ProcessorMain* m_audioproc;

	PluginMeterPassthrough* m_plug_meter_passthrough;
	PluginGain* m_plug_gain;

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



	PhaseRotator* phase_rotator;
	AGC* ws_agc;
	AGC2band* agc2b;
	HPF30Hz* hpf30Hz;
	LPF15kHz* lpf15kHz;
	Limiter6band* limiter6b;
	StereoEnhancer* stereo_enh;
	Compressor* final_lim;
	//FFT* fft_out;
	Gain* gain_final;
	DelayLine* delay_line;

	//WebsocketServer* ws_server;
	SignalGenerator* sig_gen;

	//CommandHandler* cmd_handler;
};



#endif /* INCLUDE_FM_SMOOV_H_ */
