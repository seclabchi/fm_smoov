/*
 * agc.h
 *
 *  Created on: Jul 28, 2019
 *      Author: zaremba
 */

#ifndef __AGC_H__
#define __AGC_H__

#include <stdint.h>
#include <math.h>
#include <string>

using namespace std;

class AGC
{
public:
	AGC(float _TL, float _GT, float _Tatt, float _Trel, string _name);
	virtual ~AGC();
	void set(float _TL, float _GT, float _Tatt, float _Trel);
	void process(float* inL, float* inR, float* outL, float* outR, uint32_t samps);

private:
	float Tatt; //attack time constant in seconds
	float Trel; //release time constant in seconds
	float alphaA; //attack time constant calc
	float alphaR; //release time constant calc
	float TL; //target in dB
	float GT; //gain threshold for input (hold below threshold)
	static const uint32_t Fs = 48000; //sampling frequency
	static constexpr float peak_response_rate = .010; //10 ms
	static constexpr float gate_hold_time = 8; //5 sec.
    uint32_t M;
    uint32_t GHT;
    uint32_t peak_int_Laccum, peak_int_Raccum;
    uint32_t peak_int_L, peak_int_R;
    float peak_int_Lf, peak_int_Rf;
    float piLdb, piRdb, pidb;
    float gc, gs, gsprev;
    uint32_t inlo;
    uint32_t* prr_bufL, *prr_bufR;
    float* membufL, *membufR;
    string name;
};

#endif /* __AGC_H__ */
