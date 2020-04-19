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
	Crossover(uint32_t bufsize);
	virtual ~Crossover();
	void process(float* inL, float* inR, float** outb1, float** outb2, float** outb3, float** outb4, int samps);

private:
	void invert(float* in, uint32_t samps, float* out);
	void allpass(float* in, uint32_t samps, Filter* f1, Filter* f2, float* out);

	uint32_t m_bufsize;
	float* s1loL, *s1loR, *s1hiL, *s1hiR, *s2loL, *s2loR, *s2hiL, *s2hiR, *s3hiL, *s3hiR;
	float* apbuf1, *apbuf2;

	SOS* b1lo_sos1, *b1lo_sos2;
	SOS* b1hi_sos1, *b1hi_sos2;
	SOS* b2lo_sos1, *b2lo_sos2;
	SOS* b2hi_sos1, *b2hi_sos2;
	SOS* b3lo_sos1, *b3lo_sos2;
	SOS* b3hi_sos1, *b3hi_sos2;
	Filter* b1loL, *b1loR, *b1hiL, *b1hiR;
	Filter* b2loL, *b2loR, *b2hiL, *b2hiR;
	Filter* b3loL, *b3loR, *b3hiL, *b3hiR;
	AllpassFilter* b2apL, *b2apR, *b3apL, *b3apR, *b3ap2L, *b3ap2R;
};

#endif /* INCLUDE_CROSSOVER_H_ */
