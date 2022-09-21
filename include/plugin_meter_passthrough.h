/*
 * plugin_meter_passthrough.h
 *
 *  Created on: Jun 14, 2022
 *      Author: zaremba
 */

#ifndef PLUGIN_MAIN_H_
#define PLUGIN_MAIN_H_

#include <string>
#include <ProcessorPlugin.h>

using namespace std;

class PluginMeterPassthrough: public ProcessorPlugin {
public:
	PluginMeterPassthrough(const string& name, uint32_t samprate, uint32_t bufsize);
	virtual ~PluginMeterPassthrough();
	virtual int do_process();
	virtual bool do_init(const std::map<std::string, PluginConfigVal>& config_vals);
	virtual bool do_change_cfg(const std::map<std::string, PluginConfigVal>& config_vals);
};

#endif /* PLUGIN_MAIN_H_ */
