/*
 * plugin_phase_rotator.h
 *
 *  Created on: Oct 16, 2022
 *      Author: zaremba
 */

#ifndef PLUGIN_PHASE_ROTATOR_H_
#define PLUGIN_PHASE_ROTATOR_H_

#include <ProcessorPlugin.h>
#include "parametric_filter.h"

class PluginPhaseRotator: public ProcessorPlugin {
public:
	PluginPhaseRotator(const std::string& _name, uint32_t samprate, uint32_t bufsize);
	virtual ~PluginPhaseRotator();
	virtual int do_process();
	virtual fmsmoov::PluginConfigResponse do_change_cfg(const fmsmoov::PluginConfig& cfg);
	virtual bool do_init(const fmsmoov::PluginConfig& cfg);
	virtual void do_set_aux_input_bufs(vector<AudioBuf*>* bufs);
	virtual void finalize_buffer_init();
private:
	ParametricFilter* rotL1, *rotL2, *rotL3, *rotL4;
	ParametricFilter* rotR1, *rotR2, *rotR3, *rotR4;
};

#endif /* PLUGIN_PHASE_ROTATOR_H_ */
