/*
 * plugin_agc.cpp
 *
 *  Created on: Sep 30, 2022
 *      Author: zaremba
 */

#include <plugin_agc.h>

PluginAGC::PluginAGC(string name, uint32_t samprate, uint32_t bufsize) : ProcessorPlugin(name, samprate, bufsize) {
	LOGT("PluginAGC CTOR");
	//TODO: change defaults
	m_GT = 0.0;
	m_TL = -24.0;
	//Tms = 8685 / (dB/S)
	//m_Tatt = 17.37;  // for 0.5 dB/sec attack time
	//m_Trel = 17.37;  // for 0.5 dB/sec release time
	m_Tatt = 17.37;
	m_Trel = 17.37;
	m_Tatt_gate = 23.0;
	m_alphaA = expf(-logf(9.0)/(m_samprate * m_Tatt));
	m_alphaR = expf(-logf(9.0)/(m_samprate * m_Trel));
	m_alpha_gate = expf(-logf(9.0)/(m_samprate * m_Tatt_gate));
	m_gate_level = -20.0;  //relative to target rate
	//m_alphaA = 1 - expf(-1/(m_samprate * m_Tatt));
	//m_alphaR = 1 - expf(-1/(m_samprate * m_Trel));
	m_gs_avg = 0.0;
	m_gate_close_reset_thresh = 2.0f / ((float)m_bufsize / (float)m_samprate); //how many process cycles in 1 second?
	LOGI("Gate close reset threshold is {} process cycles.", m_gate_close_reset_thresh);
	m_gate_closed_cycles = 0;
}

PluginAGC::~PluginAGC() {
	LOGT("PluginAGC DTOR");
}

bool PluginAGC::do_init(const fmsmoov::PluginConfig& cfg) {
	AudioBuf* btmp;

	btmp = new AudioBuf("AGC_OUT_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
												m_bufsize, NULL);

	m_bufs_out->push_back(btmp);

	btmp = new AudioBuf("AGC_OUT_R", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
											m_bufsize, NULL);

	m_bufs_out->push_back(btmp);

	btmp = new AudioBuf("AGC_ADJ_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
													m_bufsize, NULL);

	m_bufs_out->push_back(btmp);

	btmp = new AudioBuf("AGC_ADJ_R", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
											m_bufsize, NULL);

	m_bufs_out->push_back(btmp);


	this->do_change_cfg(cfg);

	return true;
}

fmsmoov::PluginConfigResponse PluginAGC::do_change_cfg(const fmsmoov::PluginConfig& cfg) {
	fmsmoov::PluginConfigResponse pcr;

	if(cfg.has_agc()) {
		m_GT = cfg.agc().gt();
		m_TL = cfg.agc().tl();
		m_Tatt = cfg.agc().tatt();
		m_Trel = cfg.agc().trel();
		m_alphaA = expf(-logf(9.0)/(m_samprate * m_Tatt));
		m_alphaR = expf(-logf(9.0)/(m_samprate * m_Trel));
	}
	else {
		LOGE("PluginAGC init missing AGC config");
	}

	return pcr;
}

void PluginAGC::do_set_aux_input_bufs(vector<AudioBuf*>* bufs) {
	m_login_L = m_bufs_in->at(4)->get();
	m_login_R = m_bufs_in->at(5)->get();
}

void PluginAGC::finalize_buffer_init() {

}


float PluginAGC::get_gs_avg() {
	return m_gs_avg;
}

bool PluginAGC::get_gate_active() {
	return m_gate_active;
}

/* The main AGC processing loop.  Lots of work needed to figure this out.
 * Make it sound like a pro unit.  Hard-coded 1024 sample per call
 * assumption TODO:factor for variable buffer sizes.
 */
int PluginAGC::do_process() {

	if(m_gate_active) {
		m_gate_closed_cycles++;
	}
	else {
		m_gate_closed_cycles = 0;
	}

	for(uint32_t i = 0; i < m_bufsize; i++)
	{
		if(m_login_L[i] > m_login_R[i]) {
			idb = m_login_L[i];
		}
		else {
			idb = m_login_R[i];
		}

		if(idb > m_TL + m_gate_level) {
			gc = m_TL - idb;

			if(gc < m_gsprev)
			{
			  gs = m_alphaA*m_gsprev + (1-m_alphaA)*gc;
			}
			else if(gc > m_gsprev)
			{
			  gs = m_alphaR*m_gsprev + (1-m_alphaR)*gc;
			}
			m_gate_active = false;
		}
		else {
			m_gate_active = true;

			if(m_gate_closed_cycles > m_gate_close_reset_thresh) {
				//return to 0 dB
				gc = 0.0;

				if(gc < m_gsprev)
				{
				  gs = m_alpha_gate*m_gsprev + (1-m_alpha_gate)*gc;
				}
				else if(gc > m_gsprev)
				{
				  gs = m_alpha_gate*m_gsprev + (1-m_alpha_gate)*gc;
				}
			}
			else {
				gs = m_gsprev;
			}
		}

		m_gsprev = gs;
		m_gs_avg += gs;

		out_L[i] = in_L[i] * powf(10, gs/20.0);
		out_R[i] = in_R[i] * powf(10, gs/20.0);
	}

	m_gs_avg = m_gs_avg / m_bufsize;

	return 0;
}
