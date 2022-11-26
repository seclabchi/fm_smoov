/*
 * ProcessorPlugin.cpp
 *
 *  Created on: Jun 14, 2022
 *      Author: zaremba
 */


#include "ProcessorPlugin.h"

#include <iterator>
#include <string>
#include <sstream>

#include "spdlog/sinks/stdout_color_sinks.h"

ProcessorPlugin::ProcessorPlugin(const std::string& _name, uint32_t samprate, uint32_t bufsize) : m_name(_name), m_samprate(samprate),
									m_has_been_inited(false), m_bufsize(bufsize)
{
	std::stringstream logname;
	logname << "PLUGIN_" << m_name;
	log = spdlog::stdout_color_mt(logname.str());
	log->set_pattern("%^[%H%M%S.%e][%s:%#][%n][%l] %v%$");
	log->set_level(spdlog::level::trace);
	m_children = new std::map<std::string, ProcessorPlugin*>();
	m_bufs_in = new std::vector<AudioBuf*>();
	m_bufs_out = new std::vector<AudioBuf*>();
	in_L = nullptr;
	in_R = nullptr;
	out_L = nullptr;
	out_R = nullptr;
	m_enabled = true;
}

ProcessorPlugin::~ProcessorPlugin() {
	std::map<std::string, ProcessorPlugin*>::iterator it;

	it = m_children->begin();

	while(it != m_children->end()) {
		delete it->second;
	}

	delete m_children;

	for(AudioBuf* b : *m_bufs_in) {
		delete b;
	}

	delete m_bufs_in;

	for(AudioBuf* b : *m_bufs_out) {
		delete b;
	}

	delete m_bufs_out;

}



bool ProcessorPlugin::init(const fmsmoov::PluginConfig& cfg) {
	std::stringstream buf_name;

	/* Assume at least two input buffers.
	 * TODO: this is becoming a mess, needs rework at some point.
	 */
	m_bufs_in->clear();
	m_bufs_in->push_back(new AudioBuf("IN_L", AudioBuf::REFERENCE, m_bufsize, NULL));
	in_L = m_bufs_in->at(0)->get();
	m_bufs_in->push_back(new AudioBuf("IN_R", AudioBuf::REFERENCE, m_bufsize, NULL));
	in_R = m_bufs_in->at(1)->get();

	m_bufs_out->clear();
	m_bufs_out->push_back(new AudioBuf("OUT_L", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED, m_bufsize, NULL));
	out_L = m_bufs_out->at(0)->get();
	m_bufs_out->push_back(new AudioBuf("OUT_R", AudioBuf::AUDIO_BUF_TYPE::ALLOCATED, m_bufsize, NULL));
	out_R = m_bufs_out->at(1)->get();

	m_has_been_inited = this->do_init(cfg);

	if(m_bufs_out->size() >= 2) {
		out_L = m_bufs_out->at(0)->get();
		out_R = m_bufs_out->at(1)->get();
	}
	else {
		LOGI("Plugin has less than two output buffers.  No output available.");
		out_L = nullptr;
		out_R = nullptr;
	}

	if(m_has_been_inited) {
		m_chans_in = m_bufs_in->size();
		m_chans_out = m_bufs_out->size();
		LOGI("{} has {} input and {} output channels.", m_name, m_chans_in, m_chans_out); // @suppress("Invalid arguments")
	}
	else {
		LOGE("Init failed for plugin {}", m_name);
	}

	return m_has_been_inited;
}

fmsmoov::PluginConfigResponse ProcessorPlugin::change_cfg(const fmsmoov::PluginConfig& cfg) {
	return this->do_change_cfg(cfg);
}

bool ProcessorPlugin::get_enabled() {
	return m_enabled;
}

void ProcessorPlugin::set_enable(bool _enable) {
	m_enabled = _enable;
}

int ProcessorPlugin::process() {
	if(false == m_has_been_inited) {
		LOGC("Plugin {} has not been intialized.", m_name);
		exit(-1);
	}
	else {
		return this->do_process();
	}
}

bool ProcessorPlugin::add(ProcessorPlugin* plugin) {
	bool result = true;
	std::map<std::string, ProcessorPlugin*>::iterator it = m_children->begin();

	if(plugin == this) {
		LOGE("Can't add plugin {} to itself.", plugin->get_name());
		result = false;
		goto returnval;
	}

	while(it != m_children->end()) {
		if(it->second == plugin) {
			//cannot add a duplicate memory address.  This plugin was
			//already added to the chain.
			result = false;
			LOGE("Cannot add duplicate plugin to {}.  Address is the same.", m_name);
			break;
		}
	}

	if(true == result) {
		(*m_children)[m_name] = plugin;
	}

	returnval:
	return result;
}

bool ProcessorPlugin::remove(ProcessorPlugin* plugin) {
	auto target_plugin = m_children->find(plugin->get_name());

	if(target_plugin == m_children->end()) {
		LOGE("Unable to find plugin with name {}", plugin->get_name());
		return false;
	}

	delete target_plugin->second;

	return true;
}

ProcessorPlugin& ProcessorPlugin::get_child(uint32_t child_num) {
	//TODO: this is broken in the name of continuous integration
	return *this;
}

const std::string& ProcessorPlugin::get_name() {
	return m_name;
}

const fmsmoov::PluginConfig& ProcessorPlugin::get_config() {
	return m_cfg;
}

void ProcessorPlugin::set_input(std::vector<AudioBuf*>* inbufs) {
	m_bufs_in->clear();
	m_bufs_in->push_back(new AudioBuf("IN_L", AudioBuf::REFERENCE, inbufs->at(0)->size(), inbufs->at(0)->get()));
	m_bufs_in->push_back(new AudioBuf("IN_R", AudioBuf::REFERENCE, inbufs->at(1)->size(), inbufs->at(1)->get()));
	in_L = m_bufs_in->at(0)->get();
	in_R = m_bufs_in->at(1)->get();
	LOGI("{} now has {} input and {} output channels.", m_name, m_bufs_in->size(), m_bufs_out->size()); // @suppress("Invalid arguments")
}

std::vector<AudioBuf*>* ProcessorPlugin::get_output() {
	return m_bufs_out;
}

void ProcessorPlugin::refresh_inputs(const vector<AudioBuf*>* inputs) {
	m_bufs_in->at(0)->setptr(inputs->at(0)->get(), m_bufsize);
	m_bufs_in->at(1)->setptr(inputs->at(1)->get(), m_bufsize);
	in_L = m_bufs_in->at(0)->get();
	in_R = m_bufs_in->at(1)->get();
}

void ProcessorPlugin::copy_output_bufs(vector<AudioBuf*>* target) {
	memcpy(target->at(0)->get(), m_bufs_out->at(0)->get(), m_bufsize * sizeof(float));
	memcpy(target->at(1)->get(), m_bufs_out->at(1)->get(), m_bufsize * sizeof(float));
}


vector<AudioBuf*>* ProcessorPlugin::get_aux_output_bufs() {
	vector<AudioBuf*>* ptrs = new vector<AudioBuf*>();
	for(uint32_t i = 2; i < m_bufs_out->size(); i++) {
		ptrs->push_back(m_bufs_out->at(i));
	}

	return ptrs;
}

vector<AudioBuf*>* ProcessorPlugin::get_aux_output_bufs(uint32_t start_index, uint32_t buf_count) {
	vector<AudioBuf*>* ptrs = new vector<AudioBuf*>();
	for(uint32_t i = 2 + start_index; i < 2 + start_index + buf_count; i++) {
		ptrs->push_back(m_bufs_out->at(i));
	}

	return ptrs;
}

void ProcessorPlugin::set_aux_input_bufs(vector<AudioBuf*>* bufs) {
	uint32_t localbufcnt = m_bufs_in->size();
	localbufcnt = localbufcnt - 2;  //number of existing aux in bufs
	LOGD("{} existing aux bufs, setting {} aux bufs", localbufcnt, bufs->size());

	for(uint32_t i = 0; i < localbufcnt; i++) {
		m_bufs_in->pop_back();
	}

	for(uint32_t i = 0; i < bufs->size(); i++) {
		m_bufs_in->push_back(bufs->at(i));
	}

	this->do_set_aux_input_bufs(bufs);

}

void ProcessorPlugin::append_aux_input_bufs(vector<AudioBuf*>* bufs) {
	uint32_t localbufcnt = m_bufs_in->size();
	localbufcnt = localbufcnt - 2;  //number of existing aux in bufs

	LOGD("{} existing aux bufs, adding {} aux bufs", localbufcnt, bufs->size());

	for(uint32_t i = 0; i < bufs->size(); i++) {
		m_bufs_in->push_back(bufs->at(i));
	}

	this->do_set_aux_input_bufs(bufs);
}
