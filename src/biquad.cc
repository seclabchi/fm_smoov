#include "biquad.h"
#include <stdexcept>
#include <iostream>
#include <math.h>

using namespace std;

Biquad::Biquad(float _g, float _b0, float _b1, float _b2, float _a0, float _a1, float _a2) :
				g(_g), b0(_b0), b1(_b1), b2(_b2), a0(_a0), a1(_a1), a2(_a2)
{
	w = 0.0;
	w1 = 0.0;
	w2 = 0.0;

	i = 0;

	//cout << g << " " << b0 << " " << b1 << " " << b2 << " " << a0 << " " << a1 << " " << a2 << endl;
}

Biquad::Biquad(SOS* _sos) : sos(_sos) {
	g = sos->g;
	b0 = sos->b0;
	b1 = sos->b1;
	b2 = sos->b2;
	//a0 = sos->a0;
	a1 = sos->a1;
	a2 = sos->a2;
}

Biquad::~Biquad()
{

}

void Biquad::process(float* x, uint32_t size)
{
	for(i = 0; i < size; i ++)
	{
		w = x[i] - a1*w1 - a2*w2;
		x[i] = g * (b0 * w + b1 * w1 + b2 * w2);
		w2 = w1;
		w1 = w;
	}
}

