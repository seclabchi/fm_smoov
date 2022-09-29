/*
 * plugin_meter.cpp
 *
 *  Created on: Sep 28, 2022
 *      Author: zaremba
 */

#include <plugin_meter.h>
#include <exception>

#define EPS 0.0000000001   //prevent log errors for zero samples

PluginMeter::PluginMeter(const string& name, uint32_t samprate, uint32_t bufsize) : ProcessorPlugin(name.c_str(), samprate, bufsize) {
	LOGT("PluginMeter CTOR");
	m_lintotL = 0.0;
	m_lintotR = 0.0;
	m_logtotL = 0.0;
	m_logtotR = 0.0;

}

PluginMeter::~PluginMeter() {
	LOGT("PluginMeter DTOR");
}

bool PluginMeter::do_init(const std::map<std::string, PluginConfigVal>& config_vals) {
	AudioBuf* btmp;

	btmp = new AudioBuf("METER_AMPLIN_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
											m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	m_buf_linL = btmp->get();

	btmp = new AudioBuf("METER_AMPLIN_R", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
											m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	m_buf_linR = btmp->get();

	btmp = new AudioBuf("METER_AMPLOG_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
											m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	m_buf_logL = btmp->get();

	btmp = new AudioBuf("METER_AMPLOG_R", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED,
											m_bufsize, NULL);

	m_bufs_out->push_back(btmp);
	m_buf_logR = btmp->get();

	return true;
}

bool PluginMeter::do_change_cfg(const std::map<std::string, PluginConfigVal>& config_vals) {
	//No runtime configurable options
	LOGW("Configuration change not supported.");
	return false;
}

int PluginMeter::do_process() {
	m_bufinL = (*m_bufs_in)[0]->get();
	m_bufinR = (*m_bufs_in)[1]->get();

	for(uint32_t i = 0; i < m_bufsize; i++) {
		m_buf_linL[i] = fabs(*(m_bufinL + i));
		m_buf_linR[i] = fabs(*(m_bufinR + i));
		m_buf_logL[i] = 20*logf(m_buf_linL[i] + EPS);
		m_buf_logR[i] = 20*logf(m_buf_linR[i] + EPS);

		m_lintotL += m_buf_linL[i];
		m_lintotR += m_buf_linR[i];
	}

	m_lintotL = m_lintotL / (float)m_bufsize;
	m_lintotR = m_lintotR / (float)m_bufsize;
	m_logtotL = 20 * log10(m_lintotL);
	m_logtotR = 20 * log10(m_lintotR);

	return 0;
}

void PluginMeter::get_total_levels(float& linL, float& linR, float& logL, float& logR) {
	linL = m_lintotL;
	linR = m_lintotR;
	logL = m_logtotL;
	logR = m_logtotR;
}

void PluginMeter::get_lin_vals(float* linL, float* linR, uint32_t bufsize) {
	if(bufsize != m_bufsize) {
		LOGC("Bufsize mismatch {} vs {}", bufsize, m_bufsize);
		return;
	}

	memcpy(linL, m_buf_linL, m_bufsize);
	memcpy(linR, m_buf_linR, m_bufsize);
}

void PluginMeter::get_log_vals(float* logL, float* logR, uint32_t bufsize) {
	if(bufsize != m_bufsize) {
		LOGC("Bufsize mismatch {} vs {}", bufsize, m_bufsize);
		return;
	}

	memcpy(logL, m_buf_logL, m_bufsize);
	memcpy(logR, m_buf_logR, m_bufsize);
}

