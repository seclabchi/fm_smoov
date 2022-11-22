/*
 * processor_core.h
 *
 *  Created on: Sep 28, 2022
 *      Author: zaremba
 */

#ifndef PROCESSOR_CORE_H_
#define PROCESSOR_CORE_H_

#include "common_defs.h"
#include "spdlog/spdlog.h"

#include "audiobuf.h"
#include "plugin_meter.h"
#include "plugin_30Hz_hpf.h"
#include "plugin_phase_rotator.h"
#include "plugin_gain.h"
#include "plugin_agc.h"
#include "plugin_stereo_enhance.h"
#include "plugin_bass_enhancer.h"
#include "plugin_crossover.h"
#include "plugin_compressor_limiter.h"
#include "plugin_delay.h"

#include "fmsmoov.pb.h"

using namespace std;

class ProcessorCore {
public:
	ProcessorCore(uint32_t sample_rate, uint32_t buffer_size);
	virtual ~ProcessorCore();
	void set_jack_inbufs(vector<AudioBuf*>* bufs);
	void set_jack_outbufs(vector<AudioBuf*>* bufs);
	void process();
	void get_live_data(fmsmoov::ProcessorLiveData& pld);
	fmsmoov::MasterBypassSetResponse set_master_bypass(const fmsmoov::MasterBypassSetCommand& cmd);
	fmsmoov::MasterBypassGetResponse get_master_bypass(const fmsmoov::MasterBypassGetCommand& cmd);
	fmsmoov::GainSetResponse set_gain(const fmsmoov::GainSetCommand& cmd);
	fmsmoov::GainGetResponse get_gain(const fmsmoov::GainGetCommand& cmd);
	fmsmoov::AGCSetResponse set_agc(const fmsmoov::AGCSetCommand& cmd);
	fmsmoov::AGCGetResponse get_agc(const fmsmoov::AGCGetCommand& cmd);
	fmsmoov::PhaseRotatorSetResponse set_phase_rotator(const fmsmoov::PhaseRotatorSetCommand& cmd);
	fmsmoov::PhaseRotatorGetResponse get_phase_rotator(const fmsmoov::PhaseRotatorGetCommand& cmd);
	fmsmoov::StereoEnhanceSetResponse set_stereo_enhance(const fmsmoov::StereoEnhanceSetCommand& cmd);
	fmsmoov::StereoEnhanceGetResponse get_stereo_enhance(const fmsmoov::StereoEnhanceGetCommand& cmd);
	fmsmoov::BassEnhancerSetResponse set_bass_enhancer(const fmsmoov::BassEnhancerSetCommand& cmd);
	fmsmoov::BassEnhancerGetResponse get_bass_enhancer(const fmsmoov::BassEnhancerGetCommand& cmd);
	fmsmoov::CompLimSetResponse set_compressor_limiter(const fmsmoov::CompLimSetCommand& cmd);
	fmsmoov::CompLimGetResponse get_compressor_limiter(const fmsmoov::CompLimGetCommand& cmd);
	fmsmoov::DelaySetResponse set_delay(const fmsmoov::DelaySetCommand& cmd);

private:
	ProcessorCore();

	std::shared_ptr<spdlog::logger> log;

	uint32_t m_sample_rate;
	uint32_t m_buffer_size;
	fmsmoov::ProcessorLiveData m_pld;

	bool m_master_bypass;

	/* Plugin Registry */
	PluginMeter* m_meter_main_in;
	Plugin30HzHpf* m_30Hz_hpf;
	PluginPhaseRotator* m_phase_rotator;
	PluginGain* m_gain_input;
	PluginMeter* m_meter_post_input_gain;
	PluginAGC* m_agc_front;
	PluginStereoEnhance* m_stereo_enhance;
	PluginBassEnhancer* m_bass_enhancer;
	PluginCrossover* m_crossover;
	PluginMeter* m_meter_post_crossover_b0;
	PluginMeter* m_meter_post_crossover_b1;
	PluginMeter* m_meter_post_crossover_b2;
	PluginMeter* m_meter_post_crossover_b3;
	PluginMeter* m_meter_post_crossover_b4;
	PluginMeter* m_meter_post_crossover_b5;
	PluginCompressorLimiter* m_complim;
	PluginMeter* m_meter_post_complim_b0;
	PluginMeter* m_meter_post_complim_b1;
	PluginMeter* m_meter_post_complim_b2;
	PluginMeter* m_meter_post_complim_b3;
	PluginMeter* m_meter_post_complim_b4;
	PluginMeter* m_meter_post_complim_b5;
	PluginDelay* m_delay;
	PluginGain* m_gain_output;
	PluginMeter* m_meter_main_out;

	vector<AudioBuf*>* m_jack_inbufs;
	vector<AudioBuf*>* m_jack_outbufs;

	float* m_stack_out_L;
	float* m_stack_out_R;

	float m_main_inlinL, m_main_inlinR, m_main_outlinL, m_main_outlinR;
	float m_main_inlogL, m_main_inlogR, m_main_outlogL, m_main_outlogR;

	float m_lev_precomp_b0L, m_lev_precomp_b0R, m_lev_precomp_b1L, m_lev_precomp_b1R;
	float m_lev_precomp_b2L, m_lev_precomp_b2R, m_lev_precomp_b3L, m_lev_precomp_b3R;
	float m_lev_precomp_b4L, m_lev_precomp_b4R, m_lev_precomp_b5L, m_lev_precomp_b5R;

	float m_complev_b0, m_complev_b1, m_complev_b2, m_complev_b3, m_complev_b4, m_complev_b5;

	float m_lev_postcomp_b0L, m_lev_postcomp_b0R, m_lev_postcomp_b1L, m_lev_postcomp_b1R;
	float m_lev_postcomp_b2L, m_lev_postcomp_b2R, m_lev_postcomp_b3L, m_lev_postcomp_b3R;
	float m_lev_postcomp_b4L, m_lev_postcomp_b4R, m_lev_postcomp_b5L, m_lev_postcomp_b5R;
};

#endif /* PROCESSOR_CORE_H_ */
