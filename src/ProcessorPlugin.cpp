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

ProcessorPlugin::ProcessorPlugin(const std::string& _name) : m_name(_name),
									m_has_been_inited(false)
{
	std::stringstream logname;
	logname << "PLUGIN_" << m_name;
	log = spdlog::stdout_color_mt(logname.str());
	log->set_pattern("%^[%H%M%S.%e][%s:%#][%n][%l] %v%$");
	log->set_level(spdlog::level::trace);
	m_children = new std::map<std::string, ProcessorPlugin*>();
	m_bufs_in = new std::vector<AudioBuf*>();
	m_bufs_out = new std::vector<AudioBuf*>();
}

ProcessorPlugin::~ProcessorPlugin() {
	std::map<std::string, ProcessorPlugin*>::iterator it;

	it = m_children->begin();

	while(it != m_children->end()) {
		delete it->second;
	}

	delete m_children;

	for(uint32_t i = 0; i < m_chans_out; i++) {
		delete m_bufs_out->at(i);
	}

	delete m_bufs_out;

}

bool ProcessorPlugin::init(const std::map<std::string, PluginConfigVal>& config_vals) {
	bool has_been_inited = false;
	AudioBuf* buf = NULL;
	std::stringstream buf_name;

	auto bufsize = config_vals.find("BUFSIZE");

	if(bufsize != config_vals.end()) {
		LOGD("Found config val for BUFSIZE for {}", m_name.c_str());
	}
	else {
		LOGE("Didn't find BUFSIZE config for {}", m_name.c_str());
		goto returnval;
	}

	m_chans_in = 0;
	m_chans_out = 0;
	m_bufsize = bufsize->second.uint32val;

	m_has_been_inited = this->do_init(config_vals);

	LOGI("{} has {} input and {} output channels.", this->m_name, m_chans_in, m_chans_out);

	/*for(uint32_t i = 0; i < m_chans_out; i++) {
		buf_name.str("");
		buf_name << "IN_CHAN_" << i;
		m_bufs_in->push_back(new AudioBuf(buf_name.str(), AudioBuf::AUDIO_BUF_TYPE::REFERENCE, m_bufsize));
	}
	*/
	for(uint32_t i = 0; i < m_chans_out; i++) {
		buf_name.str("");
		buf_name << m_name << "_OUT_CHAN_" << i;
		m_bufs_out->push_back(new AudioBuf(buf_name.str(), AudioBuf::AUDIO_BUF_TYPE::ALLOCATED, m_bufsize));
	}

	returnval:
	return m_has_been_inited;
}

int ProcessorPlugin::process() {
	uint32_t retval = 0;

	if(false == m_has_been_inited) {
		LOGE("Plugin {} has not been intialized.", m_name);
		return -1;
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

const std::string ProcessorPlugin::get_name() {
	return m_name;
}

/* The buffers to set MUST be references.  And obviously, you shouldn't
 * delete the passed in buffers before removing them from here.
 * TODO: this should be probably tackled by using smart pointers.
 */
void ProcessorPlugin::set_inbufs(std::vector<AudioBuf*>* bufs) {
	std::vector<AudioBuf*>::iterator it = bufs->begin();

	m_bufs_in->clear();

	for(AudioBuf* ab : *bufs) {
		if(ab->type() != AudioBuf::AUDIO_BUF_TYPE::REFERENCE) {
			LOGE("Can't set inbufs with non-reference type buf named {}.", ab->name());
		}
		m_bufs_in->push_back(ab);
	}

}

std::vector<AudioBuf*>* ProcessorPlugin::get_outbufs() {
	return m_bufs_out;
}
