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

Compressor::Compressor(float _R, float _T, float _G, float _W, float _Tatt, float _Trel)
{
	set(_R, _T, _G, _W, _Tatt, _Trel);
}

Compressor::~Compressor() {
	// TODO Auto-generated destructor stub
}

void Compressor::set(float _R, float _T, float _G, float _W, float _Tatt, float _Trel)
{
	/* Set defaults */
	timeconst_a = _Tatt;
	timeconst_r = _Trel;

	T = _T;
	R = _R;
	W = _W;
	G = _G;

	alphaA = expf(-logf(9.0)/(48000.0 * timeconst_a));
	alphaR = expf(-logf(9.0)/(48000.0 * timeconst_r));

	//previous gain smoothing values
	gsLprev = 0.0;
	gsRprev = 0.0;

	//makeup gain
	M = -(T - (T/R)); //dB
}

void Compressor::process(float* inL, float* inR, float* outL, float* outR, uint32_t samps)
{
	ibuf = 0;

	for(i = 0; i < samps; i++)
	{
		inabsL = fabs(inL[i]);
		inabsR = fabs(inR[i+1]);

		if(inabsL > 0.0)
		{
			indBL = 20 * log10f(inabsL);
		}
		else
		{
			indBL = -99.0;
		}

		if(inabsR > 0.0)
		{
			indBR = 20 * log10f(inabsR);
		}
		else
		{
			indBR = -99.0;
		}

		//calculate static characteristic L
		if(indBL < (T - W/2))
		{
		  scL = indBL;
		}
		else if((indBL >= (T-W/2)) && (indBL < (T+W/2)))
		{
		  scL = indBL + ((1/R-1) * powf(indBL - T + W/2, 2)) / (2*W);
		}
		else
		{
		  scL = T + ((indBL - T)/R) - indBL;
		}

		//calculate static characteristic R
		if(indBR < (T - W/2))
		{
		  scR = indBR;
		}
		else if((indBR >= (T-W/2)) && (indBR < (T+W/2)))
		{
		  scR = indBR + ((1/R-1) * powf(indBR - T + W/2, 2)) / (2*W);
		}
		else
		{
		  scR = T + ((indBR - T)/R) - indBR;
		}

		gcL = scL - indBL;
		gcR = scR - indBR;

		//calculate gain smoothing L

		if(gcL <= gsLprev)
		{
		  gsL = alphaA*gsLprev + (1-alphaA)*gcL;
		}
		else
		{
		  gsL = alphaR*gsLprev + (1-alphaR)*gcL;
		}

		gsLprev = gsL;

		//calculate gain smoothing R

		if(gcR <= gsRprev)
		{
		  gsR = alphaA*gsRprev + (1-alphaA)*gcR;
		}
		else
		{
		  gsR = alphaR*gsRprev + (1-alphaR)*gcR;
		}

		gsRprev = gsR;

		outL[i] = inL[i] * powf(10.0, (gsL + M + G)/20.0);
		outR[i] = inR[i] * powf(10.0, (gsR + M + G)/20.0);
	}
}
