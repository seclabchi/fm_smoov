#ifndef __BIQUAD_H__
#define __BIQUAD_H__

#include <unistd.h>
#include <stdint.h>

class Biquad
{
public:
	Biquad(float _g, float _b0, float _b1, float _b2, float _a0, float _a1, float _a2);
	virtual ~Biquad();
	void process(float* x, uint32_t size);
	void get_power(float* l, float* r);

private:
	float g, b0, b1, b2, a0, a1, a2;
	float wl, wr, wl1, wr1, wl2, wr2;
	uint32_t i;

	float lastpow_l, lastpow_r;
};

#endif
