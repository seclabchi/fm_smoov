/*
 * parametric_filter.cpp
 *
 *  Created on: Jul 4, 2022
 *      Author: zaremba
 */

#include "parametric_filter.h"

#include <math.h>

ParametricFilter::ParametricFilter(double _fsamp, uint32_t _bufsize, double _fcut, double _Q,
									double _peak_gain, std::string _type, std::string _name)
									: fsamp(_fsamp), bufsize(_bufsize), fcut(_fcut), Q(_Q),
									  peak_gain(_peak_gain), type(_type), name(_name) {

	bq1 = new ParametricBiquad(fsamp, bufsize, fcut, Q, peak_gain, type);
	bq2 = new ParametricBiquad(fsamp, bufsize, fcut, Q, peak_gain, type);
	tmpbuf = new float[bufsize];
	i = 0;
}

ParametricFilter::ParametricFilter(const ParametricFilter& rhs) : fsamp(rhs.fsamp), bufsize(rhs.bufsize),
										fcut(rhs.fcut), Q(rhs.Q), peak_gain(rhs.peak_gain),
										type(rhs.type), name(rhs.name) {
	bq1 = new ParametricBiquad(fsamp, bufsize, fcut, Q, peak_gain, type);
	bq2 = new ParametricBiquad(fsamp, bufsize, fcut, Q, peak_gain, type);
	i = 0;
}

ParametricFilter& ParametricFilter::operator = (const ParametricFilter& rhs) {
	if(&rhs == this) {
		return *this;
	}

	ParametricFilter tmp(rhs);
	std::swap(tmp, *this);

	return *this;
}

ParametricFilter::~ParametricFilter() {
	delete bq1;
	delete bq2;
	delete[] tmpbuf;
}

void ParametricFilter::process(float* in, float* out) {
	memset(tmpbuf, 0, bufsize * sizeof(float));
	bq1->process(in, tmpbuf);
	bq2->process(tmpbuf, out);
}
