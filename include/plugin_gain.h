/*
 * plugin_gain.h
 *
 *  Created on: Jul 2, 2022
 *      Author: zaremba
 */

#ifndef PLUGIN_GAIN_H_
#define PLUGIN_GAIN_H_

#include <ProcessorPlugin.h>

class PluginGain: public ProcessorPlugin {
public:
	typedef enum {
		IN_LIN_OUT_LIN = 0,
		IN_LIN_OUT_DB = 1,
		IN_DB_OUT_LIN = 2,
		IN_DB_OUT_DB = 3
	} GAIN_PROCESS_TYPE;

public:
	PluginGain(string name, uint32_t samprate, uint32_t bufsize);
	virtual ~PluginGain();
	virtual int do_process();
	virtual bool do_init(const fmsmoov::PluginConfig& cfg);
	virtual fmsmoov::PluginConfigResponse do_change_cfg(const fmsmoov::PluginConfig& cfg);
	virtual void do_set_aux_input_bufs(vector<AudioBuf*>* bufs);
	virtual void finalize_buffer_init();
	float get_gain();
private:
	float m_gain_L;
	float m_gain_R;
	float m_gainlog_L, m_gainlog_R;
};

#endif /* PLUGIN_GAIN_H_ */
