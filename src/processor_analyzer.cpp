#include "processor_analyzer.h"

#include <cstdlib>
#include <iostream>
#include <math.h>

using namespace std;

ProcessorAnalyzer::ProcessorAnalyzer(audio_params_t* params, string name) : Processor(params, name)
{
    m_log = new Logger(string(name));
    m_call_count = 0;
    m_modulus = 1;
}

ProcessorAnalyzer::~ProcessorAnalyzer()
{
    delete m_log;
}

void ProcessorAnalyzer::process(float* buf, size_t num_frames)
{
    if(false == m_enabled)
    {
        return;
    }
    
    if(0 == m_call_count % m_modulus)
    {
        float samp_per_chan = num_frames;
        int32_t total_samp = num_frames * m_params->num_chans;
            
        float* bufstart = buf;
        float* bufstop = buf + total_samp;
        
        float total_l = 0.0;
        float total_r = 0.0;
        float rms_l = 0.0;
        float rms_r = 0.0;
        float peak_l = 0.0;
        float peak_r = 0.0;
        float samp_l, samp_r;
        
        for(float* samp = bufstart; samp < bufstop; samp+=2)
        {
            samp_l = fabs(*samp);
            samp_r = fabs(*(samp+1));
            
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
        
        rms_l = sqrtf(total_l / samp_per_chan);
        rms_r = sqrtf(total_r / samp_per_chan);
        
        m_log->log_msg(this->m_name + string(" Peak/RMS sample level L: ") + to_string(peak_l) 
            + "/" + to_string(rms_l)  + ", R: " + to_string(peak_r) + "/" 
            + to_string(rms_r));
    }
    
    m_call_count++;
}

void ProcessorAnalyzer::set_modulus(uint32_t modulus)
{
    if(modulus > 0)
    {
        m_modulus = modulus;
    }
    else
    {
        throw runtime_error("Invalid modulus 0.");
    }
}

string ProcessorAnalyzer::do_command(vector<string> cmds)
{
    string retval = "OK";
    
    retval = Processor::do_command(cmds);
    
    if(0 == retval.compare("OK"))
    {
        return retval;
    }
    
    if(0 == cmds.at(1).compare("modulus"))
    {
        this->set_modulus(atoi(cmds.at(2).c_str()));
        retval = "OK";
    }
    else
    {
        retval = "Unknown command " + cmds.at(1);
    }
    
    return retval;
}