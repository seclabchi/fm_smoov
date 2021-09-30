/*
 * apf_200hz.h
 *
 *  Created on: May 21, 2020
 *      Author: zaremba
 */

#ifndef INCLUDE_apf_200hz_H_
#define INCLUDE_apf_200hz_H_

#include <stdint.h>
#include "filter.h"

class APF200Hz {
public:
	APF200Hz(uint32_t bufsize);
	virtual ~APF200Hz();
	//address to array of two float pointers expected for ins and outs
	void process(float* inL, float* inR, float* outL, float* outR, uint32_t samps);

	uint32_t m_bufsize;

	float* tmpbufLlo, *tmpbufRlo, *tmpbufLhi, *tmpbufRhi;

	SOS* b0hi_sos1, *b0hi_sos2, *b0lo_sos1, *b0lo_sos2;

	Filter* b0hiL, *b0hiR, *b0loL, *b0loR;

};

#endif /* INCLUDE_apf_200hz_H_ */
