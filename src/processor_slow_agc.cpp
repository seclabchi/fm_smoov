#include "processor_slow_agc.h"

#include <cstdlib>
#include <iostream>
#include <math.h>

using namespace std;

ProcessorSlowAGC::ProcessorSlowAGC()
{
    m_target = 10000;
}

ProcessorSlowAGC::~ProcessorSlowAGC()
{
    
}

void ProcessorSlowAGC::set_target_gain(int16_t target)
{
    m_target = target;
}

void ProcessorSlowAGC::process(void** buf, size_t size, size_t count)
{
    cout << "Processing AGC with buffer size " << size << ", count " << count << endl;
    int16_t* bufstart = *((int16_t**)buf);
    int16_t* bufstop = *((int16_t**)buf) + count;
    
    double total_l = 0.0;
    double total_r = 0.0;
    double rms_l = 0.0;
    double rms_r = 0.0;
    double peak_l = 0.0;
    double peak_r = 0.0;
    int16_t samp_l, samp_r;
    
    for(int16_t* samp = bufstart; samp < bufstop; samp+=2)
    {
        samp_l = abs(*samp);
        samp_r = abs(*(samp+1));
        
        total_l += pow(samp_l, 2.0);
        total_r += pow(samp_r, 2.0);
        
        if(samp_l > peak_l)
        {
            peak_l = samp_l;
        }
        
        if(samp_r > peak_r)
        {
            peak_r = samp_r;
        }
    }
    
    rms_l = sqrt(total_l / (double)count/2.0);
    rms_r = sqrt(total_r / (double)count/2.0);
    
    cout << "Peak/RMS sample level L: " << peak_l << "/" << rms_l  << ", R: " << peak_r << "/" << rms_r << endl;
}