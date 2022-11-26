/*
 * plugin_template.h
 *
 *  Created on: Oct 16, 2022
 *      Author: zaremba
 */

#ifndef PLUGIN_TEMPLATE_H_
#define PLUGIN_TEMPLATE_H_

#include <ProcessorPlugin.h>

class PluginTemplate: public ProcessorPlugin {
public:
	PluginTemplate(const std::string& _name, uint32_t samprate, uint32_t bufsize);
	virtual ~PluginTemplate();
	virtual int do_process();
	virtual fmsmoov::PluginConfigResponse do_change_cfg(const fmsmoov::PluginConfig& cfg);
	virtual bool do_init(const fmsmoov::PluginConfig& cfg);
	virtual void do_set_aux_input_bufs(vector<AudioBuf*>* bufs);
	virtual void finalize_buffer_init();
};

#endif /* PLUGIN_TEMPLATE_H_ */
