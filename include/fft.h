/*
 * fft.h
 *
 *  Created on: May 3, 2020
 *      Author: zaremba
 */

#ifndef __FFT_H__
#define __FFT_H__

#include <stdint.h>
#include <math.h>
#include <string>
#include "kiss_fft.h"

using namespace std;

class FFT
{
public:
	FFT();
	virtual ~FFT();
	void process(float* inL, float* inR, float* outL, float* outR, uint32_t samps);

private:
	kiss_fft_cfg cfg;
	kiss_fft_cpx* cx_in, *cx_out;
	float scale_factor;
};

#endif /* __FFT_H__ */
