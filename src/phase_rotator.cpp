/*
 * phase_rotator.cpp
 *
 *  Created on: May 21, 2020
 *      Author: zaremba
 */

#include <phase_rotator.h>

PhaseRotator::PhaseRotator(uint32_t _bufsize) {
	m_bufsize = _bufsize;
	rot1 = new APF200Hz(m_bufsize);
	rot2 = new APF200Hz(m_bufsize);
	rot3 = new APF200Hz(m_bufsize);
	rot4 = new APF200Hz(m_bufsize);
}

PhaseRotator::~PhaseRotator() {
	delete rot1;
	delete rot2;
	delete rot3;
	delete rot4;
}

void PhaseRotator::process(float* inL, float* inR, float* outL, float* outR, uint32_t samps)
{
	rot1->process(inL, inR, outL, outR, samps);
	rot2->process(outL, outR, outL, outR, samps);
	rot3->process(outL, outR, outL, outR, samps);
	rot4->process(outL, outR, outL, outR, samps);
}
