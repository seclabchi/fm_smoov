/*
 * plugin_main.h
 *
 *  Created on: Jun 14, 2022
 *      Author: zaremba
 */

#ifndef PLUGIN_MAIN_H_
#define PLUGIN_MAIN_H_

#include <ProcessorPlugin.h>

class PluginMain: public ProcessorPlugin {
public:
	PluginMain();
	virtual ~PluginMain();
	virtual int process(const AudioBuf& in, AudioBuf& out);
	virtual void init(std::map<std::string, PluginConfigVal> config_vals);
};

#endif /* PLUGIN_MAIN_H_ */
