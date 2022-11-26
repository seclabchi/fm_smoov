/*
 * plugin_meter.h
 *
 *  Created on: Sep 28, 2022
 *      Author: zaremba
 */

#ifndef PLUGIN_METER_H_
#define PLUGIN_METER_H_

#define EPS 0.0000001;

#include <string>
#include <ProcessorPlugin.h>

using namespace std;

class PluginMeter: public ProcessorPlugin {
public:
	PluginMeter(const string& name, uint32_t samprate, uint32_t bufsize);
	virtual ~PluginMeter();
	virtual int do_process();
	virtual bool do_init(const fmsmoov::PluginConfig& cfg);
	virtual fmsmoov::PluginConfigResponse do_change_cfg(const fmsmoov::PluginConfig& cfg);
	virtual void do_set_aux_input_bufs(vector<AudioBuf*>* bufs);
	virtual void finalize_buffer_init();
	void get_total_levels(float& linL, float& linR, float& logL, float& logR);
	void get_total_levels_log(float& logL, float& logR);
	void get_lin_vals(float* linL, float* linR, uint32_t bufsize);
	void get_log_vals(float* logL, float* logR, uint32_t bufsize);
private:
	float m_alpha;
	float m_tau;
	float m_cursamp_L, m_cursamp_R;
	float m_prevlev_L, m_prevlev_R;

	float m_lintotL, m_lintotR, m_logtotL, m_logtotR;
	float* m_bufinL;
	float* m_bufinR;
	float* m_buf_linL;
	float* m_buf_linR;
	float* m_buf_logL;
	float* m_buf_logR;



};

#endif /* PLUGIN_METER_H_ */
