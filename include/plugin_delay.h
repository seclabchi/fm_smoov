/*
 * plugin_delay.h
 *
 *  Created on: Oct 29, 2022
 *      Author: zaremba
 */

#ifndef PLUGIN_DELAY_H_
#define PLUGIN_DELAY_H_

#include <ProcessorPlugin.h>
#include "delay_line.h"

class PluginDelay: public ProcessorPlugin {
public:
	PluginDelay(const std::string& _name, uint32_t samprate, uint32_t bufsize);
	virtual ~PluginDelay();
	virtual int do_process();
	virtual fmsmoov::PluginConfigResponse do_change_cfg(const fmsmoov::PluginConfig& cfg);
	virtual bool do_init(const fmsmoov::PluginConfig& cfg);
	virtual void do_set_aux_input_bufs(vector<AudioBuf*>* bufs);
	virtual void finalize_buffer_init();
	void set_delay(uint32_t delay_ms);
private:

	DelayLine *delay_line;
};

#endif /* PLUGIN_TEMPLATE_H_ */
