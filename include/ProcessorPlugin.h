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

	int process();
	virtual int do_process() = 0;
	bool init(const std::map<std::string, PluginConfigVal>& config_vals);
	bool change_cfg(const std::map<std::string, PluginConfigVal>& config_vals);
	virtual bool do_change_cfg(const std::map<std::string, PluginConfigVal>& config_vals) = 0;
	virtual bool do_init(const std::map<std::string, PluginConfigVal>& config_vals) = 0;
	virtual bool add(ProcessorPlugin* plugin);
	virtual bool remove(ProcessorPlugin* plugin);
	virtual ProcessorPlugin& get_child(uint32_t child_num);
	bool set_main_inbufs(std::vector<AudioBuf*>* bufs);
	void get_main_outbufs(std::vector<AudioBuf*>* outbufs);

	const std::string get_name();
protected:
	std::shared_ptr<spdlog::logger> log;
	std::string m_name;
	std::map<std::string, ProcessorPlugin*>* m_children;
	std::vector<AudioBuf*>* m_bufs_in;
	std::vector<AudioBuf*>* m_bufs_out;
	bool m_has_been_inited;
	uint32_t m_chans_in;
	uint32_t m_chans_out;
	uint32_t m_bufsize;
	//TODO: multichannel support?
	float* in_L;
	float* in_R;
	float* out_L;
	float* out_R;
};

#endif /* PROCESSORPLUGIN_H_ */
