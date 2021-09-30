/*
 * agc_2_band.cpp
 *
 *  Created on: Jun 13, 2020
 *      Author: zaremba
 *
 *  2-band AGC with 10ms peak program level detection integrator.
 *
 */

#include "agc_2_band.h"
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <unistd.h>
#include "linlog.h"
#include "agc.h"

using namespace std;

AGC2band::AGC2band(AGCParams** agc_params, uint32_t buflen)
{
	m_buflen = buflen;
	cross = new CrossoverTwoband(m_buflen);
	agcLo = new AGC(agc_params[0]->TL, agc_params[0]->GT, agc_params[0]->Tatt, agc_params[0]->Trel, "2bagcLo");
	agcHi = new AGC(agc_params[1]->TL, agc_params[1]->GT, agc_params[1]->Tatt, agc_params[1]->Trel, "2bagcHi");

	loL = new float[m_buflen];
	loR = new float[m_buflen];
	hiL = new float[m_buflen];
	hiR = new float[m_buflen];

	agcLoL = new float[m_buflen];
	agcLoR = new float[m_buflen];
	agcHiL = new float[m_buflen];
	agcHiR = new float[m_buflen];

	lo = new float*[2];
	hi = new float*[2];

	lo[0] = loL;
	lo[1] = loR;
	hi[0] = hiL;
	hi[1] = hiR;

	agcLo->set(agc_params[0]->TL, agc_params[0]->GT, agc_params[0]->Tatt, agc_params[0]->Trel);
	agcHi->set(agc_params[1]->TL, agc_params[1]->GT, agc_params[1]->Tatt, agc_params[1]->Trel);
}

AGC2band::~AGC2band() {
	delete agcLo;
	delete agcHi;
	delete[] loL;
	delete[] loR;
	delete[] hiL;
	delete[] hiR;
	delete[] lo;
	delete[] hi;
	delete[] agcLoL;
	delete[] agcLoR;
	delete[] agcHiL;
	delete[] agcHiR;
}

void AGC2band::set(AGCParams** agc_params)
{
	agcLo->set(agc_params[0]->TL, agc_params[0]->GT, agc_params[0]->Tatt, agc_params[0]->Trel);
	agcHi->set(agc_params[1]->TL, agc_params[1]->GT, agc_params[1]->Tatt, agc_params[1]->Trel);
}

void AGC2band::process(float* inL, float* inR, float* outL, float* outR, uint32_t samps)
{
	cross->process(inL, inR, lo, hi, samps);

	agcLo->process(lo[0], lo[1], agcLoL, agcLoR, samps);
	agcHi->process(hi[0], hi[1], agcHiL, agcHiR, samps);

	for(uint32_t i = 0; i < samps; i++)
	{
		outL[i] = agcLoL[i] + agcHiL[i];
		outR[i] = agcLoR[i] + agcHiR[i];
	}
}
