/*
 * plugin_30Hz_hpf.cpp
 *
 *  Created on: Jul 4, 2022
 *      Author: zaremba
 */

#include <plugin_30Hz_hpf.h>

Plugin30HzHpf::Plugin30HzHpf(const std::string& _name, uint32_t samprate, uint32_t bufsize) : ProcessorPlugin(_name, samprate, bufsize) {
	LOGT("Plugin30HzHpf CTOR");
	setup_filters();
}

Plugin30HzHpf::~Plugin30HzHpf() {
	LOGT("Plugin30HzHpf DTOR");
	delete filtL;
	delete filtR;
}

void Plugin30HzHpf::setup_filters() {
	filtL = new ParametricFilter(m_samprate, m_bufsize, 30.0, 1.0, 0.0, "highpass", "30HzHPF_L");
	filtR = new ParametricFilter(m_samprate, m_bufsize, 30.0, 1.0, 0.0, "highpass", "30HzHPF_R");
}

bool Plugin30HzHpf::do_init(const fmsmoov::PluginConfig& cfg) {

	return true;
}

fmsmoov::PluginConfigResponse Plugin30HzHpf::do_change_cfg(const fmsmoov::PluginConfig& cfg) {
	fmsmoov::PluginConfigResponse pcr;
	return pcr;
}

void Plugin30HzHpf::do_set_aux_input_bufs(vector<AudioBuf*>* bufs) {

}

void Plugin30HzHpf::finalize_buffer_init() {

}

int Plugin30HzHpf::do_process() {

	filtL->process(in_L, out_L);
	filtR->process(in_R, out_R);

	return 0;
}

