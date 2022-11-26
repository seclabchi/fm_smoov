/*
 * processor_core.cpp
 *
 *  Created on: Sep 28, 2022
 *      Author: zaremba
 */

#include <processor_core.h>

#include "spdlog/sinks/stdout_color_sinks.h"
#include "Prefs.h"

ProcessorCore::ProcessorCore(uint32_t sample_rate, uint32_t buffer_size) :
					m_sample_rate(sample_rate), m_buffer_size(buffer_size) {
	log = spdlog::stdout_color_mt("CORE");
	log->set_pattern("%^[%H%M%S.%e][%s:%#][%n][%l] %v%$");
	log->set_level(spdlog::level::trace);
	LOGT("ProcessorCore CTOR");

	m_master_bypass = false;

	fmsmoov::PluginConfig cfgdummy;
	fmsmoov::PluginConfig cfg;

	Prefs* prefs = Prefs::get_instance();

	AudioBuf* b;
	m_jack_inbufs = new vector<AudioBuf*>();
	b = new AudioBuf("MAIN_IN_L", AudioBuf::AUDIO_BUF_TYPE::REFERENCE, m_buffer_size, NULL);
	m_jack_inbufs->push_back(b);
	b = new AudioBuf("MAIN_IN_R", AudioBuf::AUDIO_BUF_TYPE::REFERENCE, m_buffer_size, NULL);
	m_jack_inbufs->push_back(b);

	m_jack_outbufs = new vector<AudioBuf*>();
	b = new AudioBuf("MAIN_OUT_L", AudioBuf::AUDIO_BUF_TYPE::REFERENCE, m_buffer_size, NULL);
	m_jack_outbufs->push_back(b);
	b = new AudioBuf("MAIN_OUT_R", AudioBuf::AUDIO_BUF_TYPE::REFERENCE, m_buffer_size, NULL);
	m_jack_outbufs->push_back(b);

	/* TODO: stored settings, so when fmsmoov restarts, it picks up the saved settings */
	m_meter_main_in = new PluginMeter("METER_MAIN_IN", m_sample_rate, m_buffer_size);
	m_meter_main_in->init(cfgdummy);

	m_30Hz_hpf = new Plugin30HzHpf("30Hz_HPF", m_sample_rate, m_buffer_size);
	m_30Hz_hpf->init(cfgdummy);

	m_phase_rotator = new PluginPhaseRotator("PHASE_ROTATOR", m_sample_rate, m_buffer_size);
	m_phase_rotator->init(cfgdummy);

	m_gain_input = new PluginGain("GAIN_INPUT", m_sample_rate, m_buffer_size);
	double gain_input = prefs->get_gain_input();
	cfg.mutable_gain()->set_l(gain_input);
	cfg.mutable_gain()->set_r(gain_input);
	cfg.mutable_gain()->set_enabled(true);
	m_gain_input->init(cfg);

	m_meter_post_input_gain = new PluginMeter("METER_POST_INPUT", m_sample_rate, m_buffer_size);
	m_meter_post_input_gain->init(cfgdummy);

	m_agc_front = new PluginAGC("AGC_FRONT", m_sample_rate, m_buffer_size);
	m_agc_front->init(cfgdummy);

	m_stereo_enhance = new PluginStereoEnhance("STEREO_ENHANCE", m_sample_rate, m_buffer_size);
	m_stereo_enhance->init(cfgdummy);

	m_bass_enhancer = new PluginBassEnhancer("BASS_ENHANCER", m_sample_rate, m_buffer_size);
	m_bass_enhancer->init(cfgdummy);

	m_crossover = new PluginCrossover("CROSSOVER", m_sample_rate, m_buffer_size);
	m_crossover->init(cfgdummy);

	m_meter_post_crossover_b0 = new PluginMeter("METER_POST_CROSSOVER_B0", m_sample_rate, m_buffer_size);
	m_meter_post_crossover_b0->init(cfgdummy);
	m_meter_post_crossover_b1 = new PluginMeter("METER_POST_CROSSOVER_B1", m_sample_rate, m_buffer_size);
	m_meter_post_crossover_b1->init(cfgdummy);
	m_meter_post_crossover_b2 = new PluginMeter("METER_POST_CROSSOVER_B2", m_sample_rate, m_buffer_size);
	m_meter_post_crossover_b2->init(cfgdummy);
	m_meter_post_crossover_b3 = new PluginMeter("METER_POST_CROSSOVER_B3", m_sample_rate, m_buffer_size);
	m_meter_post_crossover_b3->init(cfgdummy);
	m_meter_post_crossover_b4 = new PluginMeter("METER_POST_CROSSOVER_B4", m_sample_rate, m_buffer_size);
	m_meter_post_crossover_b4->init(cfgdummy);
	m_meter_post_crossover_b5 = new PluginMeter("METER_POST_CROSSOVER_B5", m_sample_rate, m_buffer_size);
	m_meter_post_crossover_b5->init(cfgdummy);

	m_complim = new PluginCompressorLimiter("COMPRESSOR_LIMITER", m_sample_rate, m_buffer_size);
	fmsmoov::CompLimConfig* clc = cfg.mutable_complim();
	fmsmoov::CompLimBandConfig* b0cfg = clc->mutable_b0cfg();
	fmsmoov::CompLimBandConfig* b1cfg = clc->mutable_b1cfg();
	fmsmoov::CompLimBandConfig* b2cfg = clc->mutable_b2cfg();
	fmsmoov::CompLimBandConfig* b3cfg = clc->mutable_b3cfg();
	fmsmoov::CompLimBandConfig* b4cfg = clc->mutable_b4cfg();
	fmsmoov::CompLimBandConfig* b5cfg = clc->mutable_b5cfg();

	/* CompLim defaults */
	//R = 1.6;
	//T = -40;
	//comp_b0 = new Compressor(R, T, -3.0, 0.0, 0.045, 0.070, m_bufsize, m_samprate, "b0");
	//comp_b1 = new Compressor(R, T, -3.0, 0.0, 0.005, 0.0015, m_bufsize, m_samprate, "b1");
	//comp_b2 = new Compressor(R, T, -2.0, 0.0, 0.001, 0.0003, m_bufsize, m_samprate, "b2");
	//comp_b3 = new Compressor(R, T-6, -2.0, 0.0, 0.0001, 0.0003, m_bufsize, m_samprate, "b3");
	//comp_b4 = new Compressor(R, T-9, -2.0, 0.0, 0.00001, 0.00003, m_bufsize, m_samprate, "b4");
	//comp_b5 = new Compressor(R, T-15, 1.0, 0.0, 0.000001, 0.000003, m_bufsize, m_samprate, "b5");

	float R = 1.8;
	float T = -32.0;

	b0cfg->set_ratio(R);
	b0cfg->set_threshold(T);
	b0cfg->set_fixed_gain(-3.0);
	b0cfg->set_knee_width(0.0);
	b0cfg->set_attack_time_ms(0.045);
	b0cfg->set_release_time_ms(0.070);

	b1cfg->set_ratio(R);
	b1cfg->set_threshold(T-2.0);
	b1cfg->set_fixed_gain(-2.0);
	b1cfg->set_knee_width(0.0);
	b1cfg->set_attack_time_ms(0.005);
	b1cfg->set_release_time_ms(0.015);

	b2cfg->set_ratio(R);
	b2cfg->set_threshold(T-4.0);
	b2cfg->set_fixed_gain(-2.0);
	b2cfg->set_knee_width(0.0);
	b2cfg->set_attack_time_ms(0.001);
	b2cfg->set_release_time_ms(0.0003);

	b3cfg->set_ratio(R);
	b3cfg->set_threshold(T-6.0);
	b3cfg->set_fixed_gain(-2.0);
	b3cfg->set_knee_width(0.0);
	b3cfg->set_attack_time_ms(0.0001);
	b3cfg->set_release_time_ms(0.0003);

	b4cfg->set_ratio(R);
	b4cfg->set_threshold(T-8.0);
	b4cfg->set_fixed_gain(-2.0);
	b4cfg->set_knee_width(0.0);
	b4cfg->set_attack_time_ms(0.00001);
	b4cfg->set_release_time_ms(0.00003);

	b5cfg->set_ratio(R);
	b5cfg->set_threshold(T-10.0);
	b5cfg->set_fixed_gain(1.0);
	b5cfg->set_knee_width(0.0);
	b5cfg->set_attack_time_ms(0.000001);
	b5cfg->set_release_time_ms(0.000003);

	m_complim->init(cfg);

	m_meter_post_complim_b0 = new PluginMeter("METER_POST_COMPLIM_B0", m_sample_rate, m_buffer_size);
	m_meter_post_complim_b0->init(cfgdummy);
	m_meter_post_complim_b1 = new PluginMeter("METER_POST_COMPLIM_B1", m_sample_rate, m_buffer_size);
	m_meter_post_complim_b1->init(cfgdummy);
	m_meter_post_complim_b2 = new PluginMeter("METER_POST_COMPLIM_B2", m_sample_rate, m_buffer_size);
	m_meter_post_complim_b2->init(cfgdummy);
	m_meter_post_complim_b3 = new PluginMeter("METER_POST_COMPLIM_B3", m_sample_rate, m_buffer_size);
	m_meter_post_complim_b3->init(cfgdummy);
	m_meter_post_complim_b4 = new PluginMeter("METER_POST_COMPLIM_B4", m_sample_rate, m_buffer_size);
	m_meter_post_complim_b4->init(cfgdummy);
	m_meter_post_complim_b5 = new PluginMeter("METER_POST_COMPLIM_B5", m_sample_rate, m_buffer_size);
	m_meter_post_complim_b5->init(cfgdummy);

	m_delay = new PluginDelay("DELAY", m_sample_rate, m_buffer_size);
	m_delay->init(cfgdummy);

	//TODO:  Do we need a manual output gain adjustment?  Skip for now.
	m_gain_output = new PluginGain("GAIN_OUTPUT", m_sample_rate, m_buffer_size);
	m_gain_output->init(cfgdummy);

	m_meter_main_out = new PluginMeter("METER_MAIN_OUT", m_sample_rate, m_buffer_size);
	m_meter_main_out->init(cfgdummy);

	m_gain_input->set_input(m_meter_main_in->get_output());
	m_meter_post_input_gain->set_input(m_gain_input->get_output());

	//m_30Hz_hpf->set_input(m_meter_post_input_gain->get_output());
	m_phase_rotator->set_input(m_meter_post_input_gain->get_output());

	m_agc_front->set_input(m_phase_rotator->get_output());
	m_agc_front->set_aux_input_bufs(m_meter_post_input_gain->get_aux_output_bufs());

	m_stereo_enhance->set_input(m_agc_front->get_output());

	m_bass_enhancer->set_input(m_bass_enhancer->get_output());

	m_crossover->set_input(m_stereo_enhance->get_output());

	m_meter_post_crossover_b0->set_input(m_crossover->get_aux_output_bufs(0, 2));
	m_meter_post_crossover_b1->set_input(m_crossover->get_aux_output_bufs(2, 2));
	m_meter_post_crossover_b2->set_input(m_crossover->get_aux_output_bufs(4, 2));
	m_meter_post_crossover_b3->set_input(m_crossover->get_aux_output_bufs(6, 2));
	m_meter_post_crossover_b4->set_input(m_crossover->get_aux_output_bufs(8, 2));
	m_meter_post_crossover_b5->set_input(m_crossover->get_aux_output_bufs(10, 2));

	m_complim->set_input(m_crossover->get_output());

	/* Set up the compressor limiter plugin aux bufs as follows:
	 * All of the crossover outputs (L/R pairs for bands 0-5)
	 * The log level outputs from each band's meter (aux output bufs 2-3 for the meters)
	 */
	m_complim->set_aux_input_bufs(m_crossover->get_aux_output_bufs());
	m_complim->append_aux_input_bufs(m_meter_post_crossover_b0->get_aux_output_bufs(2,2));
	m_complim->append_aux_input_bufs(m_meter_post_crossover_b1->get_aux_output_bufs(2,2));
	m_complim->append_aux_input_bufs(m_meter_post_crossover_b2->get_aux_output_bufs(2,2));
	m_complim->append_aux_input_bufs(m_meter_post_crossover_b3->get_aux_output_bufs(2,2));
	m_complim->append_aux_input_bufs(m_meter_post_crossover_b4->get_aux_output_bufs(2,2));
	m_complim->append_aux_input_bufs(m_meter_post_crossover_b5->get_aux_output_bufs(2,2));
	m_complim->append_aux_input_bufs(m_agc_front->get_aux_output_bufs(2, 2));
	m_complim->finalize_buffer_init();

	m_meter_post_complim_b0->set_input(m_complim->get_aux_output_bufs(0, 2));
	m_meter_post_complim_b1->set_input(m_complim->get_aux_output_bufs(2, 2));
	m_meter_post_complim_b2->set_input(m_complim->get_aux_output_bufs(4, 2));
	m_meter_post_complim_b3->set_input(m_complim->get_aux_output_bufs(6, 2));
	m_meter_post_complim_b4->set_input(m_complim->get_aux_output_bufs(8, 2));
	m_meter_post_complim_b5->set_input(m_complim->get_aux_output_bufs(10, 2));

	m_delay->set_input(m_complim->get_output());

	m_meter_main_out->set_input(m_delay->get_output());
}

ProcessorCore::~ProcessorCore() {
	// TODO Auto-generated destructor stub
}

void ProcessorCore::set_jack_inbufs(vector<AudioBuf*>* bufs) {
	(*m_jack_inbufs)[0]->setptr((*bufs)[0]->get(), (*bufs)[0]->size());
	(*m_jack_inbufs)[1]->setptr((*bufs)[1]->get(), (*bufs)[1]->size());

}

void ProcessorCore::set_jack_outbufs(vector<AudioBuf*>* bufs) {
	(*m_jack_outbufs)[0]->setptr((*bufs)[0]->get(), (*bufs)[0]->size());
	(*m_jack_outbufs)[1]->setptr((*bufs)[1]->get(), (*bufs)[1]->size());
}

void ProcessorCore::process() {
	m_meter_main_in->refresh_inputs(m_jack_inbufs);
	m_meter_main_in->process();
	m_meter_main_in->get_total_levels(m_main_inlinL, m_main_inlinR, m_main_inlogL, m_main_inlogR);

	if(true == m_master_bypass) {
		m_meter_main_out->refresh_inputs(m_jack_inbufs);
	}
	else {
		m_gain_input->process();
		m_meter_post_input_gain->process();
		//m_30Hz_hpf->process();
		m_phase_rotator->process();
		m_agc_front->process();
		m_stereo_enhance->process();
		m_bass_enhancer->process();
		m_crossover->process();
		m_meter_post_crossover_b0->process();
		m_meter_post_crossover_b1->process();
		m_meter_post_crossover_b2->process();
		m_meter_post_crossover_b3->process();
		m_meter_post_crossover_b4->process();
		m_meter_post_crossover_b5->process();
		m_meter_post_crossover_b0->get_total_levels_log(m_lev_precomp_b0L, m_lev_precomp_b0R);
		m_meter_post_crossover_b1->get_total_levels_log(m_lev_precomp_b1L, m_lev_precomp_b1R);
		m_meter_post_crossover_b2->get_total_levels_log(m_lev_precomp_b2L, m_lev_precomp_b2R);
		m_meter_post_crossover_b3->get_total_levels_log(m_lev_precomp_b3L, m_lev_precomp_b3R);
		m_meter_post_crossover_b4->get_total_levels_log(m_lev_precomp_b4L, m_lev_precomp_b4R);
		m_meter_post_crossover_b5->get_total_levels_log(m_lev_precomp_b5L, m_lev_precomp_b5R);
		m_complim->process();
		m_complim->get_comp_levels(m_complev_b0, m_complev_b1, m_complev_b2, m_complev_b3, m_complev_b4, m_complev_b5);
		m_meter_post_complim_b0->process();
		m_meter_post_complim_b1->process();
		m_meter_post_complim_b2->process();
		m_meter_post_complim_b3->process();
		m_meter_post_complim_b4->process();
		m_meter_post_complim_b5->process();
		m_meter_post_complim_b0->get_total_levels_log(m_lev_postcomp_b0L, m_lev_postcomp_b0R);
		m_meter_post_complim_b1->get_total_levels_log(m_lev_postcomp_b1L, m_lev_postcomp_b1R);
		m_meter_post_complim_b2->get_total_levels_log(m_lev_postcomp_b2L, m_lev_postcomp_b2R);
		m_meter_post_complim_b3->get_total_levels_log(m_lev_postcomp_b3L, m_lev_postcomp_b3R);
		m_meter_post_complim_b4->get_total_levels_log(m_lev_postcomp_b4L, m_lev_postcomp_b4R);
		m_meter_post_complim_b5->get_total_levels_log(m_lev_postcomp_b5L, m_lev_postcomp_b5R);
		m_delay->process();
		m_meter_main_out->refresh_inputs(m_delay->get_output());
	}

	m_meter_main_out->process();
	m_meter_main_out->get_total_levels(m_main_outlinL, m_main_outlinR, m_main_outlogL, m_main_outlogR);
	m_meter_main_out->copy_output_bufs(m_jack_outbufs);

	/* Now populate the processor live data */
	m_pld.set_inl(m_main_inlogL);
	m_pld.set_inr(m_main_inlogR);
	m_pld.set_outl(m_main_outlogL);
	m_pld.set_outr(m_main_outlogR);
	m_pld.set_agc_adj(m_agc_front->get_gs_avg());
	m_pld.set_agc_gate_active(m_agc_front->get_gate_active());

	m_pld.set_lev_precomp_b0l(m_lev_precomp_b0L);
	m_pld.set_lev_precomp_b0r(m_lev_precomp_b0R);
	m_pld.set_lev_precomp_b1l(m_lev_precomp_b1L);
	m_pld.set_lev_precomp_b1r(m_lev_precomp_b1R);
	m_pld.set_lev_precomp_b2l(m_lev_precomp_b2L);
	m_pld.set_lev_precomp_b2r(m_lev_precomp_b2R);
	m_pld.set_lev_precomp_b3l(m_lev_precomp_b3L);
	m_pld.set_lev_precomp_b3r(m_lev_precomp_b3R);
	m_pld.set_lev_precomp_b4l(m_lev_precomp_b4L);
	m_pld.set_lev_precomp_b4r(m_lev_precomp_b4R);
	m_pld.set_lev_precomp_b5l(m_lev_precomp_b5L);
	m_pld.set_lev_precomp_b5r(m_lev_precomp_b5R);

	m_pld.set_lev_comp_b0(m_complev_b0);
	m_pld.set_lev_comp_b1(m_complev_b1);
	m_pld.set_lev_comp_b2(m_complev_b2);
	m_pld.set_lev_comp_b3(m_complev_b3);
	m_pld.set_lev_comp_b4(m_complev_b4);
	m_pld.set_lev_comp_b5(m_complev_b5);

	m_pld.set_lev_postcomp_b0l(m_lev_postcomp_b0L);
	m_pld.set_lev_postcomp_b0r(m_lev_postcomp_b0R);
	m_pld.set_lev_postcomp_b1l(m_lev_postcomp_b1L);
	m_pld.set_lev_postcomp_b1r(m_lev_postcomp_b1R);
	m_pld.set_lev_postcomp_b2l(m_lev_postcomp_b2L);
	m_pld.set_lev_postcomp_b2r(m_lev_postcomp_b2R);
	m_pld.set_lev_postcomp_b3l(m_lev_postcomp_b3L);
	m_pld.set_lev_postcomp_b3r(m_lev_postcomp_b3R);
	m_pld.set_lev_postcomp_b4l(m_lev_postcomp_b4L);
	m_pld.set_lev_postcomp_b4r(m_lev_postcomp_b4R);
	m_pld.set_lev_postcomp_b5l(m_lev_postcomp_b5L);
	m_pld.set_lev_postcomp_b5r(m_lev_postcomp_b5R);
}

void ProcessorCore::get_live_data(fmsmoov::ProcessorLiveData& pld) {
	pld.CopyFrom(m_pld);
}

fmsmoov::MasterBypassSetResponse ProcessorCore::set_master_bypass(const fmsmoov::MasterBypassSetCommand& cmd) {
	fmsmoov::MasterBypassSetResponse rsp;
	m_master_bypass = cmd.bypass();
	return rsp;
}

fmsmoov::MasterBypassGetResponse ProcessorCore::get_master_bypass(const fmsmoov::MasterBypassGetCommand& cmd) {
	fmsmoov::MasterBypassGetResponse rsp;
	rsp.set_bypass(m_master_bypass);
	return rsp;
}

fmsmoov::GainSetResponse ProcessorCore::set_gain(const fmsmoov::GainSetCommand& cmd) {
	fmsmoov::GainSetResponse rsp;
	fmsmoov::PluginConfigResponse pcr;
	fmsmoov::PluginConfig cfg;

	switch(cmd.which()) {
	case fmsmoov::GainControlType::MAIN_IN:

		if(cmd.has_gain()){
			cfg.mutable_gain()->set_l(cmd.gain());
			cfg.mutable_gain()->set_r(cmd.gain());
		}
		else if(cmd.has_gain_pair()) {
			cfg.mutable_gain()->set_l(cmd.gain_pair().l());
			cfg.mutable_gain()->set_r(cmd.gain_pair().r());
		}
		else {
			LOGE("Unknown gain type");
		}

		pcr = m_gain_input->do_change_cfg(cfg);

		break;
	case fmsmoov::GainControlType::MAIN_OUT:
		break;
	default:
		LOGE("Unknown GainControlType {}", fmsmoov::GainControlType_Name(cmd.which()));
	}

	return rsp;
}

fmsmoov::GainGetResponse ProcessorCore::get_gain(const fmsmoov::GainGetCommand& cmd) {
	fmsmoov::GainGetResponse rsp;

	switch(cmd.which()) {
	case fmsmoov::GainControlType::MAIN_IN:
		rsp.set_which(cmd.which());
		rsp.set_gain(m_gain_input->get_gain());
		rsp.set_type(fmsmoov::GainType::LOG);
		break;
	case fmsmoov::GainControlType::MAIN_OUT:
		break;
	default:
		LOGE("Unknown GainControlType {}", fmsmoov::GainControlType_Name(cmd.which()));
	}

	return rsp;
}

fmsmoov::AGCSetResponse ProcessorCore::set_agc(const fmsmoov::AGCSetCommand& cmd) {
	fmsmoov::AGCSetResponse rsp;
	return rsp;
}

fmsmoov::AGCGetResponse ProcessorCore::get_agc(const fmsmoov::AGCGetCommand& cmd) {
	fmsmoov::AGCGetResponse rsp;
	return rsp;
}

fmsmoov::PhaseRotatorSetResponse ProcessorCore::set_phase_rotator(const fmsmoov::PhaseRotatorSetCommand& cmd) {
	fmsmoov::PhaseRotatorSetResponse rsp;

	m_phase_rotator->set_enable(cmd.enable());

	return rsp;
}

fmsmoov::PhaseRotatorGetResponse ProcessorCore::get_phase_rotator(const fmsmoov::PhaseRotatorGetCommand& cmd) {
	fmsmoov::PhaseRotatorGetResponse rsp;

	rsp.set_enabled(m_phase_rotator->get_enabled());

	return rsp;
}

fmsmoov::StereoEnhanceSetResponse ProcessorCore::set_stereo_enhance(const fmsmoov::StereoEnhanceSetCommand& cmd) {
	fmsmoov::StereoEnhanceSetResponse rsp;

	m_stereo_enhance->set_enable(cmd.enable());

	return rsp;
}

fmsmoov::StereoEnhanceGetResponse ProcessorCore::get_stereo_enhance(const fmsmoov::StereoEnhanceGetCommand& cmd) {
	fmsmoov::StereoEnhanceGetResponse rsp;

	rsp.set_enabled(m_stereo_enhance->get_enabled());

	return rsp;
}

fmsmoov::BassEnhancerSetResponse ProcessorCore::set_bass_enhancer(const fmsmoov::BassEnhancerSetCommand& cmd) {
	fmsmoov::BassEnhancerSetResponse rsp;

	m_bass_enhancer->set_enable(cmd.enable());

	return rsp;
}

fmsmoov::BassEnhancerGetResponse ProcessorCore::get_bass_enhancer(const fmsmoov::BassEnhancerGetCommand& cmd) {
	fmsmoov::BassEnhancerGetResponse rsp;

	rsp.set_enabled(m_bass_enhancer->get_enabled());

	return rsp;
}

fmsmoov::CompLimSetResponse ProcessorCore::set_compressor_limiter(const fmsmoov::CompLimSetCommand& cmd) {
	fmsmoov::CompLimSetResponse rsp;

	m_complim->set_enable(cmd.enable());

	return rsp;
}

fmsmoov::CompLimGetResponse ProcessorCore::get_compressor_limiter(const fmsmoov::CompLimGetCommand& cmd) {
	fmsmoov::CompLimGetResponse rsp;

	rsp.set_enabled(m_complim->get_enabled());

	return rsp;
}

fmsmoov::DelaySetResponse ProcessorCore::set_delay(const fmsmoov::DelaySetCommand& cmd) {
	fmsmoov::DelaySetResponse rsp;

	m_delay->set_delay(cmd.delay_ms());

	return rsp;
}
