#include "processor_lpf.h"

#include <cstdlib>
#include <cstring>
#include <cmath>

ProcessorLPF::ProcessorLPF(uint32_t samp_rate)
{
    B[0] = 0.01292;  
    B[1] = -0.01292;
    B[2] = 0.00000;
    A[0] = 1.00000;
    A[1] = -1.00002;
    A[2] = 0.00000;
    m_enabled = true;
    m_last_output = 0.0;
    m_buf = new int16_t[20000 * 2];
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
            m_buf[i] = *(bufstart + i);
            m_buf[i+1] = *(bufstart + i + 1);
        }
        
        memcpy(buf, m_buf, count*2);
        
    }
}