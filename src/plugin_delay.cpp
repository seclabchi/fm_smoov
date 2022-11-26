/*
 * plugin_template.cpp
 *
 *  Created on: Oct 16, 2022
 *      Author: zaremba
 */

#include <plugin_delay.h>

PluginDelay::PluginDelay(const std::string& _name, uint32_t samprate, uint32_t bufsize) : ProcessorPlugin(_name.c_str(), samprate, bufsize) {
	// TODO Auto-generated constructor stub
	delay_line = new DelayLine(m_samprate, 0.0, m_bufsize);
}

PluginDelay::~PluginDelay() {
	// TODO Auto-generated destructor stub
}

int PluginDelay::do_process() {
	if(m_enabled) {
		//memset(out_L, 0, m_bufsize*sizeof(float));
		//memset(out_R, 0, m_bufsize*sizeof(float));
		delay_line->process(in_L, in_R, out_L, out_R);
	}
	else {
		memcpy(out_L, in_L, m_bufsize * sizeof(float));
		memcpy(out_R, in_R, m_bufsize * sizeof(float));
	}
}

fmsmoov::PluginConfigResponse PluginDelay::do_change_cfg(const fmsmoov::PluginConfig& cfg) {
	fmsmoov::PluginConfigResponse pcr;
	return pcr;
}

bool PluginDelay::do_init(const fmsmoov::PluginConfig& cfg) {
	return true;
}

void PluginDelay::do_set_aux_input_bufs(vector<AudioBuf*>* bufs) {

}

void PluginDelay::finalize_buffer_init() {

}

void PluginDelay::set_delay(uint32_t delay_ms) {
	delay_line->change_delay(delay_ms);
}
