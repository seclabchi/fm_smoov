/*
 * plugin_precomp_eq.cpp
 *
 *  Created on: Dec 11, 2022
 *      Author: zaremba
 */

#include <plugin_precomp_eq.h>

PluginPrecompEq::PluginPrecompEq(const std::string& _name, uint32_t samprate, uint32_t bufsize) : ProcessorPlugin(_name.c_str(), samprate, bufsize) {
	// TODO Auto-generated constructor stub

}

PluginPrecompEq::~PluginPrecompEq() {
	// TODO Auto-generated destructor stub
}

int PluginPrecompEq::do_process() {
	m_bass_shelf_boost_L->process(in_L, out_L);
	m_bass_shelf_boost_R->process(in_R, out_R);
	//memcpy(out_L, in_L, m_bufsize*sizeof(float));
	//memcpy(out_R, in_R, m_bufsize*sizeof(float));
}

fmsmoov::PluginConfigResponse PluginPrecompEq::do_change_cfg(const fmsmoov::PluginConfig& cfg) {
	fmsmoov::PluginConfigResponse pcr;
	return pcr;
}

bool PluginPrecompEq::do_init(const fmsmoov::PluginConfig& cfg) {
	m_bass_shelf_boost_L = new ParametricFilter(m_samprate, m_bufsize, 90.0, 0.707,
			3.0, "bassshelf", "plugin_precomp_eq_bass_shelf_boost");
	m_bass_shelf_boost_R = new ParametricFilter(m_samprate, m_bufsize, 90.0, 0.707,
			3.0, "bassshelf", "plugin_precomp_eq_bass_shelf_boost");
}

void PluginPrecompEq::do_set_aux_input_bufs(vector<AudioBuf*>* bufs) {

}

void PluginPrecompEq::finalize_buffer_init() {

}
