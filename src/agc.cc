/*
 * agc.cpp
 *
 *  Created on: Jun 10, 2019
 *      Author: zaremba
 *
 *  Wideband AGC with 10ms peak program level detection integrator.
 *
 */

#include <agc.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <unistd.h>
#include "linlog.h"

using namespace std;

AGC::AGC(float _TL, float _GT, float _Tatt, float _Trel)
{
	set(_TL, _GT, _Tatt, _Trel);
}

AGC::~AGC() {
	// TODO Auto-generated destructor stub
	delete[] membufL;
	delete[] membufR;
}

void AGC::set(float _TL, float _GT, float _Tatt, float _Trel)
{
	Tatt = _Tatt;
	Trel = _Trel;

	GT = _GT;  /* Gain threshold */
	TL = _TL;   /* Target level */

	alphaA = expf(-logf(9.0)/(48000.0 * Tatt));
	alphaR = expf(-logf(9.0)/(48000.0 * Trel));

	M = Fs * peak_response_rate; //in samples

	GHT = Fs * gate_hold_time; //in samples
	peak_int_Laccum = 0;
	peak_int_Raccum = 0;

	if(NULL != membufL)
	{
		delete[] membufL;
	}
	if(NULL != membufR)
	{
		delete[] membufR;
	}

	membufL = new float[M];
	membufR = new float[M];

	cout << "M: " << M << endl;

}

void AGC::process(float* inL, float* inR, float* outL, float* outR, uint32_t samps)
{
	/* Peak response rate of 10 ms to conform to EBU Peak Program Meter */
	for(uint32_t i = 0; i < samps; i++)
	{
		if(i < M)
		{
		  peak_int_Laccum = peak_int_Laccum + abs((int32_t)(inL[i] * 32767.0f)) - abs((int32_t)(membufL[i] * 32767.0f));
		  peak_int_Raccum = peak_int_Raccum + abs((int32_t)(inR[i] * 32767.0f)) - abs((int32_t)(membufR[i] * 32767.0f));
		}
		else
		{
		  peak_int_Laccum = peak_int_Laccum + abs((int32_t)(inL[i] * 32767.0f)) - abs((int32_t)(inL[i-M] * 32767.0f));
		  peak_int_Raccum = peak_int_Raccum + abs((int32_t)(inR[i] * 32767.0f)) - abs((int32_t)(inR[i-M] * 32767.0f));
		}

		peak_int_Lf = ((float)peak_int_Laccum / (float)M) / 32767.0f;
		peak_int_Rf = ((float)peak_int_Raccum / (float)M) / 32767.0f;

		piLdb = lin2log(peak_int_Lf);
		piRdb = lin2log(peak_int_Rf);

		/* Take the stronger of the left/right channel to preserve stereo image */

		if(piLdb >= piRdb)
		{
		  pidb = piLdb;
		}

		else
		{
		  pidb = piRdb;
		}

//		cout << "pidb=" << pidb << endl;

		if(pidb < GT)
		{
			inlo++;
		}
		else
		{
			inlo = 0;
		}

		if(inlo > GHT)
		{
		  gc = 1;
		}

		if(0 == inlo)
		{
		  gc = TL - pidb;
		}

		if(gc < gsprev)
		{
		  gs = alphaA*gsprev + (1-alphaA)*gc;
		}
		else if(gc > gsprev)
		{
		  gs = alphaR*gsprev + (1-alphaR)*gc;
		}

		gsprev = gs;

		outL[i] = inL[i] * log2lin(gs);
		outR[i] = inR[i] * log2lin(gs);

	}
	/* Now copy the last M samples into the membuf so they can be used in the next iteration */
	memcpy(membufL, inL+(samps-M), M * sizeof(float));
	memcpy(membufR, inR+(samps-M), M * sizeof(float));

//	cout << "inL[samps]/membufL[M], inR[samps]/membufR[M]: " << inL[samps-1] << "/" << membufL[M-1] <<
//			"/" << inR[samps-1] << "/" << membufR[M-1] << endl;

	cout << "gs=" << gs << endl;
}
