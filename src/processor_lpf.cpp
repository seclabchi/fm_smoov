#include "processor_lpf.h"

#include <cstdlib>
#include <cstring>
#include <cmath>

ProcessorLPF::ProcessorLPF(uint32_t samp_rate)
{
    m_simp_state_l = 0;
    m_simp_state_r = 0;
    m_enabled = true;
}

ProcessorLPF::~ProcessorLPF()
{
    
}

void ProcessorLPF::enable(bool en)
{
    m_enabled = en;
}

void ProcessorLPF::process(void* buf, size_t size, size_t count)
{
    if(true == m_enabled)
    {
        int16_t* bufstart = (int16_t*)buf;
        int16_t* bufstop = (int16_t*)buf + count*2;
        int16_t* b = bufstart;
        
        for(uint32_t i = 0; i < count*2; i+=2)
        {
            if(0 == i)
            {
                m_buf[0] = b[0] + m_simp_state_l;
                m_buf[1] = b[1] + m_simp_state_r;
            }
            else
            {
                m_buf[i] = b[i] + b[i-2];
                m_buf[i+1] = b[i+1] + b[i-1];
            }
        }
        
        m_simp_state_l = *(bufstop - 2);
        m_simp_state_r = *(bufstop - 1);
        memcpy(buf, m_buf, count*2);
        
    }
}