#include "biquad.h"
#include <stdexcept>
#include <iostream>
#include <math.h>

using namespace std;

Biquad::Biquad(float _g, float _b0, float _b1, float _b2, float _a0, float _a1, float _a2) :
				g(_g), b0(_b0), b1(_b1), b2(_b2), a0(_a0), a1(_a1), a2(_a2)
{
	wl = 0.0;
	wl1 = 0.0;
	wl2 = 0.0;

	wr = 0.0;
	wr1 = 0.0;
	wr2 = 0.0;

	i = 0;

	cout << g << " " << b0 << " " << b1 << " " << b2 << " " << a0 << " " << a1 << " " << a2 << endl;
}

Biquad::~Biquad()
{

}

void Biquad::process(float* x, uint32_t size)
{
	lastpow_l = 0.0;
	lastpow_r = 0.0;

	if(size % 2 != 0)
	{
		throw runtime_error("Size needs to be a multiple of two.");
	}

	for(i = 0; i < size; i += 2)
	{
		wl = x[i] - a1*wl1 - a2*wl2;
		wr = x[i+1] - a1*wr1 - a2*wr2;
		x[i] = g * (b0 * wl + b1 * wl1 + b2 * wl2);
		x[i+1] = g * (b0 * wr + b1 * wr1 + b2 * wr2);

		lastpow_l += powf(x[i], 2.0);
		lastpow_r += powf(x[i+1], 2.0);

		wl2 = wl1;
		wr2 = wr1;
		wl1 = wl;
		wr1 = wr;
	}

	lastpow_l = 2.0 * lastpow_l / size;
	lastpow_r = 2.0 * lastpow_r / size;
}

void Biquad::get_power(float* l, float* r)
{
	*l = lastpow_l;
	*r = lastpow_r;
}
