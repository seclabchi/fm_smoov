/*
 * compressor.h
 *
 *  Created on: Jun 10, 2019
 *      Author: zaremba
 */

#ifndef __LIMITER_6_BAND_H__
#define __LIMITER_6_BAND_H__

#define BUFSIZE_COMP 384000

#include <stdint.h>
#include <math.h>
#include "crossover_6_band.h"
#include "compressor.h"

class LimiterSettings
{
public:
	LimiterSettings(float _R, float _T, float _G, float _W, float _Tatt, float _Trel);
	virtual ~LimiterSettings();
	float R;
	float T;
	float G;
	float W;
	float Tatt;
	float Trel;
};

class Limiter6band
{
public:
	Limiter6band(LimiterSettings** settings, uint32_t bufsize);
	virtual ~Limiter6band();
	void set(LimiterSettings** settings);
	void band_enable(bool _b0, bool _b1, bool _b2, bool _b3, bool _b4, bool _b5);
	void process(float* inL, float* inR, float* outL, float* outR, uint32_t samps);
	void get_last_power(float* l, float* r);


private:
	uint32_t m_bufsize;
	Limiter6band();
	Crossover6band* cross;
	Compressor* comp0;
	Compressor* comp1;
	Compressor* comp2;
	Compressor* comp3;
	Compressor* comp4;
	Compressor* comp5;

	float* b0L, *b0R, *b1L, *b1R, *b2L, *b2R, *b3L, *b3R, *b4L, *b4R, *b5L, *b5R;
	float** b0, **b1, **b2, **b3, **b4, **b5;

	bool be0, be1, be2, be3, be4, be5;

	void recalculate();
};

#endif /* __LIMITER_6_BAND_H__ */
