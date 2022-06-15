/*
 * ProcessorPlugin.cpp
 *
 *  Created on: Jun 14, 2022
 *      Author: zaremba
 */


#include "ProcessorPlugin.h"

#include <iterator>

#include "spdlog/sinks/stdout_color_sinks.h"

ProcessorPlugin::ProcessorPlugin(const std::string& _name) : m_name(_name),
									m_has_been_inited(false)
{
	log = spdlog::stdout_color_mt("PLUGIN");
	log->set_pattern("%^[%H%M%S.%e][%s:%#][%n][%l] %v%$");
	log->set_level(spdlog::level::trace);
	m_children = new std::vector<ProcessorPlugin*>();
	m_bufs = new std::vector<AudioBuf*>();
}

ProcessorPlugin::~ProcessorPlugin() {
	for(ProcessorPlugin* p : *m_children) {
		delete p;
	}
	delete m_children;

	for(AudioBuf* b : *m_bufs) {
		delete b;
	}
	delete m_bufs;

}

void ProcessorPlugin::init(std::map<std::string, PluginConfigVal> config_vals) {
	m_has_been_inited = true;
}

bool ProcessorPlugin::add(ProcessorPlugin* plugin) {
	bool result = true;

	if(plugin == this) {
		LOGE("Can't add plugin {} to itself.", plugin->getName());
		result = false;
		goto returnval;
	}

	for(ProcessorPlugin* p : *m_children) {
		if(p == plugin) {
			//cannot add a duplicate memory address.  This plugin was
			//already added to the chain.
			result = false;
			LOGE("Cannot add duplicate plugin to {}.  Address is the same.", m_name);
			break;
		}
	}
	if(true == result) {
		m_children->push_back(plugin);
	}

	returnval:
	return result;
}

bool ProcessorPlugin::remove(ProcessorPlugin* plugin) {
	bool result = false;

	std::vector<ProcessorPlugin*>::iterator it = m_children->begin();

	while(it != m_children->end()) {
		if(*it == plugin) {
			delete *it;
			m_children->erase(it);
			result = true;
			break;
		}
		it++;
	}

	return result;
}

ProcessorPlugin& ProcessorPlugin::get_child(uint32_t child_num) {
	//TODO: this is broken in the name of continuous integration
	return *this;
}

const std::string ProcessorPlugin::getName() {
	return m_name;
}
