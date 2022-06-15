/*
 * plugin_passthrough.cpp
 *
 *  Created on: Jun 14, 2022
 *      Author: zaremba
 */

#include <plugin_main.h>

PluginMain::PluginMain() : ProcessorPlugin("MAIN") {
	LOGT("PluginMain CTOR");
}

PluginMain::~PluginMain() {
	LOGT("PluginMain DTOR");
}

void PluginMain::init(std::map<std::string, PluginConfigVal> config_vals) {
	auto chans = config_vals.find("CHANS");
	if(chans != config_vals.end()) {
		LOGD("Found config val for CHANS for {}", m_name.c_str());
	}
	else {
		LOGE("Didn't find CHANS config for {}", m_name.c_str());
	}

	this->ProcessorPlugin::init(config_vals);
}

int PluginMain::process(const AudioBuf& in, AudioBuf& out) {
	return 0;
}

