/*
 * generic_delay.cpp
 *
 *  Created on: Nov 8, 2020
 *      Author: zaremba
 */

#include "generic_delay.h"
#include <stdexcept>
#include <string.h>

GenericDelay::GenericDelay(uint32_t _samprate, uint32_t _delay_ms)
{
	bufL = NULL;
	bufR = NULL;
	this->set_delay(samprate, _delay_ms);
}

GenericDelay::~GenericDelay()
{

}

void GenericDelay::set_delay(uint32_t _samprate, uint32_t _delay_ms)
{
	samprate = _samprate;
	delay_ms = _delay_ms;
	reset_buffers();
}

void GenericDelay::process(double* inL, double* inR, double* outL, double* outR, uint32_t nsamps)
{
	for(uint32_t i = 0; i < nsamps; i++)
	{
		memcpy(outL, bufL, sampdelay);
		memcpy(outR, bufR, sampdelay);
	}
}

void GenericDelay::reset_buffers()
{
	cout << "GenericDelay: resetting delay to " << delay_ms << " ms." << endl;
	if(NULL != bufL)
	{
		delete[] bufL;
		bufL = NULL;
	}
	if(NULL != bufR)
	{
		delete[] bufR;
		bufR = NULL;
	}
	if(0 < delay_ms)
	{
		cout << "GenericDelay: sample delay is " << sampdelay << endl;
		bufL = new double[sampdelay];
		bufR = new double[sampdelay];
	}
}




