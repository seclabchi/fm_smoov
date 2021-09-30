/*
 * signal_generator.h
 *
 *  Created on: May 21, 2020
 *      Author: zaremba
 */

#ifndef SIGNAL_GENERATOR_H_
#define SIGNAL_GENERATOR_H_

#include <stdlib.h>
#include <stdint.h>


class SignalGenerator {
public:
	SignalGenerator(uint32_t _bufsize);
	virtual ~SignalGenerator();
	void process(float* outL, float* outR, uint32_t samps);
private:
	uint32_t bufsize;
	float* tmpbufL, *tmpbufR;
};

#endif /* SIGNAL_GENERATOR_H_ */
