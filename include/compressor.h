/*
 * compressor.h
 *
 *  Created on: Jun 10, 2019
 *      Author: zaremba
 */

#ifndef __COMPRESSOR_H__
#define __COMPRESSOR_H__

#define BUFSIZE_COMP 384000

#include <stdint.h>
#include <math.h>

class Compressor
{
public:
	Compressor(float _R, float _T, float _G, float _W, float _Tatt, float _Trel);
	virtual ~Compressor();
	void set(float _R, float _T, float _G, float _W, float _Tatt, float _Trel);
	void process(float* inL, float* inR, float* outL, float* outR, uint32_t samps);
	void get_last_power(float* l, float* r);


private:
	Compressor();
	float timeconst_a; //attack time constant in seconds
	float timeconst_r; //release time constant in seconds
	float alphaA; //attack time constant calc
	float alphaR; //release time constant calc
	float T; //threshold in dB
	float R; //compression ratio
	float W; //soft knee width in dB
	float M; //makeup gain in dB
	float G; //fixed gain in dB

	float detL, detR;

	float inabsL, inabsR;
	float indBL, indBR;
	float scL, scR;
	float gcL, gcR;
	float gsL, gsR;
	float gsLprev, gsRprev;
	float gL, gR;

	uint32_t i;

	float buf_l[BUFSIZE_COMP];
	float buf_r[BUFSIZE_COMP];
	uint32_t ibuf;

	float lastpow_l, lastpow_r;
	float samp_abs_l, samp_abs_r;

	void recalculate();
};

#endif /* __COMPRESSOR_H__ */
