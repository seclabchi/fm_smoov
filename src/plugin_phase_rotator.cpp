/*
 * plugin_phase_rotator.cpp
 *
 *  Created on: Oct 16, 2022
 *      Author: zaremba
 */

#include <plugin_phase_rotator.h>

PluginPhaseRotator::PluginPhaseRotator(const std::string& _name, uint32_t samprate, uint32_t bufsize) : ProcessorPlugin(_name.c_str(), samprate, bufsize) {
	// TODO Auto-generated constructor stub
}

PluginPhaseRotator::~PluginPhaseRotator() {
	// TODO Auto-generated destructor stub
}

int PluginPhaseRotator::do_process() {
	if(m_enabled) {
		rotL1->process(in_L, in_L);
		rotL2->process(in_L, in_L);
		rotL3->process(in_L, in_L);
		rotL4->process(in_L, out_L);

		rotR1->process(in_R, in_R);
		rotR2->process(in_R, in_R);
		rotR3->process(in_R, in_R);
		rotR4->process(in_R, out_R);
	}
	else {
		memcpy(out_L, in_L, m_bufsize * sizeof(float));
		memcpy(out_R, in_R, m_bufsize * sizeof(float));
	}
}

bool PluginPhaseRotator::do_change_cfg(const fmsmoov::PluginConfig& cfg) {

}

bool PluginPhaseRotator::do_init(const fmsmoov::PluginConfig& cfg) {
	rotL1 = new ParametricFilter(m_samprate, m_bufsize, 200.0f, 1.0f,
			0.0f, "allpass", "phase_rotator_1");
	rotL2 = new ParametricFilter(m_samprate, m_bufsize, 200.0f, 1.0f,
				0.0f, "allpass", "phase_rotator_1");
	rotL3 = new ParametricFilter(m_samprate, m_bufsize, 200.0f, 1.0f,
				0.0f, "allpass", "phase_rotator_1");
	rotL4 = new ParametricFilter(m_samprate, m_bufsize, 200.0f, 1.0f,
				0.0f, "allpass", "phase_rotator_1");
	rotR1 = new ParametricFilter(m_samprate, m_bufsize, 200.0f, 1.0f,
				0.0f, "allpass", "phase_rotator_2");
	rotR2 = new ParametricFilter(m_samprate, m_bufsize, 200.0f, 1.0f,
					0.0f, "allpass", "phase_rotator_2");
	rotR3 = new ParametricFilter(m_samprate, m_bufsize, 200.0f, 1.0f,
					0.0f, "allpass", "phase_rotator_2");
	rotR4 = new ParametricFilter(m_samprate, m_bufsize, 200.0f, 1.0f,
					0.0f, "allpass", "phase_rotator_2");
}

void PluginPhaseRotator::do_set_aux_input_bufs(vector<AudioBuf*>* bufs) {

}

void PluginPhaseRotator::finalize_buffer_init() {

}
