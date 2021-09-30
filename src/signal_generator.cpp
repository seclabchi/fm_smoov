/*
 * signal_generator.cpp
 *
 *  Created on: May 21, 2020
 *      Author: zaremba
 */

#include "signal_generator.h"

SignalGenerator::SignalGenerator(uint32_t _bufsize) {
	bufsize = _bufsize;
	tmpbufL = new float[bufsize];
	tmpbufR = new float[bufsize];
}

SignalGenerator::~SignalGenerator() {
	delete[] tmpbufL;
	delete[] tmpbufR;
}

void SignalGenerator::process(float* outL, float* outR, uint32_t samps)
{
	for(uint32_t i = 0; i < samps; i++)
	{
		outL[i] = -1.0 + 2.0 * ((float)rand()/(float)RAND_MAX);
		outR[i] = -1.0 + 2.0 * ((float)rand()/(float)RAND_MAX);
	}
}
