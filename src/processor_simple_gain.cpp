#include "processor_simple_gain.h"

#include <cstdlib>
#include <cmath>
#include <sstream>

ProcessorSimpleGain::ProcessorSimpleGain(audio_params_t* params, string name)
    : Processor(params, name)
{
    m_log = new Logger(string(name));
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
    if(false == m_enabled)
    {
        return;
    }
    
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

string ProcessorSimpleGain::do_command(vector<string> cmds)
{
    string retval = "ERROR";
    
    retval = Processor::do_command(cmds);
    
    if(0 == retval.compare("OK"))
    {
        return retval;
    }
    
    if(0 == cmds.at(1).compare("gain"))
    {
        float gain_l, gain_r;
        istringstream(cmds.at(2)) >> gain_l;
        istringstream(cmds.at(3)) >> gain_r;
        
        this->set_gain(gain_l, gain_r);
        retval = "OK";
    }
    
    return retval;
}