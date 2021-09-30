/*
 * delay_line.h
 *
 *  Created on: Aug 28, 2021
 *      Author: zaremba
 */

#ifndef INCLUDE_DELAY_LINE_H_
#define INCLUDE_DELAY_LINE_H_

#include <cstdlib>
#include <stdint.h>

class DelayLine
{
public:
	DelayLine(uint32_t _samp_rate, uint32_t _ms_delay, uint32_t _bufsize);
	void quit();
	void init(uint32_t sampfreq, uint32_t delay_ms, uint32_t bufsize);
	void change_delay(double new_delay);
	virtual ~DelayLine();
	void process(float* inL, float* inR, float* outL, float* outR, uint32_t nsamp);
private:
	uint32_t samp_rate;
	uint32_t ms_delay;
	uint32_t bufsize;
	float *delay_buf_l = NULL;
	float *delay_buf_r = NULL;
	uint32_t delay_buf_size = 0;
	float *rPtrL = 0;
	float *rPtrR = 0;
	float *wPtrL = 0;
	float *wPtrR = 0;
	float *eob_l = 0;
	float *eob_r = 0;
};



#endif /* INCLUDE_DELAY_LINE_H_ */
