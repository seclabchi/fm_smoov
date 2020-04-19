/*
 * HPF30Hz.h
 *
 *  Created on: Apr 18, 2020
 *      Author: zaremba
 */

#ifndef INCLUDE_hpf_30hz_H_
#define INCLUDE_hpf_30hz_H_

#include <stdint.h>
#include "filter.h"

class HPF30Hz {
public:
	HPF30Hz(uint32_t bufsize);
	virtual ~HPF30Hz();
	//address to array of two float pointers expected for ins and outs
	void process(float* inL, float* inR, float* outL, float* outR, uint32_t samps);

	uint32_t m_bufsize;

	SOS* b0hi_sos1, *b0hi_sos2;

	Filter* b0hiL, *b0hiR;

};

#endif /* INCLUDE_hpf_30hz_H_ */
