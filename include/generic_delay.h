#ifndef __GENERIC_DELAY_H__
#define __GENERIC_DELAY_H__

#include <iostream>

using namespace std;

class GenericDelay
{
public:
	GenericDelay(uint32_t _samprate, uint32_t _delay_ms);
	virtual ~GenericDelay();
	void set_delay(uint32_t samprate, uint32_t _delay_ms);
	void process(double* inL, double* inR, double* outL, double* outR, uint32_t nsamps);
private:
	void reset_buffers();
	uint32_t samprate;
	uint32_t delay_ms;
	uint32_t sampdelay;
	double *bufL, *bufR;
};

#endif
