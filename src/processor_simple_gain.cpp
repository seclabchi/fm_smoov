#include "processor_simple_gain.h"

#include <cstdlib>
#include <cmath>

ProcessorSimpleGain::ProcessorSimpleGain(uint32_t samp_rate)
{
    this->set_gain(0.0, 0.0);     
}

ProcessorSimpleGain::~ProcessorSimpleGain()
{
    
}

void ProcessorSimpleGain::set_gain(float l, float r)
{
    m_gain_l = powf(10.0, l/20.0);
    m_gain_r = powf(10.0, r/20.0);
}

void ProcessorSimpleGain::process(void* buf, size_t size, size_t count)
{
    int16_t* bufstart = (int16_t*)buf;
    int16_t* bufstop = (int16_t*)buf + count;
    bool leftright = false;
    
    for(int16_t* samp = bufstart; samp < bufstop; samp++)
    {
        if(false == leftright)
        {
            //left channel
            *samp =  m_gain_l * (*samp);
            leftright = true;
        }
        else
        {
            //right channel
            *samp =  m_gain_r * (*samp);
            leftright = false;
        }
    }
    
}