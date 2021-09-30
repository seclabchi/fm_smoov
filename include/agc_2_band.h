/*
 * agc_2_band.h
 *
 *  Created on: Jun 13, 2020
 *      Author: zaremba
 */

#ifndef __AGC_2_BAND_H__
#define __AGC_2_BAND_H__

#include <stdint.h>
#include <math.h>
#include <string>
#include "agc.h"
#include "crossover_twoband.h"


using namespace std;

class AGC2band
{
public:
	AGC2band(AGCParams** agc_params, uint32_t buflen);
	virtual ~AGC2band();
	void set(AGCParams** agc_params);
	void process(float* inL, float* inR, float* outL, float* outR, uint32_t samps);

private:
	uint32_t m_buflen;
	CrossoverTwoband* cross;
	AGC* agcLo;
	AGC* agcHi;
	float* loL, *loR, *hiL, *hiR;
	float** lo, **hi;
	float* agcLoL, *agcLoR, *agcHiL, *agcHiR;
};

#endif /* __AGC_2_BAND_H__ */
