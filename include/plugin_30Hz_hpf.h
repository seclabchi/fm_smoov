/*
 * plugin_30Hz_hpf.h
 *
 *  Created on: Jul 4, 2022
 *      Author: zaremba
 */

#ifndef PLUGIN_30HZ_HPF_H_
#define PLUGIN_30HZ_HPF_H_

#include <ProcessorPlugin.h>
#include "parametric_filter.h"

#include "fmsmoov.pb.h"

class Plugin30HzHpf: public ProcessorPlugin {
public:
	Plugin30HzHpf(const std::string& _name, uint32_t samprate, uint32_t bufsize);
	virtual ~Plugin30HzHpf();
	virtual int do_process();
	virtual bool do_init(const fmsmoov::PluginConfig& cfg);
	virtual bool do_change_cfg(const fmsmoov::PluginConfig& cfg);
	virtual void do_set_aux_input_bufs(vector<AudioBuf*>* bufs);
	virtual void finalize_buffer_init();
private:
	void setup_filters();
	ParametricFilter* filtL;
	ParametricFilter* filtR;
};

#endif /* PLUGIN_30HZ_HPF_H_ */
