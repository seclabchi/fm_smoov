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
	PluginGain(uint32_t samprate, uint32_t bufsize);
	virtual ~PluginGain();
	virtual int do_process();
	virtual bool do_init(const std::map<std::string, PluginConfigVal>& config_vals);
	virtual bool do_change_cfg(const std::map<std::string, PluginConfigVal>& config_vals);
private:
	float gain_lin_L;
	float gain_lin_R;
	float gain_db_L;
	float gain_db_R;
};

#endif /* PLUGIN_GAIN_H_ */
