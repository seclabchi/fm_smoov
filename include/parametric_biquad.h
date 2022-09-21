/*
 * parametric_biquad.h
 *
 *  Created on: Jul 4, 2022
 *      Author: zaremba
 */

#ifndef PARAMETRIC_BIQUAD_H_
#define PARAMETRIC_BIQUAD_H_

#include <string>

#include "spdlog/spdlog.h"

class ParametricBiquad {
public:
	ParametricBiquad(double _fsamp, uint32_t _bufsize, double _fcut, double _Q, double _peak_gain, std::string _type);
	ParametricBiquad(const ParametricBiquad& rhs);
	ParametricBiquad& operator = (const ParametricBiquad& rhs);
	virtual ~ParametricBiquad();
	void process(float* in, float* out);
private:
	void calculate_coeffs();
	double fsamp;
	uint32_t bufsize;
	double fcut;
	double Q;
	double peak_gain;
	std::string type;
	double s1, s2;
	double y1, y2;
	double x0, x1, x2;
	double b0, b1, b2, a0, a1, a2;
};

#endif /* PARAMETRIC_BIQUAD_H_ */
