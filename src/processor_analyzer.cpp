#include "processor_analyzer.h"

#include <cstdlib>
#include <iostream>
#include <math.h>

using namespace std;

ProcessorAnalyzer::ProcessorAnalyzer(audio_params_t* params, string name) : Processor(params, name)
{
    m_log = new Logger(string("ProcessorAnalyzer"));
}

ProcessorAnalyzer::~ProcessorAnalyzer()
{
    delete m_log;
}

void ProcessorAnalyzer::process(float* buf, size_t num_frames)
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
    
    m_log->log_msg(string("Peak/RMS sample level L: ") + to_string(peak_l) + "/" + to_string(rms_l)  + ", R: " + to_string(peak_r) + "/" + to_string(rms_r));
}