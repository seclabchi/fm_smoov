/*
 * plugin_meter.cpp
 *
 *  Created on: Sep 28, 2022
 *      Author: zaremba
 */

#include <plugin_meter.h>
#include <exception>

PluginMeter::PluginMeter(const string& name, uint32_t samprate, uint32_t bufsize) : ProcessorPlugin(name.c_str(), samprate, bufsize) {
	LOGT("PluginMeter CTOR");

	m_tau = 0.010;  //10 ms integration time (PPL EBU standard)
	m_alpha = expf(-logf(9.0)/(m_samprate * m_tau));
	m_prevlev_L = EPS;
	m_prevlev_R = EPS;

	m_lintotL = 0.0;
	m_lintotR = 0.0;
	m_logtotL = 0.0;
	m_logtotR = 0.0;

}

PluginMeter::~PluginMeter() {
	LOGT("PluginMeter DTOR");
}

bool PluginMeter::do_init(const fmsmoov::PluginConfig& cfg) {
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

fmsmoov::PluginConfigResponse PluginMeter::do_change_cfg(const fmsmoov::PluginConfig& cfg) {
	fmsmoov::PluginConfigResponse pcr;

	//No runtime configurable options
	LOGW("Configuration change not supported.");

	return pcr;
}

void PluginMeter::do_set_aux_input_bufs(vector<AudioBuf*>* bufs) {

}

void PluginMeter::finalize_buffer_init() {

}


/* Measure peak values for now */
int PluginMeter::do_process() {
	m_lintotL = 0.0;
	m_lintotR = 0.0;

	/* Assume this is a EBU Peak Program Level meter with 10ms integration time for now */
	for(uint32_t i = 0; i < m_bufsize; i++) {
		m_cursamp_L = fabs(in_L[i]);
		m_cursamp_R = fabs(in_R[i]);

		m_buf_linL[i] = m_alpha*m_prevlev_L + (1.0f-m_alpha)*m_cursamp_L;
		m_buf_logL[i] = 20*log10(m_buf_linL[i]);
		m_prevlev_L = m_buf_linL[i];
		m_buf_linR[i] = m_alpha*m_prevlev_R + (1.0f-m_alpha)*m_cursamp_R;
		m_buf_logR[i] = 20*log10(m_buf_linR[i]);
		m_prevlev_R = m_buf_linR[i];

		if(m_lintotL < m_buf_linL[i]) {
			m_lintotL = m_buf_linL[i];
		}

		if(m_lintotR < m_buf_linR[i]) {
			m_lintotR = m_buf_linR[i];
		}
	}

	m_logtotL = 20 * log10f(m_lintotL);
	m_logtotR = 20 * log10f(m_lintotR);

	memcpy(out_L, in_L, m_bufsize * sizeof(float));
	memcpy(out_R, in_R, m_bufsize * sizeof(float));

	return 0;
}

void PluginMeter::get_total_levels(float& linL, float& linR, float& logL, float& logR) {
	linL = m_lintotL;
	linR = m_lintotR;
	logL = m_logtotL;
	logR = m_logtotR;
}

void PluginMeter::get_total_levels_log(float& logL, float& logR) {
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

