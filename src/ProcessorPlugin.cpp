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
									m_bufsize(bufsize), m_has_been_inited(false)
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

bool ProcessorPlugin::init(const std::map<std::string, PluginConfigVal>& config_vals) {
	bool has_been_inited = false;
	AudioBuf* buf = NULL;
	std::stringstream buf_name;

	m_has_been_inited = this->do_init(config_vals);

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

bool ProcessorPlugin::change_cfg(const std::map<std::string, PluginConfigVal>& config_vals) {
	return this->do_change_cfg(config_vals);
}

int ProcessorPlugin::process() {
	uint32_t retval = 0;

	if(false == m_has_been_inited) {
		LOGE("Plugin {} has not been intialized.", m_name);
		return -1;
	}
	else {
		in_L = m_bufs_in->at(0)->get();
		in_R = m_bufs_in->at(1)->get();
		out_L = m_bufs_out->at(0)->get();
		out_R = m_bufs_out->at(1)->get();
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

const PluginConfig& ProcessorPlugin::get_config() {
	return *m_config;
}

/* This will store the passed input buffer as a REFERENCE, regardless of the source
 * type.  And obviously, you shouldn't
 * delete the passed in buffers before removing them from here.
 * TODO: this should be probably tackled by using smart pointers.
 */
bool ProcessorPlugin::set_main_inbufs(std::vector<AudioBuf*>* bufs) {
	std::vector<AudioBuf*>::iterator it = bufs->begin();
	AudioBuf* newbuf;

	uint32_t setbufs_size = bufs->size();
	uint32_t plugin_bufs_size = m_bufs_in->size();

	if(setbufs_size != plugin_bufs_size) {
		LOGE("Attempting to set {} inbufs with {} buffers.", plugin_bufs_size, setbufs_size);
		return false;
	}

	uint32_t i = 0;
	for(AudioBuf* ab : *bufs) {
		m_bufs_in->at(i)->setptr(ab->get(), ab->size());
		i++;
	}

	return true;
}

void ProcessorPlugin::get_main_inbufs(std::vector<AudioBuf*>* inbufs) {
	inbufs->clear();
	inbufs->push_back(m_bufs_in->at(0));
	inbufs->push_back(m_bufs_in->at(1));
}

//TODO:  Smart pointers look good here
void ProcessorPlugin::get_main_outbufs(std::vector<AudioBuf*>* outbufs) {
	outbufs->clear();
	outbufs->push_back(m_bufs_out->at(0));
	outbufs->push_back(m_bufs_out->at(1));
}

void ProcessorPlugin::get_live_data(fmsmoov::ProcessorLiveData& data) {
	data.CopyFrom(m_live_data);
}
