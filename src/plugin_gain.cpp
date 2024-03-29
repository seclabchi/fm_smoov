/*
 * plugin_gain.cpp
 *
 *  Created on: Jul 2, 2022
 *      Author: zaremba
 */

#include <plugin_gain.h>
#include "Prefs.h"

static Prefs* prefs;

PluginGain::PluginGain(string name, uint32_t samprate, uint32_t bufsize) : ProcessorPlugin(name, samprate, bufsize) {
	LOGT("PluginGain CTOR");
	m_gain_L = 1.0;
	m_gain_R = 1.0;
	m_gainlog_L = 0.0;
	m_gainlog_R = 0.0;
	prefs = Prefs::get_instance();
}

PluginGain::~PluginGain() {
	LOGT("PluginGain DTOR");
}

bool PluginGain::do_init(const fmsmoov::PluginConfig& cfg) {
	m_gain_L = powf(10, cfg.gain().l()/20.0);
	m_gain_R = powf(10, cfg.gain().r()/20.0);
	return true;
}

fmsmoov::PluginConfigResponse PluginGain::do_change_cfg(const fmsmoov::PluginConfig& cfg) {
	fmsmoov::PluginConfigResponse pcr;

	m_gain_L = powf(10, cfg.gain().l()/20.0);
	m_gain_R = powf(10, cfg.gain().r()/20.0);

	prefs->set_gain_input(m_gain_L);

	return pcr;
}

void PluginGain::do_set_aux_input_bufs(vector<AudioBuf*>* bufs) {

}

void PluginGain::finalize_buffer_init() {

}


int PluginGain::do_process() {

	for(uint32_t i = 0; i < m_bufsize; i++) {
		out_L[i] = m_gain_L*in_L[i];
		out_R[i] = m_gain_R*in_R[i];
	}

	return 0;
}

float PluginGain::get_gain() {
	return 20*log10f(m_gain_L);
}
