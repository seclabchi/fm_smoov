/*
 * Crossover.h
 *
 *  Created on: Aug 18, 2019
 *      Author: zaremba
 */

#ifndef INCLUDE_CROSSOVER_6_BAND_H_
#define INCLUDE_CROSSOVER_6_BAND_H_

#include <stdint.h>
#include "filter.h"

class Crossover6band {
public:
	Crossover6band(uint32_t bufsize);
	virtual ~Crossover6band();
	void process(float* inL, float* inR, float** outb1, float** outb2, float** outb3,
			     float** outb4, float** outb5, float** outb6, int samps);
	void band_enable(bool _b0, bool _b1, bool _b2, bool _b3, bool _b4, bool _b5);

private:

	uint32_t m_bufsize;
	float* band0outL, *band0outR, *band1outL, *band1outR, *band2outL, *band2outR;
	float* band3outL, *band3outR, *band4outL, *band4outR, *band5outL, *band5outR;
	float* s0loL, *s0loR, *s0hiL, *s0hiR, *s1loL, *s1loR, *s1hiL, *s1hiR;

	bool be0, be1, be2, be3, be4, be5;

	SOS* b0lo_sos1, *b0lo_sos2;
	SOS* b0hi_sos1, *b0hi_sos2;
	SOS* b1lo_sos1, *b1lo_sos2;
	SOS* b1hi_sos1, *b1hi_sos2;
	SOS* b2lo_sos1, *b2lo_sos2;
	SOS* b2hi_sos1, *b2hi_sos2;
	SOS* b3lo_sos1, *b3lo_sos2;
	SOS* b3hi_sos1, *b3hi_sos2;
	SOS* b4lo_sos1, *b4lo_sos2;
	SOS* b4hi_sos1, *b4hi_sos2;

	Filter* f0loL, *f0loR, *f0hiL, *f0hiR;
	Filter* f1loL, *f1loR, *f1hiL, *f1hiR;
	Filter* f2loL, *f2loR, *f2hiL, *f2hiR;
	Filter* f3loL, *f3loR, *f3hiL, *f3hiR;
	Filter* f4loL, *f4loR, *f4hiL, *f4hiR;

	AllpassFilter* ap2L, *ap2R, *ap1Lp, *ap1Rp, *ap3L, *ap3Lp, *ap3R, *ap3Rp, *ap4L, *ap4Lp, *ap4R, *ap4Rp;
};

#endif /* INCLUDE_CROSSOVER_6_BAND_H_ */
