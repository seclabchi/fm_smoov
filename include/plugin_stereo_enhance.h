/*
 * plugin_stereo_enhance.h
 *
 *  Created on: Oct 16, 2022
 *      Author: zaremba
 */

#ifndef PLUGIN_STEREO_ENHANCE_H_
#define PLUGIN_STEREO_ENHANCE_H_

#include <ProcessorPlugin.h>

class PluginStereoEnhance: public ProcessorPlugin {
public:
	PluginStereoEnhance(const std::string& _name, uint32_t samprate, uint32_t bufsize);
	virtual ~PluginStereoEnhance();
	virtual int do_process();
	virtual fmsmoov::PluginConfigResponse do_change_cfg(const fmsmoov::PluginConfig& cfg);
	virtual bool do_init(const fmsmoov::PluginConfig& cfg);
	virtual void do_set_aux_input_bufs(vector<AudioBuf*>* bufs);
	virtual void finalize_buffer_init();
private:
	float M, S;
	float m_sep_factor;
};

#endif /* PLUGIN_STEREO_ENHANCE_H_ */
