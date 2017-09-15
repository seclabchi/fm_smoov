#include "tone_generator.h"

#include <math.h>

ToneGenerator::ToneGenerator(uint32_t samp_rate)
{
    m_samp_rate = samp_rate;
    m_freq = 1000.0;
    m_tone_pos = 0;
    m_enable_left = false;
    m_enable_right = false;
}

ToneGenerator::~ToneGenerator()
{
    
}

void ToneGenerator::process(void* buf, size_t size, size_t count)
{
    if((false == m_enable_left) && (false == m_enable_right))
    {
        return;
    }
    
    int16_t* bufstart = (int16_t*)buf;
    int16_t* bufstop = (int16_t*)buf + count;
    int16_t sampval;
    
    if(m_samp_rate == m_tone_pos)
    {
        m_tone_pos = 0;
    }
    
    for(int16_t* samp = bufstart; samp < bufstop; samp+=2)
    {
        sampval = 32767 * sin(2.0*M_PI*((double)m_tone_pos/48000.0)*m_freq);
        
        if(m_enable_left)
        {
            *samp = sampval;
        }
        
        if(m_enable_right)
        {
            *(samp + 1) = sampval;
        }
        m_tone_pos++;
    }
}

void ToneGenerator::set_frequency(double freq)
{
    m_freq = freq;
}

void ToneGenerator::enable_channels(bool left, bool right)
{
    m_enable_left = left;
    m_enable_right = right;
}
