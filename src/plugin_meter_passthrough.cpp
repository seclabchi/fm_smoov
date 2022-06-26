/*
 * plugin_passthrough.cpp
 *
 *  Created on: Jun 14, 2022
 *      Author: zaremba
 */

#include <plugin_meter_passthrough.h>

PluginMeterPassthrough::PluginMeterPassthrough() : ProcessorPlugin("MAIN") {
	LOGT("PluginMain CTOR");
}

PluginMeterPassthrough::~PluginMeterPassthrough() {
	LOGT("PluginMain DTOR");
}

bool PluginMeterPassthrough::do_init(const std::map<std::string, PluginConfigVal>& config_vals) {

	//this is an example plugin, two input and two output channels

	m_chans_in = 2;
	m_chans_out = 2;

	return true;
}

int PluginMeterPassthrough::do_process() {
	memcpy(m_bufs_out->at(0)->get(), m_bufs_in->at(0)->get(), m_bufs_out->at(0)->size() * sizeof(float));
	memcpy(m_bufs_out->at(1)->get(), m_bufs_in->at(1)->get(), m_bufs_out->at(1)->size() * sizeof(float));

	return 0;
}

