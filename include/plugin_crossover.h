/*
 * plugin_crossover.h
 *
 *  Created on: Oct 27, 2022
 *      Author: zaremba
 */

#ifndef PLUGIN_CROSSOVER_H_
#define PLUGIN_CROSSOVER_H_

#include <ProcessorPlugin.h>

#include "filter_lr4.h"

class PluginCrossover: public ProcessorPlugin {
public:
	PluginCrossover(const std::string& _name, uint32_t samprate, uint32_t bufsize);
	virtual ~PluginCrossover();
	virtual int do_process();
	virtual fmsmoov::PluginConfigResponse do_change_cfg(const fmsmoov::PluginConfig& cfg);
	virtual bool do_init(const fmsmoov::PluginConfig& cfg);
	virtual void do_set_aux_input_bufs(vector<AudioBuf*>* bufs);
	virtual void finalize_buffer_init();
private:
	SOS* s0lo, *s0hi, *s1lo, *s1hi, *s2lo, *s2hi, *s3lo, *s3hi, *s4lo, *s4hi;
	FilterLR4* f0loL, *f0loR, *f0hiL, *f0hiR, *f1loL, *f1loR, *f1hiL, *f1hiR, *f2loL, *f2loR, *f2hiL, *f2hiR, *f3loL, *f3loR, *f3hiL, *f3hiR, *f4loL, *f4loR, *f4hiL, *f4hiR;
	AllpassFilterLR4* fa0p2L, *fa0p2R, *fa0q1L, *fa0q1R, *fa0q3L, *fa0q3R, *fa0q4L, *fa0q4R, *fa1p4L, *fa1p4R, *fa1q3L, *fa1q3R;

	float* f0lo_outL, *f0lo_outR, *a0p2_outL, *a0p2_outR, *f1lo_outL, *f1lo_outR, *fa1p4_outL, *fa1p4_outR;
	float* f1hi_outL, *f1hi_outR, *fa1q3_outL, *fa1q3_outR;
	float* f0hi_outL, *f0hi_outR, *a0q1_outL, *a0q1_outR, *a0q3_outL, *a0q3_outR, *a0q4_outL, *a0q4_outR;

	float* b0_outL, *b0_outR, *b1_outL, *b1_outR, *b2_outL, *b2_outR, *b3_outL, *b3_outR, *b4_outL, *b4_outR, *b5_outL, *b5_outR;
};

#endif /* PLUGIN_CROSSOVER_H_ */
