/*
 * stereo_enhancer.h
 *
 *  Created on: Apr 19, 2020
 *      Author: zaremba
 */

#ifndef INCLUDE_STEREO_ENHANCER_H_
#define INCLUDE_STEREO_ENHANCER_H_

#include <stdint.h>

class StereoEnhancer
{
	public:
		StereoEnhancer(float _sep_factor);
		virtual ~StereoEnhancer();
		void set(float _sep_factor);
		void process(float* inL, float* inR, float* outL, float* outR, uint32_t samps);
	private:
		StereoEnhancer();
		float sep_factor;
		float M;
		float S;
};



#endif /* INCLUDE_STEREO_ENHANCER_H_ */
