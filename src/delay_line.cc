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

DelayLine::DelayLine(uint32_t _samp_rate, uint32_t _ms_delay, uint32_t _bufsize) : m_samprate(_samp_rate), m_delay_ms(_ms_delay), m_bufsize(_bufsize) {
	this->quit();
	this->init(m_samprate, m_delay_ms, m_bufsize);
}

DelayLine::~DelayLine()
{
	this->quit();
}

void DelayLine::change_delay(uint32_t new_delay)
{
	this->init(m_samprate, new_delay, m_bufsize);
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

mutex mtx;


void DelayLine::init(uint32_t sampfreq, uint32_t delay_ms, uint32_t bufsize)
{
	unique_lock<mutex> lck(mtx);


    //clean up the existing dynamic buffers if needed
    //quit();
	memset(delay_buf_l, 0, delay_buf_size * sizeof(float));
	memset(delay_buf_r, 0, delay_buf_size * sizeof(float));

    //calculate the number of samples the delay line needs
    samp_delay = m_samprate * (double)(delay_ms / 1000.0);
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
    mtx.unlock();
}

void DelayLine::process(float *inL, float *inR, float *outL, float *outR)
{
	unique_lock<mutex> lck(mtx);

    if(wPtrL + m_bufsize > eob_l)
    {
        uint32_t spaceLeft = eob_l - wPtrL;

        memcpy(wPtrL, inL, spaceLeft * sizeof(float));
        memcpy(wPtrR, inR, spaceLeft * sizeof(float));
        wPtrL = delay_buf_l;
        wPtrR = delay_buf_r;
        memcpy(wPtrL, inL + spaceLeft, (m_bufsize - spaceLeft) * sizeof(float));
        memcpy(wPtrR, inR + spaceLeft, (m_bufsize - spaceLeft) * sizeof(float));
        wPtrL += m_bufsize - spaceLeft;
        wPtrR += m_bufsize - spaceLeft;
    }
    else
    {
        memcpy(wPtrL, inL, m_bufsize * sizeof(float));
        memcpy(wPtrR, inR, m_bufsize * sizeof(float));
        wPtrL += m_bufsize;
        wPtrR += m_bufsize;
    }

    if(rPtrL + m_bufsize > eob_l)
    {
        uint32_t spaceLeft = eob_l - rPtrL;
        memcpy(outL, rPtrL, spaceLeft * sizeof(float));
        memcpy(outR, rPtrR, spaceLeft * sizeof(float));
        rPtrL = delay_buf_l;
        rPtrR = delay_buf_r;
        memcpy(outL + spaceLeft, rPtrL, (m_bufsize - spaceLeft) * sizeof(float));
        memcpy(outR + spaceLeft, rPtrR, (m_bufsize - spaceLeft) * sizeof(float));
        rPtrL += m_bufsize - spaceLeft;
        rPtrR += m_bufsize - spaceLeft;
    }
    else
    {
        memcpy(outL, rPtrL, m_bufsize * sizeof(float));
        memcpy(outR, rPtrR, m_bufsize * sizeof(float));
        rPtrL += m_bufsize;
        rPtrR += m_bufsize;
    }

    mtx.unlock();
}



