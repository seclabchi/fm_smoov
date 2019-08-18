/*
 * slow_agc.h
 *
 *  Created on: Jul 28, 2019
 *      Author: zaremba
 */

#ifndef __SLOW_AGC_H__
#define __SLOW_AGC_H__

#define BUFSIZE_COMP 384000

#include <stdint.h>
#include <math.h>

class SlowAGC
{
public:
	SlowAGC();
	virtual ~SlowAGC();
	void process(float* p, uint32_t samps);

private:
	float timeconst_a; //attack time constant in seconds
	float timeconst_r; //release time constant in seconds
	float alpha_a; //attack time constant calc
	float alpha_r; //release time constant calc
	float T; //target in dB
	float M; //max gain in dB
	float rms_accum;

	float detL, detR;

	float gsLprev, gsRprev;
	float gL, gR;

	uint32_t i;

	float buf_l[BUFSIZE_COMP];
	float buf_r[BUFSIZE_COMP];
	uint32_t ibuf;

	float samp_abs_l, samp_abs_r;

	void recalculate();
};

#endif /* __SLOW_AGC_H__ */
