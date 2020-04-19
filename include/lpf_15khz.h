/*
 * LPF15kHz.h
 *
 *  Created on: Apr 18, 2020
 *      Author: zaremba
 */

#ifndef INCLUDE_lpf_15khz_H_
#define INCLUDE_lpf_15khz_H_

#include <stdint.h>
#include "filter3sos.h"

class LPF15kHz {
public:
	LPF15kHz(uint32_t bufsize);
	virtual ~LPF15kHz();
	//address to array of two float pointers expected for ins and outs
	void process(float* inL, float* inR, float* outL, float* outR, uint32_t samps);

	uint32_t m_bufsize;

	SOS* sos1, *sos2, *sos3;

	Filter3SOS* fL, *fR;

};

#endif /* INCLUDE_lpf_15khz_H_ */
