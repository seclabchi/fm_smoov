/*
 * processor_core.cpp
 *
 *  Created on: Sep 28, 2022
 *      Author: zaremba
 */

#include <processor_core.h>

#include "spdlog/sinks/stdout_color_sinks.h"

ProcessorCore::ProcessorCore(uint32_t sample_rate, uint32_t buffer_size) :
					m_sample_rate(sample_rate), m_buffer_size(buffer_size) {
	log = spdlog::stdout_color_mt("CORE");
	log->set_pattern("%^[%H%M%S.%e][%s:%#][%n][%l] %v%$");
	log->set_level(spdlog::level::trace);
	LOGT("ProcessorCore CTOR");

	m_master_bypass = false;

	std::map<std::string, PluginConfigVal> config;
	PluginConfigVal val;

	memset(&val, 0, sizeof(PluginConfigVal));
	val.name = "DUMMY";
	val.uint32val = 42;
	config[val.name] = val;

	AudioBuf* b;
	m_jack_inbufs = new vector<AudioBuf*>();
	b = new AudioBuf("MAIN_IN_L", AudioBuf::AUDIO_BUF_TYPE::REFERENCE, m_buffer_size, NULL);
	m_jack_inbufs->push_back(b);
	b = new AudioBuf("MAIN_IN_R", AudioBuf::AUDIO_BUF_TYPE::REFERENCE, m_buffer_size, NULL);
	m_jack_inbufs->push_back(b);

	m_jack_outbufs = new vector<AudioBuf*>();
	b = new AudioBuf("MAIN_OUT_L", AudioBuf::AUDIO_BUF_TYPE::REFERENCE, m_buffer_size, NULL);
	m_jack_outbufs->push_back(b);
	b = new AudioBuf("MAIN_OUT_R", AudioBuf::AUDIO_BUF_TYPE::REFERENCE, m_buffer_size, NULL);
	m_jack_outbufs->push_back(b);

	m_meter_main_in = new PluginMeter("METER_MAIN_IN", m_sample_rate, m_buffer_size);
	m_meter_main_in->init(config);

	m_meter_main_out = new PluginMeter("METER_MAIN_OUT", m_sample_rate, m_buffer_size);
	m_meter_main_out->init(config);

	m_gain_input = new PluginGain("GAIN_INPUT", m_sample_rate, m_buffer_size);
	m_gain_input->init(config);

}

ProcessorCore::~ProcessorCore() {
	// TODO Auto-generated destructor stub
}

void ProcessorCore::set_jack_inbufs(vector<AudioBuf*>* bufs) {
	(*m_jack_inbufs)[0]->setptr((*bufs)[0]->get(), (*bufs)[0]->size());
	(*m_jack_inbufs)[1]->setptr((*bufs)[1]->get(), (*bufs)[1]->size());

}

void ProcessorCore::set_jack_outbufs(vector<AudioBuf*>* bufs) {
	(*m_jack_outbufs)[0]->setptr((*bufs)[0]->get(), (*bufs)[0]->size());
	(*m_jack_outbufs)[1]->setptr((*bufs)[1]->get(), (*bufs)[1]->size());
}

void ProcessorCore::process() {
	m_meter_main_in->set_input(m_jack_inbufs);
	m_meter_main_in->process();
	m_meter_main_in->get_total_levels(m_main_inlinL, m_main_inlinR, m_main_inlogL, m_main_inlogR);

	if(true == m_master_bypass) {
		memcpy((*m_jack_outbufs)[0]->get(), (*m_jack_inbufs)[0]->get(), m_buffer_size * sizeof(float));
		memcpy((*m_jack_outbufs)[1]->get(), (*m_jack_inbufs)[1]->get(), m_buffer_size * sizeof(float));
		m_meter_main_out->set_input(m_jack_outbufs);
	}
	else {
		m_gain_input->set_input(m_jack_inbufs);
		m_gain_input->process();
		m_meter_main_out->set_input(m_gain_input->get_output());
	}

	m_meter_main_out->process();
	m_meter_main_in->get_total_levels(m_main_outlinL, m_main_outlinR, m_main_outlogL, m_main_outlogR);

	/* Now populate the processor live data */
	m_pld.set_inl(m_main_inlogL);
	m_pld.set_inr(m_main_inlogR);
	m_pld.set_outl(m_main_outlogL);
	m_pld.set_outr(m_main_outlogR);
}

void ProcessorCore::get_live_data(fmsmoov::ProcessorLiveData& pld) {
	pld.CopyFrom(m_pld);
}

void ProcessorCore::set_master_bypass(bool bypass) {
	m_master_bypass = bypass;
}
