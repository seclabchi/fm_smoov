/*
 * plugin_passthrough.cpp
 *
 *  Created on: Jun 14, 2022
 *      Author: zaremba
 */

#include <plugin_meter_passthrough.h>

PluginMeterPassthrough::PluginMeterPassthrough(const string& name, uint32_t samprate, uint32_t bufsize) : ProcessorPlugin(name.c_str(), samprate, bufsize) {
	LOGT("PluginMeterPassthrough CTOR");
}

PluginMeterPassthrough::~PluginMeterPassthrough() {
	LOGT("PluginMeterPassthrough DTOR");
}

bool PluginMeterPassthrough::do_init(const std::map<std::string, PluginConfigVal>& config_vals) {

	//this plugin has two input buffers for now:  the left and right inputs.

	AudioBuf* btmp = new AudioBuf("METER_PT_IN_L", AudioBuf::AUDIO_BUF_TYPE::REFERENCE,
									m_bufsize, NULL);

	m_bufs_in->push_back(btmp);

	btmp = new AudioBuf("METER_PT_IN_R", AudioBuf::AUDIO_BUF_TYPE::REFERENCE,
										m_bufsize, NULL);

	m_bufs_in->push_back(btmp);

	btmp = new AudioBuf("METER_PT_OUT_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
											m_bufsize, NULL);

	//this plugin has six output buffers for now:  the L and R passthrough, the absolute amplitude,
	//and the absolute amplitude in dB of the left and right channels.

	m_bufs_out->push_back(btmp);

	btmp = new AudioBuf("METER_PT_OUT_R", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
											m_bufsize, NULL);

	m_bufs_out->push_back(btmp);

	btmp = new AudioBuf("METER_PT_AMPLIN_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
											m_bufsize, NULL);

	m_bufs_out->push_back(btmp);

	btmp = new AudioBuf("METER_PT_AMPLIN_R", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
											m_bufsize, NULL);

	m_bufs_out->push_back(btmp);

	btmp = new AudioBuf("METER_PT_AMPLOG_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
											m_bufsize, NULL);

	m_bufs_out->push_back(btmp);

	btmp = new AudioBuf("METER_PT_AMPLOG_R", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
											m_bufsize, NULL);

	m_bufs_out->push_back(btmp);



	return true;
}

bool PluginMeterPassthrough::do_change_cfg(const std::map<std::string, PluginConfigVal>& config_vals) {
	//No runtime configurable options
	LOGW("Configuration change not supported.");
	return false;
}

int PluginMeterPassthrough::do_process() {

	float inL = 0.0;
	float inR = 0.0;

	float* bufinL = m_bufs_in->at(0)->get();
	float* bufinR = m_bufs_in->at(1)->get();

	for(uint32_t i = 0; i < m_bufsize; i++) {
		inL += fabs(*bufinL);
		inR += fabs(*bufinR);
		bufinL++;
		bufinR++;
	}

	inL = 20.0 * log10(inL / m_bufsize);
	inR = 20.0 * log10(inR / m_bufsize);

	m_live_data.set_inl(inL);
	m_live_data.set_inr(inR);

	memcpy(m_bufs_out->at(0)->get(), m_bufs_in->at(0)->get(), m_bufs_out->at(0)->size() * sizeof(float));
	memcpy(m_bufs_out->at(1)->get(), m_bufs_in->at(1)->get(), m_bufs_out->at(1)->size() * sizeof(float));

	return 0;
}

