/*
 * gain.cpp
 *
 *  Created on: May 17, 2020
 *      Author: zaremba
 *
 *  Fixed gain block.
 *
 */

#include <gain.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <unistd.h>
#include "linlog.h"

using namespace std;

Gain::Gain(float _GL, float _GR, string _name)
{
	set(_GL, _GR);
	name = _name;
}

Gain::~Gain() {
	// TODO Auto-generated destructor stub
}

void Gain::set(float _GL, float _GR)
{
	GL = _GL;
	GR = _GR;

	GLlin = log2lin(GL);
	GRlin = log2lin(GR);
}

void Gain::process(float* inL, float* inR, float* outL, float* outR, uint32_t samps)
{
	for(uint32_t i = 0; i < samps; i++)
	{
		outL[i] = GLlin * inL[i];
		outR[i] = GRlin * inR[i];
	}
}
