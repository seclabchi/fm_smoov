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

	float R = 1.6;
	float T = -40.0;

	/*
	comp_b0 = new Compressor(R, T, -16.0, 0.0, 0.080, 0.1200, m_bufsize, m_samprate, "b0");
	comp_b1 = new Compressor(R, T, -10.0, 0.0, 0.030, 0.0500, m_bufsize, m_samprate, "b1");
	comp_b2 = new Compressor(R, T, -10.0, 0.0, 0.001, 0.0025, m_bufsize, m_samprate, "b2");
	comp_b3 = new Compressor(R, T, -9.0, 0.0, 0.00075, 0.00105, m_bufsize, m_samprate, "b3");
	comp_b4 = new Compressor(R, T, -8.0, 0.0, 0.00030, 0.00050, m_bufsize, m_samprate, "b4");
	comp_b5 = new Compressor(R, T, -7.0, 0.0, 0.00004, 0.00009, m_bufsize, m_samprate, "b5");
	*/

	comp_b0 = new Compressor(R, T, -3.0, 0.0, 0.045, 0.070, m_bufsize, m_samprate, "b0");
	comp_b1 = new Compressor(R, T, -3.0, 0.0, 0.005, 0.0015, m_bufsize, m_samprate, "b1");
	comp_b2 = new Compressor(R, T, -2.0, 0.0, 0.001, 0.0003, m_bufsize, m_samprate, "b2");
	comp_b3 = new Compressor(R, T-6, -2.0, 0.0, 0.0001, 0.0003, m_bufsize, m_samprate, "b3");
	comp_b4 = new Compressor(R, T-9, -2.0, 0.0, 0.00001, 0.00003, m_bufsize, m_samprate, "b4");
	comp_b5 = new Compressor(R, T-15, 1.0, 0.0, 0.000001, 0.000003, m_bufsize, m_samprate, "b5");
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

bool PluginCompressorLimiter::do_change_cfg(const fmsmoov::PluginConfig& cfg) {
	return true;
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
