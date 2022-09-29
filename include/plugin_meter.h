/*
 * plugin_meter.h
 *
 *  Created on: Sep 28, 2022
 *      Author: zaremba
 */

#ifndef PLUGIN_METER_H_
#define PLUGIN_METER_H_

#include <string>
#include <ProcessorPlugin.h>

using namespace std;

class PluginMeter: public ProcessorPlugin {
public:
	PluginMeter(const string& name, uint32_t samprate, uint32_t bufsize);
	virtual ~PluginMeter();
	virtual int do_process();
	virtual bool do_init(const std::map<std::string, PluginConfigVal>& config_vals);
	virtual bool do_change_cfg(const std::map<std::string, PluginConfigVal>& config_vals);
	void get_total_levels(float& linL, float& linR, float& logL, float& logR);
	void get_lin_vals(float* linL, float* linR, uint32_t bufsize);
	void get_log_vals(float* logL, float* logR, uint32_t bufsize);
private:
	float m_lintotL, m_lintotR, m_logtotL, m_logtotR;
	float* m_bufinL;
	float* m_bufinR;
	float* m_buf_linL;
	float* m_buf_linR;
	float* m_buf_logL;
	float* m_buf_logR;
};

#endif /* PLUGIN_METER_H_ */
