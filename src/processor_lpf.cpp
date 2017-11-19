#include "processor_lpf.h"

#include <cstdlib>
#include <cstring>
#include <cmath>

ProcessorLPF::ProcessorLPF(audio_params_t* params)
{
    m_enabled = true;
    m_bq = new BiQuad(0.01292, -0.01292, 0.00000, 1.00000, -1.00002, 0.00000);
}

ProcessorLPF::~ProcessorLPF()
{
    delete m_bq;
}

void ProcessorLPF::enable(bool en)
{
    m_enabled = en;
}

void ProcessorLPF::process(float* buf, size_t num_frames)
{
    if(true == m_enabled)
    {
        float* bufstart = (float*)buf;
        float* bufstop = (float*)buf + (num_frames * m_params->frame_size);
        
        for(float* i = bufstart; i < bufstop; i+=2)
        {
            
        }
    }
}