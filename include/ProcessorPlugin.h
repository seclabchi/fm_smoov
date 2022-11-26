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
#include "fmsmoov.pb.h"

using namespace std;

class ProcessorPlugin {
public:
	ProcessorPlugin(const std::string& _name, uint32_t samprate, uint32_t bufsize);
	virtual ~ProcessorPlugin();

	int process();
	virtual int do_process() = 0;
	bool init(const fmsmoov::PluginConfig& cfg);
	fmsmoov::PluginConfigResponse change_cfg(const fmsmoov::PluginConfig& cfg);
	virtual fmsmoov::PluginConfigResponse do_change_cfg(const fmsmoov::PluginConfig& cfg) = 0;
	virtual bool do_init(const fmsmoov::PluginConfig& cfg) = 0;
	virtual bool add(ProcessorPlugin* plugin);
	virtual bool remove(ProcessorPlugin* plugin);
	virtual ProcessorPlugin& get_child(uint32_t child_num);
	void set_input(std::vector<AudioBuf*>* inbufs);
	std::vector<AudioBuf*>* get_output();
	void refresh_inputs(const vector<AudioBuf*>* inputs);
	vector<AudioBuf*>* get_aux_output_bufs();
	vector<AudioBuf*>* get_aux_output_bufs(uint32_t start_index, uint32_t buf_count);
	void set_aux_input_bufs(vector<AudioBuf*>* bufs);
	void append_aux_input_bufs(vector<AudioBuf*>* bufs);
	virtual void do_set_aux_input_bufs(vector<AudioBuf*>* bufs) = 0;
	void copy_output_bufs(vector<AudioBuf*>* target);
	virtual void finalize_buffer_init() = 0;
	const std::string& get_name();
	const fmsmoov::PluginConfig& get_config();
	bool get_enabled();
	void set_enable(bool _enable);

	typedef struct {
		float* l;
		float* r;
	} PTR_PAIR;

	void set_input_LR_ptrs(const PTR_PAIR& ptrs);
	PTR_PAIR get_output_LR_ptrs();
protected:
	std::shared_ptr<spdlog::logger> log;
	bool m_enabled;
	std::string m_name;
	uint32_t m_samprate;
	fmsmoov::PluginConfig m_cfg;
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
	fmsmoov::ProcessorLiveData m_live_data;
};

#endif /* PROCESSORPLUGIN_H_ */
