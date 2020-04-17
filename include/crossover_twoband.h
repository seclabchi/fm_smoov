/*
 * CrossoverTwoband.h
 *
 *  Created on: Aug 18, 2019
 *      Author: zaremba
 */

#ifndef INCLUDE_CROSSOVER_TWOBAND_H_
#define INCLUDE_CROSSOVER_TWOBAND_H_

#include <stdint.h>
#include "filter.h"

class CrossoverTwoband {
public:
	CrossoverTwoband(uint32_t bufsize);
	virtual ~CrossoverTwoband();
	//address to array of two float pointers expected for ins and outs
	void process(float* inL, float* inR, float** outL, float** outR, uint32_t samps);

	uint32_t m_bufsize;

	SOS* b0lo_sos1, *b0lo_sos2;
	SOS* b0hi_sos1, *b0hi_sos2;

	Filter* b0loL, *b0loR, *b0hiL, *b0hiR;

};

#endif /* INCLUDE_CROSSOVER_H_ */
