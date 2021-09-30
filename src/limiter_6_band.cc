/*
 * compressor.cpp
 *
 *  Created on: Jun 10, 2019
 *      Author: zaremba
 */

#include "limiter_6_band.h"

#include <math.h>
#include <iostream>
#include <stdexcept>


#define EPS 0.00000000001  //epsilon, to prevent zero sample errors

using namespace std;

LimiterSettings::LimiterSettings(float _R, float _T, float _G, float _W, float _Tatt, float _Trel) :
	R(_R), T(_T), G(_G), W(_W), Tatt(_Tatt), Trel(_Trel)
{

}

LimiterSettings::~LimiterSettings()
{

}

Limiter6band::Limiter6band(LimiterSettings** settings, uint32_t bufsize)
{
	m_bufsize = bufsize;
	cross = new Crossover6band(m_bufsize);
	comp0 = new Compressor(settings[0]->R, settings[0]->T, settings[0]->G, settings[0]->W, settings[0]->Tatt, settings[0]->Trel);
	comp1 = new Compressor(settings[1]->R, settings[1]->T, settings[1]->G, settings[1]->W, settings[1]->Tatt, settings[1]->Trel);
	comp2 = new Compressor(settings[2]->R, settings[2]->T, settings[2]->G, settings[2]->W, settings[2]->Tatt, settings[2]->Trel);
	comp3 = new Compressor(settings[3]->R, settings[3]->T, settings[3]->G, settings[3]->W, settings[3]->Tatt, settings[3]->Trel);
	comp4 = new Compressor(settings[4]->R, settings[4]->T, settings[4]->G, settings[4]->W, settings[4]->Tatt, settings[4]->Trel);
	comp5 = new Compressor(settings[5]->R, settings[5]->T, settings[5]->G, settings[5]->W, settings[5]->Tatt, settings[5]->Trel);

	b0L = new float[m_bufsize];
	b0R = new float[m_bufsize];
	b1L = new float[m_bufsize];
	b1R = new float[m_bufsize];
	b2L = new float[m_bufsize];
	b2R = new float[m_bufsize];
	b3L = new float[m_bufsize];
	b3R = new float[m_bufsize];
	b4L = new float[m_bufsize];
	b4R = new float[m_bufsize];
	b5L = new float[m_bufsize];
	b5R = new float[m_bufsize];

	b0 = new float*[2];
	b1 = new float*[2];
	b2 = new float*[2];
	b3 = new float*[2];
	b4 = new float*[2];
	b5 = new float*[2];

	b0[0] = b0L;
	b0[1] = b0R;
	b1[0] = b1L;
	b1[1] = b1R;
	b2[0] = b2L;
	b2[1] = b2R;
	b3[0] = b3L;
	b3[1] = b3R;
	b4[0] = b4L;
	b4[1] = b4R;
	b5[0] = b5L;
	b5[1] = b5R;

	be0 = true;
	be1 = true;
	be2 = true;
	be3 = true;
	be4 = true;
	be5 = true;

	cross->band_enable(be0, be1, be2, be3, be4, be5);
}

Limiter6band::~Limiter6band()
{
	delete[] b0L;
	delete[] b0R;
	delete[] b1L;
	delete[] b1R;
	delete[] b2L;
	delete[] b2R;
	delete[] b3L;
	delete[] b3R;
	delete[] b4L;
	delete[] b4R;
	delete[] b5L;
	delete[] b5R;

	delete[] b0;
	delete[] b1;
	delete[] b2;
	delete[] b3;
	delete[] b4;
	delete[] b5;
}

void Limiter6band::band_enable(bool _b0, bool _b1, bool _b2, bool _b3, bool _b4, bool _b5)
{
	be0 = _b0;
	be1 = _b1;
	be2 = _b2;
	be3 = _b3;
	be4 = _b4;
	be5 = _b5;

	cross->band_enable(be0, be1, be2, be3, be4, be5);
}

void Limiter6band::set(LimiterSettings** settings)
{
	comp0->set(settings[0]->R, settings[0]->T, settings[0]->G, settings[0]->W, settings[0]->Tatt, settings[0]->Trel);
	comp1->set(settings[1]->R, settings[1]->T, settings[1]->G, settings[1]->W, settings[1]->Tatt, settings[1]->Trel);
	comp2->set(settings[2]->R, settings[2]->T, settings[2]->G, settings[2]->W, settings[2]->Tatt, settings[2]->Trel);
	comp3->set(settings[3]->R, settings[3]->T, settings[3]->G, settings[3]->W, settings[3]->Tatt, settings[3]->Trel);
	comp4->set(settings[4]->R, settings[4]->T, settings[4]->G, settings[4]->W, settings[4]->Tatt, settings[4]->Trel);
	comp5->set(settings[5]->R, settings[5]->T, settings[5]->G, settings[5]->W, settings[5]->Tatt, settings[5]->Trel);
}

void Limiter6band::process(float* inL, float* inR, float* outL, float* outR, uint32_t samps)
{
	cross->process(inL, inR, b0, b1, b2, b3, b4, b5, samps);

	comp0->process(b0[0], b0[1], b0[0], b0[1], samps);
	comp1->process(b1[0], b1[1], b1[0], b1[1], samps);
	comp2->process(b2[0], b2[1], b2[0], b2[1], samps);
	comp3->process(b3[0], b3[1], b3[0], b3[1], samps);
	comp4->process(b4[0], b4[1], b4[0], b4[1], samps);
	comp5->process(b5[0], b5[1], b5[0], b5[1], samps);

	for(uint32_t i = 0; i < samps; i++)
	{
		outL[i] = b0L[i] + b1L[i] + b2L[i] + b3L[i] + b4L[i] + b5L[i];
		outR[i] = b0R[i] + b1R[i] + b2R[i] + b3R[i] + b4R[i] + b5R[i];
	}

}
