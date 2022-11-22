/*
 * plugin_stereo_enhance.cpp
 *
 *  Created on: Oct 16, 2022
 *      Author: zaremba
 */

#include <plugin_stereo_enhance.h>

PluginStereoEnhance::PluginStereoEnhance(const std::string& _name, uint32_t samprate, uint32_t bufsize) : ProcessorPlugin(_name.c_str(), samprate, bufsize) {
	// TODO Auto-generated constructor stub
	m_sep_factor = 0.5;
}

PluginStereoEnhance::~PluginStereoEnhance() {
	// TODO Auto-generated destructor stub
}

int PluginStereoEnhance::do_process() {
	if(m_enabled) {
		for(uint32_t i = 0; i < m_bufsize; i++)
		{
			M = (in_L[i] + in_R[i]) / 2.0;
			S = (in_L[i] - in_R[i]) / 2.0;

			M = M * 0.85;
			S = S * 1.15;

			out_L[i] = M + S;
			out_R[i] = M - S;
		}
	}
	else {
		memcpy(out_L, in_L, m_bufsize * sizeof(float));
		memcpy(out_R, in_R, m_bufsize * sizeof(float));
	}
}

bool PluginStereoEnhance::do_change_cfg(const fmsmoov::PluginConfig& cfg) {

}

bool PluginStereoEnhance::do_init(const fmsmoov::PluginConfig& cfg) {

}

void PluginStereoEnhance::do_set_aux_input_bufs(vector<AudioBuf*>* bufs) {

}

void PluginStereoEnhance::finalize_buffer_init() {

}
