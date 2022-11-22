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
#include <string>
#include "common_defs.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"


using namespace std;

class Compressor
{
public:
	Compressor(float _R, float _T, float _G, float _W, float _Tatt, float _Trel, uint32_t _bufsize, uint32_t _samprate, string _name);
	virtual ~Compressor();
	void set(float _R, float _T, float _G, float _W, float _Tatt, float _Trel);
	void process(float* inL, float* inR, float* inLevL, float* inLevR, float* agc_adj, float* outL, float* outR, float* comp);
	void get_last_power(float* l, float* r);
	void get_total_comp(float& _comp);

private:
	Compressor();
	std::shared_ptr<spdlog::logger> log;
	float R; //compression ratio
	float T; //threshold in dB
	float G; //fixed gain in dB
	float W; //soft knee width in dB
	float Tatt; //attack time constant in seconds
	float Trel; //release time constant in seconds
	float alphaA; //attack time constant calc
	float alphaR; //release time constant calc
	float M; //makeup gain in dB
	uint32_t m_bufsize, m_samprate;
	string m_name;

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

	float m_total_comp;

	void recalculate();
};

#endif /* __COMPRESSOR_H__ */
