/*
 * compressor.cpp
 *
 *  Created on: Jun 10, 2019
 *      Author: zaremba
 */

#include "compressor.h"

#include <math.h>
#include <iostream>
#include <stdexcept>

using namespace std;

Compressor::Compressor()
{
	/* Set defaults */
	timeconst_a = .000001;
	timeconst_r = .500;

	T = -50.0;
	R = 1.6;
	W = 2.0;

	//soft knee thresholds
	knee_start = T - (W/2.0);
	knee_end = T + (W/2.0);

	//previous gain smoothing values
	gsLprev = 0.0;
	gsRprev = 0.0;

	//makeup gain
	M = -(T - (T/R)); //dB

	recalculate();
}

Compressor::~Compressor() {
	// TODO Auto-generated destructor stub
}

void Compressor::recalculate()
{
	alpha_a = expf(-logf(9.0)/(48000.0 * timeconst_a));
	alpha_r = expf(-logf(9.0)/(48000.0 * timeconst_r));
}

void Compressor::process(float* p, uint32_t samps)
{
	ibuf = 0;
	lastpow_l = 0;
	lastpow_r = 0;

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
		else
		{
			detL = -99.0;
		}

		if(samp_abs_r != 0.0)
		{
			detR = 20.0*log10f(samp_abs_r);
		}
		else
		{
			detR = -99.0;
		}


		//compute static characteristic
		//left channel
		if(p[i] < knee_start)
		{
			buf_l[ibuf] = p[i];
		}
		else if(p[i] >= knee_start && p[i] <= knee_end)
		{
			buf_l[ibuf] = p[i] + ((1.0/R)-1.0) * powf(p[i] - T + (W/2.0), 2.0) / (2.0*W);
		}
		else
		{
			buf_l[ibuf] = T + ((p[i] - T)/R);
		}

		//right channel
		if(p[i+1] < knee_start)
		{
			buf_r[i] = p[i+1];
		}
		else if(p[i+1] >= knee_start && p[i+1] <= knee_end)
		{
			buf_r[i] = p[i+1] + ((1.0/R)-1.0) * powf(p[i+1] - T + (W/2.0), 2.0) / (2.0*W);
		}
		else
		{
			buf_r[ibuf] = T + ((p[i+1] - T)/R);
		}

		//compute gain
		buf_l[ibuf] = buf_l[ibuf] - detL;
		buf_r[ibuf] = buf_r[ibuf] - detR;

		//compute gain smoothing
		//left channel

		if(buf_l[ibuf] <= gsLprev)
		{
			buf_l[ibuf] = alpha_a * gsLprev + (1.0-alpha_a) * buf_l[ibuf];
			gsLprev = buf_l[ibuf];
		}
		else
		{
			buf_l[ibuf] = alpha_r * gsLprev + (1.0-alpha_r) * buf_l[ibuf];
			gsLprev = buf_l[ibuf];
		}

		//right channel
		if(buf_r[ibuf] <= gsRprev)
		{
			buf_r[ibuf] = alpha_a * gsRprev + (1.0-alpha_a) * buf_r[ibuf];
			gsRprev = buf_r[ibuf];
		}
		else
		{
			buf_r[ibuf] = alpha_r * gsRprev + (1.0-alpha_r) * buf_r[ibuf];
			gsRprev = buf_r[ibuf];
		}

		//apply makeup gain

		buf_l[ibuf] = buf_l[ibuf] + M;
		buf_r[ibuf] = buf_r[ibuf] + M;

		//compute linear gain and apply

		gL = powf(10.0, buf_l[ibuf]/20.0);
		p[i] = gL * p[i];
		gR = powf(10.0, buf_r[ibuf]/20.0);
		p[i+1] = gR * p[i+1];

		lastpow_l += powf(samp_abs_l, 2.0);
		lastpow_r += powf(samp_abs_r, 2.0);

		ibuf++;
	}

	lastpow_l = 2.0 * lastpow_l / samps;
	lastpow_r = 2.0 * lastpow_r / samps;

}

void Compressor::get_last_power(float* l, float* r)
{
	*l = lastpow_l;
	*r = lastpow_r;
}
