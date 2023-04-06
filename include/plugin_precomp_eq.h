/*
 * plugin_precomp_eq.h
 *
 *  Created on: Dec 11, 2022
 *      Author: zaremba
 */

#ifndef PLUGIN_PRECOMP_EQ_H_
#define PLUGIN_PRECOMP_EQ_H_

#include <ProcessorPlugin.h>
#include "parametric_filter.h"

class PluginPrecompEq: public ProcessorPlugin {
public:
	PluginPrecompEq(const std::string& _name, uint32_t samprate, uint32_t bufsize);
	virtual ~PluginPrecompEq();
	virtual int do_process();
	virtual fmsmoov::PluginConfigResponse do_change_cfg(const fmsmoov::PluginConfig& cfg);
	virtual bool do_init(const fmsmoov::PluginConfig& cfg);
	virtual void do_set_aux_input_bufs(vector<AudioBuf*>* bufs);
	virtual void finalize_buffer_init();
private:
	ParametricFilter* m_bass_shelf_boost_L, *m_bass_shelf_boost_R;
};

#endif /* PLUGIN_PRECOMP_EQ_H_ */
