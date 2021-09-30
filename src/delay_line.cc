/*
 * delay_line.cc
 *
 *  Created on: Aug 28, 2021
 *      Author: zaremba
 */

#include "delay_line.h"
#include <iostream>
#include <string.h>

using namespace std;

DelayLine::DelayLine(uint32_t _samp_rate, uint32_t _ms_delay, uint32_t _bufsize)
{
	samp_rate = _samp_rate;
	ms_delay = _ms_delay;
	bufsize = _bufsize;
	this->quit();
	this->init(samp_rate, ms_delay, bufsize);
}

DelayLine::~DelayLine()
{
	this->quit();
}

void DelayLine::change_delay(double new_delay)
{
	this->init(this->samp_rate, (uint32_t)(new_delay * 1000), this->bufsize);
}


void DelayLine::quit()
{
    if(NULL != delay_buf_l)
    {
        delete[] delay_buf_l;
        delay_buf_l = NULL;
    }
    if(NULL != delay_buf_r)
    {
        delete[] delay_buf_r;
        delay_buf_r = NULL;
    }
}


void DelayLine::init(uint32_t sampfreq, uint32_t delay_ms, uint32_t bufsize)
{
    //clean up the existing dynamic buffers if needed
    quit();

    //calculate the number of samples the delay line needs
    uint32_t samp_delay = sampfreq * (double)(delay_ms / 1000.0);
    cout << "Delay = " << samp_delay << " samples." << endl;

    delay_buf_size = samp_delay + bufsize;

    cout << "delay_buf_size " << delay_buf_size << endl;
    delay_buf_l = new float[delay_buf_size];
    delay_buf_r = new float[delay_buf_size];
    eob_l = delay_buf_l + delay_buf_size;
    eob_r = delay_buf_r + delay_buf_size;
    cout << "delaybuf/eobL: " << hex << delay_buf_l << ", " << eob_l << dec << endl;
    wPtrL = delay_buf_l + samp_delay;
    wPtrR = delay_buf_r + samp_delay;
    rPtrL = delay_buf_l;
    rPtrR = delay_buf_r;

    memset(delay_buf_l, 0, delay_buf_size * sizeof(float));
    memset(delay_buf_r, 0, delay_buf_size * sizeof(float));
}

void DelayLine::process(float *inL, float *inR, float *outL, float *outR, uint32_t nsamp)
{
    if(wPtrL + nsamp > eob_l)
    {
        uint32_t spaceLeft = eob_l - wPtrL;

        memcpy(wPtrL, inL, spaceLeft * sizeof(float));
        memcpy(wPtrR, inR, spaceLeft * sizeof(float));
        wPtrL = delay_buf_l;
        wPtrR = delay_buf_r;
        memcpy(wPtrL, inL + spaceLeft, (nsamp - spaceLeft) * sizeof(float));
        memcpy(wPtrR, inR + spaceLeft, (nsamp - spaceLeft) * sizeof(float));
        wPtrL += nsamp - spaceLeft;
        wPtrR += nsamp - spaceLeft;
    }
    else
    {
        memcpy(wPtrL, inL, nsamp * sizeof(float));
        memcpy(wPtrR, inR, nsamp * sizeof(float));
        wPtrL += nsamp;
        wPtrR += nsamp;
    }

    if(rPtrL + nsamp > eob_l)
    {
        uint32_t spaceLeft = eob_l - rPtrL;
        memcpy(outL, rPtrL, spaceLeft * sizeof(float));
        memcpy(outR, rPtrR, spaceLeft * sizeof(float));
        rPtrL = delay_buf_l;
        rPtrR = delay_buf_r;
        memcpy(outL + spaceLeft, rPtrL, (nsamp - spaceLeft) * sizeof(float));
        memcpy(outR + spaceLeft, rPtrR, (nsamp - spaceLeft) * sizeof(float));
        rPtrL += nsamp - spaceLeft;
        rPtrR += nsamp - spaceLeft;
    }
    else
    {
        memcpy(outL, rPtrL, nsamp * sizeof(float));
        memcpy(outR, rPtrR, nsamp * sizeof(float));
        rPtrL += nsamp;
        rPtrR += nsamp;
    }
}



