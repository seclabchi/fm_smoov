/*
 * plugin_compressor_limiter.h
 *
 *  Created on: Oct 23, 2022
 *      Author: zaremba
 */

#ifndef PLUGIN_COMPRESSOR_LIMITER_H_
#define PLUGIN_COMPRESSOR_LIMITER_H_

#include "ProcessorPlugin.h"
#include "compressor.h"

class PluginCompressorLimiter: public ProcessorPlugin {
public:
	PluginCompressorLimiter(const std::string& _name, uint32_t samprate, uint32_t bufsize);
	virtual ~PluginCompressorLimiter();
	virtual int do_process();
	virtual fmsmoov::PluginConfigResponse do_change_cfg(const fmsmoov::PluginConfig& cfg);
	virtual bool do_init(const fmsmoov::PluginConfig& cfg);
	virtual void do_set_aux_input_bufs(vector<AudioBuf*>* bufs);
	virtual void finalize_buffer_init();
	void get_comp_levels(float& complev_b0, float& complev_b1, float& complev_b2, float& complev_b3, float& complev_b4, float& complev_b5);
private:
	Compressor* comp_b0, *comp_b1, *comp_b2, *comp_b3, *comp_b4, *comp_b5;

	float* b0_inL, *b0_inR, *b1_inL, *b1_inR, *b2_inL, *b2_inR, *b3_inL, *b3_inR, *b4_inL, *b4_inR, *b5_inL, *b5_inR;
	float* b0_outL, *b0_outR, *b1_outL, *b1_outR, *b2_outL, *b2_outR, *b3_outL, *b3_outR, *b4_outL, *b4_outR, *b5_outL, *b5_outR;
	float* b0_levL, *b0_levR, *b1_levL, *b1_levR, *b2_levL, *b2_levR, *b3_levL, *b3_levR, *b4_levL, *b4_levR, *b5_levL, *b5_levR;

	float* b0_comp, *b1_comp, *b2_comp, *b3_comp, *b4_comp, *b5_comp;

	float* front_agcL, *front_agcR;

	float b0R, b1R, b2R, b3R, b4R, b5R;
	float b0T, b1T, b2T, b3T, b4T, b5T;
	float b0G, b1G, b2G, b3G, b4G, b5G;
	float b0W, b1W, b2W, b3W, b4W, b5W;
	float b0att, b1att, b2att, b3att, b4att, b5att;
	float b0rel, b1rel, b2rel, b3rel, b4rel, b5rel;
};

#endif /* PLUGIN_COMPRESSOR_LIMITER_H_ */
