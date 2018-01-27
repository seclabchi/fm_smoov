#include "tone_generator.h"

#include <math.h>
#include <iostream>

ToneGenerator::ToneGenerator(audio_params_t* params, string name) : Processor(params, name)
{
    m_freq = 1000.0;
    m_lev_lin = 1.0;
    m_lev_db = 0.0;
    m_tone_pos = 0;
    m_enable_left = false;
    m_enable_right = false;
}

ToneGenerator::~ToneGenerator()
{
    
}

void ToneGenerator::process(float* buf, size_t num_frames)
{
    if(false == m_enabled)
    {
        return;
    }
    
    if((false == m_enable_left) && (false == m_enable_right))
    {
        return;
    }
    
    float samp_per_chan = num_frames;
    int32_t total_samp = num_frames * m_params->num_chans;
        
    float* bufstart = buf;
    float* bufstop = buf + total_samp;
    
    float sampval;
    
    if(m_params->samp_rate == m_tone_pos)
    {
        m_tone_pos = 0;
    }
    
    for(float* samp = bufstart; samp < bufstop; samp+=2)
    {
        sampval = sin(2.0*M_PI*((double)m_tone_pos/48000.0)*m_freq);
        
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

void ToneGenerator::set_level(double lev_db)
{
    m_lev_db = lev_db;
    m_lev_lin = powf(10, lev_db/20.0);
}


void ToneGenerator::enable_channels(bool left, bool right)
{
    m_enable_left = left;
    m_enable_right = right;
}

string ToneGenerator::do_command(vector<string> cmds)
{
    string retval = "OK";
    
    retval = Processor::do_command(cmds);
    
    if(0 == retval.compare("OK"))
    {
        return retval;
    }
    
    if(0 == cmds.at(1).compare("freq"))
    {
        this->set_frequency(atof(cmds.at(2).c_str()));
        retval = "OK";
    }
    else
    {
        retval = "Unknown command " + cmds.at(1);
    }
    
    return retval;
}