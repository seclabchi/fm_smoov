#include "processor_simple_gain.h"

#include <cstdlib>
#include <cmath>

ProcessorSimpleGain::ProcessorSimpleGain(audio_params_t* params)
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

void ProcessorSimpleGain::process(float* buf, size_t num_frames)
{
    float* bufstart = buf;
    float* bufstop = buf + (num_frames * m_params->num_chans);
    
    for(float* samp = bufstart; samp < bufstop; samp+=2)
    {
        //left channel
        *samp =  m_gain_l * (*samp);
 
        //right channel
        *(samp + 1) =  m_gain_r * (*(samp + 1));
    }
    
}