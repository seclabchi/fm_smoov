/*
 * gain.h
 *
 *  Created on: May 17, 2020
 *      Author: zaremba
 */

#ifndef __GAIN_H__
#define __GAIN_H__

#include <stdint.h>
#include <math.h>
#include <string>

using namespace std;

class Gain
{
public:
	Gain(float _GL, float _GR, string _name);
	virtual ~Gain();
	void set(float  _GL, float  _GR);
	void process(float* inL, float* inR, float* outL, float* outR, uint32_t samps);

private:
	float GL, GR, GLlin, GRlin;
    string name;
};

#endif /* __GAIN_H__ */
