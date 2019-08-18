/*
 * slow_agc.cpp
 *
 *  Created on: Jun 10, 2019
 *      Author: zaremba
 */

#include "slow_agc.h"

#include <math.h>
#include <iostream>
#include <stdexcept>

using namespace std;

SlowAGC::SlowAGC()
{
	/* Set defaults */
	timeconst_a = .5;
	timeconst_r = .5;

	T = -12;  /* Target level */
	M = 60.0;   /* Max gain */

	//previous gain smoothing values
	gsLprev = 0.0;
	gsRprev = 0.0;

	recalculate();
}

SlowAGC::~SlowAGC() {
	// TODO Auto-generated destructor stub
}

void SlowAGC::recalculate()
{
	alpha_a = expf(-logf(9.0)/(48000.0 * timeconst_a));
	alpha_r = expf(-logf(9.0)/(48000.0 * timeconst_r));
}

void SlowAGC::process(float* p, uint32_t samps)
{
	ibuf = 0;
	float gDBl, gDBr;

	for(i = 0; i < samps; i+=2)
	{
		if(samps % 2 != 0)
		{
			throw runtime_error("Samp buffer must be multiple of two.");
		}
		//detect signal power in dB

		samp_abs_l = fabsf(p[i]);
		samp_abs_r = fabsf(p[i+1]);

		if(samp_abs_l != 0.0)
		{
			detL = 20.0*log10f(samp_abs_l);
		}

		if(samp_abs_r != 0.0)
		{
			detR = 20.0*log10f(samp_abs_r);
		}

		//compute gain
		//left channel
		gDBl = T - detL;

		//right channel
		gDBr = T - detR;

		//compute gain smoothing
		//left channel

		if(gDBl <= gsLprev)
		{
			buf_l[ibuf] = alpha_a * gsLprev + (1.0-alpha_r) * gDBl;
			gsLprev = buf_l[ibuf];
		}
		else
		{
			buf_l[ibuf] = alpha_r * gsLprev + (1.0-alpha_a) * gDBl;
			gsLprev = buf_l[ibuf];
		}

		//right channel
		if(gDBr <= gsRprev)
		{
			buf_r[ibuf] = alpha_a * gsRprev + (1.0-alpha_r) * gDBr;
			gsRprev = buf_r[ibuf];
		}
		else
		{
			buf_r[ibuf] = alpha_r * gsRprev + (1.0-alpha_a) * gDBr;
			gsRprev = buf_r[ibuf];
		}

		//compute linear gain and apply

		gL = powf(10.0, buf_l[ibuf]/20.0);
		p[i] = gL * p[i];
		gR = powf(10.0, buf_r[ibuf]/20.0);
		p[i+1] = gR * p[i+1];

		ibuf++;
	}
}
