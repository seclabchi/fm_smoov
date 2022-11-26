/*
 * plugin_compressor_limiter.cpp
 *
 *  Created on: Oct 23, 2022
 *      Author: zaremba
 */

#include <plugin_compressor_limiter.h>

/* Intial default values for the compressors
 *
 * b0 - 150 Hz:  R=3, T=-30, G=0, Tatt=0.080000, Trel=0.120000
 * b1 - 420 Hz:  R=3, T=-30, G=0, Tatt=0.030000, Trel=0.050000
 * b2 - 700 Hz:  R=3, T=-30, G=0, Tatt=0.001000, Trel=0.002500
 * b3 - 1.6 kHz: R=3, T=-30, G=0, Tatt=0.000750, Trel=0.001050
 * b4 - 3.7 kHz: R=3, T=-30, G=0, Tatt=0.000300, Trel=0.000500
 * b5 - 6.2 kHz: R=3, T=-30, G=0, Tatt=0.000040, Trel=0.000090
 */


PluginCompressorLimiter::PluginCompressorLimiter(const std::string& _name, uint32_t samprate, uint32_t bufsize) : ProcessorPlugin(_name.c_str(), samprate, bufsize) {
	/* Compressor CTOR:  R T G W att rel */


}

PluginCompressorLimiter::~PluginCompressorLimiter() {
	// TODO Auto-generated destructor stub
}

int PluginCompressorLimiter::do_process() {

	if(m_enabled) {
		comp_b0->process(b0_inL, b0_inR, b0_levL, b0_levR, front_agcL, b0_outL, b0_outR, b0_comp);
		comp_b1->process(b1_inL, b1_inR, b1_levL, b1_levR, front_agcL, b1_outL, b1_outR, b1_comp);
		comp_b2->process(b2_inL, b2_inR, b2_levL, b2_levR, front_agcL, b2_outL, b2_outR, b2_comp);
		comp_b3->process(b3_inL, b3_inR, b3_levL, b3_levR, front_agcL, b3_outL, b3_outR, b3_comp);
		comp_b4->process(b4_inL, b4_inR, b4_levL, b4_levR, front_agcL, b4_outL, b4_outR, b4_comp);
		comp_b5->process(b5_inL, b5_inR, b5_levL, b5_levR, front_agcL, b5_outL, b5_outR, b5_comp);

		for(uint32_t i = 0; i < m_bufsize; i++) {
			out_L[i] = b0_outL[i] + b1_outL[i] + b2_outL[i] + b3_outL[i] + b4_outL[i] + b5_outL[i];
			out_R[i] = b0_outR[i] + b1_outR[i] + b2_outR[i] + b3_outR[i] + b4_outR[i] + b5_outR[i];
		}
	}
	else {
		memcpy(out_L, in_L, m_bufsize * sizeof(float));
		memcpy(out_R, in_R, m_bufsize * sizeof(float));
	}

	return 0;
}

void PluginCompressorLimiter::get_comp_levels(float& complev_b0, float& complev_b1, float& complev_b2, float& complev_b3, float& complev_b4, float& complev_b5) {
	comp_b0->get_total_comp(complev_b0);
	comp_b1->get_total_comp(complev_b1);
	comp_b2->get_total_comp(complev_b2);
	comp_b3->get_total_comp(complev_b3);
	comp_b4->get_total_comp(complev_b4);
	comp_b5->get_total_comp(complev_b5);
}

fmsmoov::PluginConfigResponse PluginCompressorLimiter::do_change_cfg(const fmsmoov::PluginConfig& cfg) {
	fmsmoov::PluginConfigResponse pcr;

	if(cfg.has_complim()) {
		fmsmoov::CompLimConfig clc = cfg.complim();
		fmsmoov::CompLimBandConfig b0cfg = clc.b0cfg();
		fmsmoov::CompLimBandConfig b1cfg = clc.b1cfg();
		fmsmoov::CompLimBandConfig b2cfg = clc.b2cfg();
		fmsmoov::CompLimBandConfig b3cfg = clc.b3cfg();
		fmsmoov::CompLimBandConfig b4cfg = clc.b4cfg();
		fmsmoov::CompLimBandConfig b5cfg = clc.b5cfg();

		comp_b0->set(b0cfg.ratio(), b0cfg.threshold(), b0cfg.fixed_gain(), b0cfg.knee_width(), b0cfg.attack_time_ms(), b0cfg.release_time_ms());
		comp_b1->set(b1cfg.ratio(), b1cfg.threshold(), b1cfg.fixed_gain(), b1cfg.knee_width(), b1cfg.attack_time_ms(), b1cfg.release_time_ms());
		comp_b2->set(b2cfg.ratio(), b2cfg.threshold(), b2cfg.fixed_gain(), b2cfg.knee_width(), b2cfg.attack_time_ms(), b2cfg.release_time_ms());
		comp_b3->set(b3cfg.ratio(), b3cfg.threshold(), b3cfg.fixed_gain(), b3cfg.knee_width(), b3cfg.attack_time_ms(), b3cfg.release_time_ms());
		comp_b4->set(b4cfg.ratio(), b4cfg.threshold(), b4cfg.fixed_gain(), b4cfg.knee_width(), b4cfg.attack_time_ms(), b4cfg.release_time_ms());
		comp_b5->set(b5cfg.ratio(), b5cfg.threshold(), b5cfg.fixed_gain(), b5cfg.knee_width(), b5cfg.attack_time_ms(), b5cfg.release_time_ms());
	}
	else {
		LOGE("Config sent to CompressorLimiter with no complim field.");
	}

	return pcr;
}

bool PluginCompressorLimiter::do_init(const fmsmoov::PluginConfig& cfg) {
	AudioBuf* btmp;

	btmp = new AudioBuf("COMPLIM_B0_OUT_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
											m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b0_outL = btmp->get();

	btmp = new AudioBuf("COMPLIM_B0_OUT_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b0_outR = btmp->get();

	btmp = new AudioBuf("COMPLIM_B1_OUT_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b1_outL = btmp->get();

	btmp = new AudioBuf("COMPLIM_B1_OUT_R", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b1_outR = btmp->get();

	btmp = new AudioBuf("COMPLIM_B2_OUT_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b2_outL = btmp->get();

	btmp = new AudioBuf("COMPLIM_B2_OUT_R", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b2_outR = btmp->get();

	btmp = new AudioBuf("COMPLIM_B3_OUT_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b3_outL = btmp->get();

	btmp = new AudioBuf("COMPLIM_B3_OUT_R", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b3_outR = btmp->get();

	btmp = new AudioBuf("COMPLIM_B4_OUT_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b4_outL = btmp->get();

	btmp = new AudioBuf("COMPLIM_B4_OUT_R", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b4_outR = btmp->get();

	btmp = new AudioBuf("COMPLIM_B5_OUT_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b5_outL = btmp->get();

	btmp = new AudioBuf("COMPLIM_B5_OUT_R", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b5_outR = btmp->get();


	btmp = new AudioBuf("COMPLIM_B0_COMP", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
													m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b0_comp = btmp->get();

	btmp = new AudioBuf("COMPLIM_B1_COMP", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
														m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b1_comp = btmp->get();


	btmp = new AudioBuf("COMPLIM_B2_COMP", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
														m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b2_comp = btmp->get();


	btmp = new AudioBuf("COMPLIM_B3_COMP", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
														m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b3_comp = btmp->get();

	btmp = new AudioBuf("COMPLIM_B4_COMP", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
														m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b4_comp = btmp->get();


	btmp = new AudioBuf("COMPLIM_B5_COMP", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
														m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	b5_comp = btmp->get();

	if(cfg.has_complim()) {
		fmsmoov::CompLimConfig clc = cfg.complim();
		fmsmoov::CompLimBandConfig b0cfg = clc.b0cfg();
		fmsmoov::CompLimBandConfig b1cfg = clc.b1cfg();
		fmsmoov::CompLimBandConfig b2cfg = clc.b2cfg();
		fmsmoov::CompLimBandConfig b3cfg = clc.b3cfg();
		fmsmoov::CompLimBandConfig b4cfg = clc.b4cfg();
		fmsmoov::CompLimBandConfig b5cfg = clc.b5cfg();
		comp_b0 = new Compressor(b0cfg.ratio(), b0cfg.threshold(), b0cfg.fixed_gain(), b0cfg.knee_width(), b0cfg.attack_time_ms(), b0cfg.release_time_ms(), m_bufsize, m_samprate, "b0");
		comp_b1 = new Compressor(b1cfg.ratio(), b1cfg.threshold(), b1cfg.fixed_gain(), b1cfg.knee_width(), b1cfg.attack_time_ms(), b1cfg.release_time_ms(), m_bufsize, m_samprate, "b1");
		comp_b2 = new Compressor(b2cfg.ratio(), b2cfg.threshold(), b2cfg.fixed_gain(), b2cfg.knee_width(), b2cfg.attack_time_ms(), b2cfg.release_time_ms(), m_bufsize, m_samprate, "b2");
		comp_b3 = new Compressor(b3cfg.ratio(), b3cfg.threshold(), b3cfg.fixed_gain(), b3cfg.knee_width(), b3cfg.attack_time_ms(), b3cfg.release_time_ms(), m_bufsize, m_samprate, "b3");
		comp_b4 = new Compressor(b4cfg.ratio(), b4cfg.threshold(), b4cfg.fixed_gain(), b4cfg.knee_width(), b4cfg.attack_time_ms(), b4cfg.release_time_ms(), m_bufsize, m_samprate, "b4");
		comp_b5 = new Compressor(b5cfg.ratio(), b5cfg.threshold(), b5cfg.fixed_gain(), b5cfg.knee_width(), b5cfg.attack_time_ms(), b5cfg.release_time_ms(), m_bufsize, m_samprate, "b5");
	}
	else {
		LOGE("CompressorLimiter init doesn't have a complim member.");
		return false;
	}

	return true;
}

void PluginCompressorLimiter::do_set_aux_input_bufs(vector<AudioBuf*>* bufs) {

}

void PluginCompressorLimiter::finalize_buffer_init() {
	/* Start at buffer index 2, which is the beginning of the aux bufs.
	 * TODO: This is all pretty brittle and prone to failure.  Segregate
	 * the main in/out bufs with the aux bufs???  Dunno.  This one screwed
	 * me up for a while, thinking something was wrong with the crossover
	 * filters when it was just that I was starting this list at zero.
	 */
	b0_inL = m_bufs_in->at(2)->get();
	b0_inR = m_bufs_in->at(3)->get();
	b1_inL = m_bufs_in->at(4)->get();
	b1_inR = m_bufs_in->at(5)->get();
	b2_inL = m_bufs_in->at(6)->get();
	b2_inR = m_bufs_in->at(7)->get();
	b3_inL = m_bufs_in->at(8)->get();
	b3_inR = m_bufs_in->at(9)->get();
	b4_inL = m_bufs_in->at(10)->get();
	b4_inR = m_bufs_in->at(11)->get();
	b5_inL = m_bufs_in->at(12)->get();
	b5_inR = m_bufs_in->at(13)->get();
	b0_levL = m_bufs_in->at(14)->get();
	b0_levR = m_bufs_in->at(15)->get();
	b1_levL = m_bufs_in->at(16)->get();
	b1_levR = m_bufs_in->at(17)->get();
	b2_levL = m_bufs_in->at(18)->get();
	b2_levR = m_bufs_in->at(19)->get();
	b3_levL = m_bufs_in->at(20)->get();
	b3_levR = m_bufs_in->at(21)->get();
	b4_levL = m_bufs_in->at(22)->get();
	b4_levR = m_bufs_in->at(23)->get();
	b5_levL = m_bufs_in->at(24)->get();
	b5_levR = m_bufs_in->at(25)->get();
	front_agcL = m_bufs_in->at(26)->get();
	front_agcR = m_bufs_in->at(27)->get();
}
