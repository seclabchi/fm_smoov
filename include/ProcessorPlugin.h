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
#include "plugin_config.h"
#include "fmsmoov.pb.h"


class ProcessorPlugin {
public:
	ProcessorPlugin(const std::string& _name, uint32_t samprate, uint32_t bufsize);
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
	void set_input(std::vector<AudioBuf*>* inbufs);
	std::vector<AudioBuf*>* get_output();

	const std::string& get_name();
	const PluginConfig& get_config();
protected:
	std::shared_ptr<spdlog::logger> log;
	std::string m_name;
	uint32_t m_samprate;
	std::map<std::string, ProcessorPlugin*>* m_children;
	PluginConfig* m_config;
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
	fmsmoov::ProcessorLiveData m_live_data;
};

#endif /* PROCESSORPLUGIN_H_ */
