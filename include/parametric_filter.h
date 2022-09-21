/*
 * parametric_filter.h
 *
 *  Created on: Jul 4, 2022
 *      Author: zaremba
 */

#ifndef PARAMETRIC_FILTER_H_
#define PARAMETRIC_FILTER_H_

#include <string>
#include <vector>

#include "spdlog/spdlog.h"
#include "audiobuf.h"
#include "parametric_biquad.h"

//this filter class will chain two biquads together for a rolloff of 12dB/octave
//TODO: add capability for adjustable rolloff - chained biquads
class ParametricFilter {
public:
	ParametricFilter(double _fsamp, uint32_t bufsize, double _fcut, double _Q, double _peak_gain, std::string _type, std::string _name);
	ParametricFilter(const ParametricFilter& rhs);
	ParametricFilter& operator = (const ParametricFilter& rhs);
	virtual ~ParametricFilter();
	void process(float* in, float* out); //caller better be sure these are valid buffers!
private:
	double fsamp;
	uint32_t bufsize;
	uint32_t i;  //process loop index
	double fcut;
	double Q;
	double peak_gain;
	std::string type;
	std::string name;
	ParametricBiquad* bq1;
	ParametricBiquad* bq2;
	float* tmpbuf;
};

#endif /* PARAMETRIC_FILTER_H_ */
