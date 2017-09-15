#include "processor_analyzer.h"

#include <cstdlib>
#include <iostream>
#include <math.h>

using namespace std;

ProcessorAnalyzer::ProcessorAnalyzer(uint32_t samp_rate)
{
    m_target = 10000;
}

ProcessorAnalyzer::~ProcessorAnalyzer()
{
    
}

void ProcessorAnalyzer::set_target_gain(int16_t target)
{
    m_target = target;
}

void ProcessorAnalyzer::process(void* buf, size_t size, size_t count)
{
    int16_t* bufstart = (int16_t*)buf;
    int16_t* bufstop = (int16_t*)buf + count;
    
    float total_l = 0.0;
    float total_r = 0.0;
    float rms_l = 0.0;
    float rms_r = 0.0;
    float peak_l = 0.0;
    float peak_r = 0.0;
    int16_t samp_l, samp_r;
    
    for(int16_t* samp = bufstart; samp < bufstop; samp+=2)
    {
        samp_l = abs(*samp);
        samp_r = abs(*(samp+1));
        
        total_l += powf(samp_l, 2.0);
        total_r += powf(samp_r, 2.0);
        
        if(samp_l > peak_l)
        {
            peak_l = samp_l;
        }
        
        if(samp_r > peak_r)
        {
            peak_r = samp_r;
        }
    }
    
    rms_l = sqrtf(total_l / (float)count/2.0);
    rms_r = sqrtf(total_r / (float)count/2.0);
    
    cout << "Peak/RMS sample level L: " << peak_l << "/" << rms_l  << ", R: " << peak_r << "/" << rms_r << endl;
}