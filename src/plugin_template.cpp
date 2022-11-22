/*
 * plugin_template.cpp
 *
 *  Created on: Oct 16, 2022
 *      Author: zaremba
 */

#include <plugin_template.h>

PluginTemplate::PluginTemplate(const std::string& _name, uint32_t samprate, uint32_t bufsize) : ProcessorPlugin(_name.c_str(), samprate, bufsize) {
	// TODO Auto-generated constructor stub

}

PluginTemplate::~PluginTemplate() {
	// TODO Auto-generated destructor stub
}

int PluginTemplate::do_process() {

}

bool PluginTemplate::do_change_cfg(const fmsmoov::PluginConfig& cfg) {

}

bool PluginTemplate::do_init(const fmsmoov::PluginConfig& cfg) {

}

void PluginTemplate::do_set_aux_input_bufs(vector<AudioBuf*>* bufs) {

}

void PluginTemplate::finalize_buffer_init() {

}
