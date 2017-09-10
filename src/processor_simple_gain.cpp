#include "processor_simple_gain.h"

#include <cstdlib>

ProcessorSimpleGain::ProcessorSimpleGain()
{
    m_gain_l = 1.0;
    m_gain_r = 1.0;     
}

ProcessorSimpleGain::~ProcessorSimpleGain()
{
    
}

void ProcessorSimpleGain::set_gain(float l, float r)
{
    m_gain_l = l;
    m_gain_r = r;
}

void ProcessorSimpleGain::process(void** buf, size_t size, size_t count)
{
    int16_t* bufstart = *((int16_t**)buf);
    int16_t* bufstop = *((int16_t**)buf) + count;
    bool leftright = false;
    
    for(int16_t* samp = bufstart; samp < bufstop; samp++)
    {
        if(false == leftright)
        {
            //left channel
            *samp = *samp * m_gain_l;
            leftright = true;
        }
        else
        {
            //right channel
            *samp = *samp * m_gain_r;
            leftright = false;
        }
    }
    
}