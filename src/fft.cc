/*
 * fft.cpp
 *
 *  Created on: May 3, 2019
 *      Author: zaremba
 *
 *  Fast fourier transform in real time for modulation monitor
 *
 */

#include "fft.h"
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <unistd.h>
#include "linlog.h"

using namespace std;

FFT::FFT()
{
	cfg = kiss_fft_alloc( 1024, 0, NULL, NULL );
	cx_in = new kiss_fft_cpx[1024];
	cx_out = new kiss_fft_cpx[1024];
	scale_factor = 1/sqrt(1024);
}

FFT::~FFT()
{
	free(cfg);
	delete[] cx_in;
	delete[] cx_out;
}

void FFT::process(float* inL, float* inR, float* outL, float* outR, uint32_t samps)
{
	for (uint32_t i = 0; i < samps; i++)
	{
		cx_in[i].r = inL[i];
	}

    kiss_fft( cfg , cx_in , cx_out );

    for(uint32_t i = 512; i < samps; i++)
    {
    	outL[i-512] = sqrtf(powf(cx_out[i].r, 2) + powf(cx_out[i].i, 2)) * scale_factor;
    }
}
