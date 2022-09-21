/*
 * plugin_30Hz_hpf.cpp
 *
 *  Created on: Jul 4, 2022
 *      Author: zaremba
 */

#include <plugin_30Hz_hpf.h>

Plugin30HzHpf::Plugin30HzHpf(uint32_t samprate, uint32_t bufsize) : ProcessorPlugin("30HZ_HPF", samprate, bufsize) {
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

bool Plugin30HzHpf::do_init(const std::map<std::string, PluginConfigVal>& config_vals) {

	//this plugin has two input buffers for now:  the left and right inputs.

	AudioBuf* btmp = new AudioBuf("30HZ_HPF_IN_L", AudioBuf::AUDIO_BUF_TYPE::REFERENCE,
									m_bufsize, NULL);

	m_bufs_in->push_back(btmp);


	btmp = new AudioBuf("30HZ_HPF_IN_R", AudioBuf::AUDIO_BUF_TYPE::REFERENCE,
										m_bufsize, NULL);

	m_bufs_in->push_back(btmp);
	in_R = m_bufs_in->at(1)->get();

	//this plugin has two output buffers for now:  the L and R filtered channels

	btmp = new AudioBuf("30HZ_HPF_OUT_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
											m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	out_L = m_bufs_out->at(0)->get();

	btmp = new AudioBuf("30HZ_HPF_OUT_R", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
											m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	out_R = m_bufs_out->at(1)->get();

	return true;
}

bool Plugin30HzHpf::do_change_cfg(const std::map<std::string, PluginConfigVal>& config_vals) {

	return true;
}

int Plugin30HzHpf::do_process() {

	filtL->process(in_L, out_L);
	filtR->process(in_R, out_R);

	//memcpy(m_bufs_out->at(0)->get(), m_bufs_in->at(0)->get(), m_bufs_out->at(0)->size() * sizeof(float));
	//memcpy(m_bufs_out->at(1)->get(), m_bufs_in->at(1)->get(), m_bufs_out->at(1)->size() * sizeof(float));

	return 0;
}

