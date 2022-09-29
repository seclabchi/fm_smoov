/*
 * processor_core.h
 *
 *  Created on: Sep 28, 2022
 *      Author: zaremba
 */

#ifndef PROCESSOR_CORE_H_
#define PROCESSOR_CORE_H_

#include "common_defs.h"
#include "spdlog/spdlog.h"

#include "audiobuf.h"
#include "plugin_meter.h"
#include "plugin_gain.h"

using namespace std;

class ProcessorCore {
public:
	ProcessorCore(uint32_t sample_rate, uint32_t buffer_size);
	virtual ~ProcessorCore();
	void set_jack_inbufs(vector<AudioBuf*>* bufs);
	void set_jack_outbufs(vector<AudioBuf*>* bufs);
	void process();
	void get_live_data(fmsmoov::ProcessorLiveData& pld);
	void set_master_bypass(bool bypass);
private:
	ProcessorCore();

	std::shared_ptr<spdlog::logger> log;

	uint32_t m_sample_rate;
	uint32_t m_buffer_size;
	fmsmoov::ProcessorLiveData m_pld;

	bool m_master_bypass;

	PluginMeter* m_meter_master_in;
	PluginMeter* m_meter_master_out;

	PluginGain* m_gain_input;

	vector<AudioBuf*>* m_jack_inbufs;
	vector<AudioBuf*>* m_jack_outbufs;

	PluginMeter* m_meter_main_in;
	PluginMeter* m_meter_main_out;

	float m_main_inlinL, m_main_inlinR, m_main_outlinL, m_main_outlinR;
	float m_main_inlogL, m_main_inlogR, m_main_outlogL, m_main_outlogR;
};

#endif /* PROCESSOR_CORE_H_ */
