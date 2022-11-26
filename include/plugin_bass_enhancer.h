/*
 * plugin_bass_enhancer.h
 *
 *  Created on: Nov 02, 2022
 *      Author: zaremba
 */

#ifndef PLUGIN_BASS_ENHANCER_H_
#define PLUGIN_BASS_ENHANCER_H_

#include <ProcessorPlugin.h>

#include "sos.h"
#include "filter_lr4.h"

class PluginBassEnhancer: public ProcessorPlugin {
public:
	PluginBassEnhancer(const std::string& _name, uint32_t samprate, uint32_t bufsize);
	virtual ~PluginBassEnhancer();
	virtual int do_process();
	virtual fmsmoov::PluginConfigResponse do_change_cfg(const fmsmoov::PluginConfig& cfg);
	virtual bool do_init(const fmsmoov::PluginConfig& cfg);
	virtual void do_set_aux_input_bufs(vector<AudioBuf*>* bufs);
	virtual void finalize_buffer_init();
private:
	SOS* belo, *behi;
	FilterLR4* cross_loL, *cross_loR, *cross_hiL, *cross_hiR;

	float* incross_lo_outL, *incross_lo_outR, *incross_hi_outL, *incross_hi_outR;
	float* monolo;
};

#endif /* PLUGIN_BASS_ENHANCER_H_ */
