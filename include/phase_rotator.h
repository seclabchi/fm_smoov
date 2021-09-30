/*
 * phase_rotator.h
 *
 *  Created on: May 21, 2020
 *      Author: zaremba
 */

#ifndef INCLUDE_PHASE_ROTATOR_H_
#define INCLUDE_PHASE_ROTATOR_H_

#include "apf_200hz.h"

class PhaseRotator {
public:
	PhaseRotator(uint32_t _bufsize);
	virtual ~PhaseRotator();
	void process(float* inL, float* inR, float* outL, float* outR, uint32_t samps);
private:
	uint32_t m_bufsize;
	APF200Hz* rot1, *rot2, *rot3, *rot4;
};

#endif /* INCLUDE_PHASE_ROTATOR_H_ */
