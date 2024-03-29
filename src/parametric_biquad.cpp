/*
 * parametric_biquad.cpp
 *
 *  Created on: Jul 4, 2022
 *      Author: zaremba
 */

#include "parametric_biquad.h"
#include <iostream>
#include <math.h>

using namespace std;

ParametricBiquad::ParametricBiquad(double _fsamp, uint32_t _bufsize, double _fcut, double _Q,
									double _peak_gain, std::string _type)
									: fsamp(_fsamp), bufsize(_bufsize), fcut(_fcut), Q(_Q),
									  peak_gain(_peak_gain), type(_type) {
	this->calculate_coeffs();
}

ParametricBiquad::ParametricBiquad(const ParametricBiquad& rhs) : fsamp(rhs.fsamp),
										fcut(rhs.fcut), Q(rhs.Q), peak_gain(rhs.peak_gain),
										type(rhs.type) {
	this->calculate_coeffs();
}

ParametricBiquad& ParametricBiquad::operator = (const ParametricBiquad& rhs) {
	if(&rhs == this) {
		return *this;
	}

	ParametricBiquad tmp(rhs);
	std::swap(tmp, *this);

	return *this;
}

ParametricBiquad::~ParametricBiquad() {
	// TODO Auto-generated destructor stub
}

void ParametricBiquad::calculate_coeffs() {
    double V = powf(10, (fabs(peak_gain) / 20.0));
    double K = tan(M_PI * (fcut/fsamp));
    double norm = 0.0;
    double root2 = 1/Q;
    double V0 = powf(10, (peak_gain/20));

	//Invert gain if a cut
	if(V0 < 1.0f) {
		V0 = 1.0f/V0;
	}

    if(!type.compare("lowpass")) {
        norm = 1 / (1 + K / Q + K * K);
        b0 = K * K * norm;
        b1 = 2 * b0;
        b2 = b0;
        a1 = 2 * (K * K - 1) * norm;
        a2 = (1 - K / Q + K * K) * norm;
    }
    else if(!type.compare("highpass")) {
        norm = 1 / (1 + K / Q + K * K);
        b0 = 1 * norm;
        b1 = -2 * b0;
        b2 = b0;
        a1 = 2 * (K * K - 1) * norm;
        a2 = (1 - K / Q + K * K) * norm;
    }
    else if(!type.compare("allpass")) {
        norm = 1 / (1 + K * (1/Q) + K * K);
        b0 = (1 - K * (1/Q) + K * K) * norm;
        b1 = 2 * (K * K - 1) * norm;
        b2 = 1;
        a1 = b1;
        a2 = b0;
    }
    else if(!type.compare("bassshelf")) {
    	if(peak_gain > 0) {
    		b0 = (1 + sqrt(V0)*root2*K + V0*powf(K,2)) / (1 + root2*K + powf(K,2));
			b1 = (2 * (V0*powf(K,2) - 1) ) / (1 + root2*K + powf(K,2));
			b2 = (1 - sqrt(V0)*root2*K + V0*powf(K,2)) / (1 + root2*K + powf(K,2));
			a1 = (2 * (powf(K,2) - 1) ) / (1 + root2*K + powf(K,2));
			a2 = (1 - root2*K + powf(K,2)) / (1 + root2*K + powf(K,2));
    	}
    	else {
    	    b0 = (1 + root2*K + powf(K,2)) / (1 + root2*sqrt(V0)*K + V0*powf(K,2));
    	    b1 = (2 * (powf(K,2) - 1) ) / (1 + root2*sqrt(V0)*K + V0*powf(K,2));
    	    b2 = (1 - root2*K + powf(K,2)) / (1 + root2*sqrt(V0)*K + V0*powf(K,2));
    	    a1 = (2 * (V0*powf(K,2) - 1) ) / (1 + root2*sqrt(V0)*K + V0*powf(K,2));
    	    a2 = (1 - root2*sqrt(V0)*K + V0*powf(K,2)) / (1 + root2*sqrt(V0)*K + V0*powf(K,2));
    	}
    }
    else if(!type.compare("trebleshelf")) {
    	if(peak_gain > 0) {
    		b0 = (V0 + root2*sqrt(V0)*K + powf(K,2)) / (1 + root2*K + powf(K,2));
			b1 = (2 * (powf(K,2) - V0) ) / (1 + root2*K + powf(K,2));
			b2 = (V0 - root2*sqrt(V0)*K + powf(K,2)) / (1 + root2*K + powf(K,2));
			a1 = (2 * (powf(K,2) - 1) ) / (1 + root2*K + powf(K,2));
			a2 = (1 - root2*K + powf(K,2)) / (1 + root2*K + powf(K,2));
    	}
    	else {
    	    b0 = (1 + root2*K + powf(K,2)) / (V0 + root2*sqrt(V0)*K + powf(K,2));
    	    b1 = (2 * (powf(K,2) - 1) ) / (V0 + root2*sqrt(V0)*K + powf(K,2));
    	    b2 = (1 - root2*K + powf(K,2)) / (V0 + root2*sqrt(V0)*K + powf(K,2));
    	    a1 = (2 * ((powf(K,2))/V0 - 1) ) / (1 + root2/sqrt(V0)*K + (powf(K,2))/V0);
    	    a2 = (1 - root2/sqrt(V0)*K + (powf(K,2))/V0) / (1 + root2/sqrt(V0)*K + (powf(K,2))/V0);
    	}
    }
    else {
        //TODO: what to do here
        b0 = 0;
        b1 = 0;
        b2 = 0;
        a0 = 1;
        a1 = 1;
        a2 = 1;
    }

	s1 = 0;
	s2 = 0;
	x2 = 0;
	x1 = 0;
	x0 = 0;
	y2 = 0;
	y1 = 0;

	a0 = 1.0;

	cout << "parametric_biquad coeffs: " << b0 << "," << b1 << "," << b2 << "," << a0 << "," << a1 << "," << a2 << endl;

}

void ParametricBiquad::process(float* in, float* out) {
   for(uint32_t i = 0; i < bufsize; i++)
	{
		x0 = in[i];
		s2 = b2 * x2 - a2 * y2;
		s1 = b1 * x1 - a1 * y1 + s2;
		out[i] = b0 * x0 + s1;
		y2 = y1;
		y1 = out[i];
		x2 = x1;
		x1 = x0;
	}
}
