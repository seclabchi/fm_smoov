/*
 * compressor.cpp
 *
 *  Created on: Jun 10, 2019
 *      Author: zaremba
 */

#include "compressor.h"

#include <math.h>
#include <iostream>
#include <stdexcept>

#define EPS 0.00000000001  //epsilon, to prevent zero sample errors

using namespace std;

Compressor::Compressor(float _R, float _T, float _G, float _W, float _Tatt, float _Trel, uint32_t _bufsize, uint32_t _samprate, string _name) :
		R(_R), T(_T), G(_G), W(_W), Tatt(_Tatt), Trel(_Trel), m_bufsize(_bufsize), m_samprate(_samprate), m_name(_name) {
	log = spdlog::stdout_color_mt(m_name);
	log->set_pattern("%^[%H%M%S.%e][%s:%#][%n][%l] %v%$");
	log->set_level(spdlog::level::trace);
	LOGT("Compressor {} CTOR", m_name);

	set(R, T, G, W, Tatt, Trel);
	LOGI("New compressor with settings: ratio: {}, threshold: {}, gain: {}, window: {}, attack {}, release {}", R, T, G, W, Tatt, Trel);
}

Compressor::~Compressor() {
	// TODO Auto-generated destructor stub
}

void Compressor::set(float _R, float _T, float _G, float _W, float _Tatt, float _Trel) {
	/* Set defaults */
	Tatt = _Tatt;
	Trel = _Trel;

	T = _T;
	R = _R;
	W = _W;
	G = _G;

	alphaA = expf(-logf(9.0)/(m_samprate * Tatt));
	alphaR = expf(-logf(9.0)/(m_samprate * Trel));

	//previous gain smoothing values
	gsLprev = 0.0;
	gsRprev = 0.0;
	gsPrev = 0.0;

	//makeup gain
	M = -(T - (T/R)); //dB
}

void Compressor::process(float* inL, float* inR, float* inLevL, float* inLevR, float* agc_adj, float* outL, float* outR, float* comp) {
	ibuf = 0;
	m_total_comp = 0;

	float Tadj = 0.0;

	for(i = 0; i < m_bufsize; i++)
	{
		if(inLevL[i] >= inLevR[i]) {
			inLev = inLevL[i];
		}
		else {
			inLev = inLevR[i];
		}

		if(inLev < T) {
			R = 1.0;
		}
		else {
			R = 1.5 + (0.13235 * (inLev - T));
		}

		M = -(T - (T/R));

		sc = T + ((inLev - T)/R);
		gc = sc - inLev;

		if(gc < gsPrev)
		{
		  gs = alphaA*gsPrev + (1-alphaA)*gc;
		}
		else if(gc > gsPrev)
		{
		  gs = alphaR*gsPrev + (1-alphaR)*gc;
		}

		gsPrev = gs;

		comp[i] = gs;
		m_total_comp += gs;

		M = 16.0;

		outL[i] = inL[i] * powf(10.0, (gs + G + M)/20.0);
		outR[i] = inR[i] * powf(10.0, (gs + G + M)/20.0);

/*		//Tadj = T - (1.0 * agc_adj[i]);
		Tadj = T;
		//makeup gain
		M = -(Tadj - (Tadj/R)); //dB


		//calculate static characteristic L
		if(inLevL[i] < Tadj)
		{
		  scL = inLevL[i];
		}
		//else if((inLevL[i] >= (T-W/2)) && (inLevL[i] < (T+W/2)))
		//{
		//  scL = inLevL[i] + ((1/R-1) * powf(inLevL[i] - T + W/2, 2)) / (2*W);
		//}
		else
		{
			if(inLevL[i] <= Tadj) {
				R = 1.0;
			}
			else {
				//R = 1.8;
				R = 1.5 + (0.13235 * (inLevL[i] - Tadj));
			}
			scL = Tadj + ((inLevL[i] - Tadj)/R);
		}

		//calculate static characteristic R
		if(inLevR[i] < Tadj)
		{
		  scR = inLevR[i];
		}
		//else if((inLevR[i] >= (T-W/2)) && (inLevR[i] < (T+W/2)))
		//{
		//  scR = inLevR[i] + ((1/R-1) * powf(inLevR[i] - T + W/2, 2)) / (2*W);
	    //}
		else
		{
			if(inLevR[i] <= Tadj) {
				R = 1.0;
			}
			else {
				//R = 1.8;
				R = 1.5 + (0.13235 * (inLevR[i] - Tadj));
			}
			scR = Tadj + ((inLevR[i] - Tadj)/R);
		}

		if(inLevL[i] > inLevR[i]) {
			gcL = scL - inLevL[i];
			gcR = gcL;
		}
		else {
			gcR = scR - inLevR[i];
			gcL = gcR;
		}

		//calculate gain smoothing L

		if(gcL < gsLprev)
		{
		  gsL = alphaA*gsLprev + (1-alphaA)*gcL;
		}
		else if(gcL > gsLprev)
		{
		  gsL = alphaR*gsLprev + (1-alphaR)*gcL;
		}

		gsLprev = gsL;

		//calculate gain smoothing R

		if(gcR < gsRprev)
		{
		  gsR = alphaA*gsRprev + (1-alphaA)*gcR;
		}
		else if(gcR > gsRprev)
		{
		  gsR = alphaR*gsRprev + (1-alphaR)*gcR;
		}

		gsRprev = gsR;

		comp[i] = gsL;  //just use left because gsL and gsR will always be equal
		m_total_comp += gsL;

		outL[i] = inL[i] * powf(10.0, (gsL + G + M)/20.0);
		outR[i] = inR[i] * powf(10.0, (gsR + G + M)/20.0);
		*/
	}

	m_total_comp = m_total_comp / m_bufsize;
}

void Compressor::get_total_comp(float& _comp) {
	_comp = m_total_comp;
}
