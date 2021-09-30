/*
 * stereo_enhancer.cpp
 *
 *  Created on: Apr 19, 2020
 *      Author: zaremba
 */

#include "stereo_enhancer.h"

StereoEnhancer::StereoEnhancer(float _sep_factor)
{
	this->set(_sep_factor);
}

StereoEnhancer::~StereoEnhancer()
{

}

void StereoEnhancer::set(float _sep_factor)
{
	sep_factor = _sep_factor;
}

void StereoEnhancer::process(float* inL, float* inR, float* outL, float* outR, uint32_t samps)
{
	for(uint32_t i = 0; i < samps; i++)
	{
		M = sep_factor * ((inL[i] + inR[i]) / 2.0);
		S = sep_factor * ((inL[i] - inR[i]) / 2.0);

		outL[i] = 1.0f/sep_factor * (M + S);
		outR[i] = 1.0f/sep_factor * (M - S);
	}
}


