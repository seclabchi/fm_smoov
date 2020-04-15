/*
 * Crossover.h
 *
 *  Created on: Aug 18, 2019
 *      Author: zaremba
 */

#ifndef INCLUDE_CROSSOVER_H_
#define INCLUDE_CROSSOVER_H_

#include <stdint.h>
#include "filter.h"

class Crossover {
public:
	Crossover(uint32_t bufsize, float** out);
	virtual ~Crossover();
	void process(float* inbuf, int nsamp);
private:
	void invert(float* in, uint32_t samps, float* out);
	void allpass(float* in, uint32_t samps, Filter* f1, Filter* f2, float* out);

	uint32_t m_bufsize;
	float* b1;
	float* b2;
	float* b3;
	float* b4;
	float* s1lo, *s1hi, *s2lo, *s2hi, *s3hi;
	float* apbuf1, *apbuf2;

	SOS* b1lo_sos1, *b1lo_sos2;
	SOS* b1hi_sos1, *b1hi_sos2;
	SOS* b2lo_sos1, *b2lo_sos2;
	SOS* b2hi_sos1, *b2hi_sos2;
	SOS* b3lo_sos1, *b3lo_sos2;
	SOS* b3hi_sos1, *b3hi_sos2;
	Filter* b1lo, *b1hi;
	Filter* b2lo, *b2hi;
	Filter* b3lo, *b3hi;
	AllpassFilter* b2ap, *b3ap, *b3ap2;
};

#endif /* INCLUDE_CROSSOVER_H_ */
