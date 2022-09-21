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

class Plugin30HzHpf: public ProcessorPlugin {
public:
	Plugin30HzHpf(uint32_t samprate, uint32_t bufsize);
	virtual ~Plugin30HzHpf();
	virtual int do_process();
	virtual bool do_init(const std::map<std::string, PluginConfigVal>& config_vals);
	virtual bool do_change_cfg(const std::map<std::string, PluginConfigVal>& config_vals);
private:
	void setup_filters();
	ParametricFilter* filtL;
	ParametricFilter* filtR;
};

#endif /* PLUGIN_30HZ_HPF_H_ */
