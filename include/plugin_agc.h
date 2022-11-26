/*
 * plugin_agc.h
 *
 *  Created on: Sep 30, 2022
 *      Author: zaremba
 */

#ifndef PLUGIN_AGC_H_
#define PLUGIN_AGC_H_

#include <ProcessorPlugin.h>

class PluginAGC: public ProcessorPlugin {
public:
	typedef enum {
		IN_LIN_OUT_LIN = 0,
		IN_LIN_OUT_DB = 1,
		IN_DB_OUT_LIN = 2,
		IN_DB_OUT_DB = 3
	} GAIN_PROCESS_TYPE;

public:
	PluginAGC(string name, uint32_t samprate, uint32_t bufsize);
	virtual ~PluginAGC();
	virtual int do_process();
	virtual bool do_init(const fmsmoov::PluginConfig& cfg);
	virtual fmsmoov::PluginConfigResponse do_change_cfg(const fmsmoov::PluginConfig& cfg);
	void do_set_aux_input_bufs(vector<AudioBuf*>* bufs);
	virtual void finalize_buffer_init();
	float get_gs_avg();
	bool get_gate_active();
private:
	float m_TL;  //target level
	float m_GT;  //gain threshold
	float m_Tatt; //attack time
	float m_Trel; //release time
	float m_Tatt_gate; //gate attack time
	float m_alphaA; //alpha attack
	float m_alphaR; //alpha release
	float m_alpha_gate;  //alpha gate

	float* m_login_L;  //input log amplitudes L
	float* m_login_R;  //input log amplitudes R
	float m_gsprev; //previous gain smoothing value
	float m_gs_avg; //avg gain smooth for live output
	float m_gate_level;
	bool m_gate_active;
	float m_gate_close_reset_thresh; //how much gate close time before return to 0 dB
	uint32_t m_gate_closed_cycles;  //how long the gate has been closed
	float last_gc;

	float gc, gs, idb, idb_acc, gcA, gcB, gsA, gsB;
};

#endif /* PLUGIN_AGC_H_ */
