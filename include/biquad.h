#ifndef __BIQUAD_H__
#define __BIQUAD_H__

#include <unistd.h>
#include <stdint.h>

#include "sos.h"

class Biquad
{
public:
	Biquad(float _g, float _b0, float _b1, float _b2, float _a0, float _a1, float _a2);
	Biquad(SOS* _sos);
	virtual ~Biquad();
	void process(float* x, uint32_t samps);

private:
	SOS* sos;
	float g, b0, b1, b2, a0, a1, a2;
	float w, w1, w2;
	uint32_t i;
};

#endif
