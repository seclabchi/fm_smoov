/*
 * ProcessorPlugin.h
 *
 *  Created on: Jun 14, 2022
 *      Author: zaremba
 */

#ifndef PROCESSORPLUGIN_H_
#define PROCESSORPLUGIN_H_

#include "common_defs.h"

#include <string>
#include <memory>
#include <vector>
#include <map>

#include "spdlog/spdlog.h"

#include "audiobuf.h"

class PluginConfigVal {
public:
	std::string name;
	float floatval;
	uint32_t uint32val;
	int32_t int32val;
};

class ProcessorPlugin {
public:
	ProcessorPlugin(const std::string& _name);
	virtual ~ProcessorPlugin();

	virtual int process(const AudioBuf& in, AudioBuf& out) = 0;
	virtual void init(std::map<std::string, PluginConfigVal> config_vals);
	virtual bool add(ProcessorPlugin* plugin);
	virtual bool remove(ProcessorPlugin* plugin);
	virtual ProcessorPlugin& get_child(uint32_t child_num);

	const std::string getName();
protected:
	std::shared_ptr<spdlog::logger> log;
	std::string m_name;
	std::vector<ProcessorPlugin*>* m_children;
	std::vector<AudioBuf*>* m_bufs;
	bool m_has_been_inited;
};

#endif /* PROCESSORPLUGIN_H_ */
