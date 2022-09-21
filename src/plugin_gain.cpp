/*
 * plugin_gain.cpp
 *
 *  Created on: Jul 2, 2022
 *      Author: zaremba
 */

#include <plugin_gain.h>

PluginGain::PluginGain(uint32_t samprate, uint32_t bufsize) : ProcessorPlugin("GAIN", samprate, bufsize) {
	LOGT("PluginGain CTOR");
}

PluginGain::~PluginGain() {
	LOGT("PluginGain DTOR");
}

bool PluginGain::do_init(const std::map<std::string, PluginConfigVal>& config_vals) {

	//this plugin has two input buffers for now:  the left and right inputs.

	AudioBuf* btmp = new AudioBuf("GAIN_IN_L", AudioBuf::AUDIO_BUF_TYPE::REFERENCE,
									m_bufsize, NULL);

	m_bufs_in->push_back(btmp);


	btmp = new AudioBuf("GAIN_IN_R", AudioBuf::AUDIO_BUF_TYPE::REFERENCE,
										m_bufsize, NULL);

	m_bufs_in->push_back(btmp);
	in_R = m_bufs_in->at(1)->get();

	//this plugin has two output buffers for now:  the L and R gain-adjusted channels

	btmp = new AudioBuf("GAIN_OUT_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
											m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	out_L = m_bufs_out->at(0)->get();

	btmp = new AudioBuf("GAIN_OUT_R", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
											m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	out_R = m_bufs_out->at(1)->get();

	return true;
}

bool PluginGain::do_change_cfg(const std::map<std::string, PluginConfigVal>& config_vals) {

	return true;
}

int PluginGain::do_process() {

	for(uint32_t i = 0; i < m_bufsize; i++) {
		out_L[i] = in_L[i];
		out_R[i] = in_R[i];
	}

	//memcpy(m_bufs_out->at(0)->get(), m_bufs_in->at(0)->get(), m_bufs_out->at(0)->size() * sizeof(float));
	//memcpy(m_bufs_out->at(1)->get(), m_bufs_in->at(1)->get(), m_bufs_out->at(1)->size() * sizeof(float));

	return 0;
}

