#include "processor_lpf.h"

#include <cstdlib>
#include <cstring>
#include <cmath>

ProcessorLPF::ProcessorLPF(uint32_t samp_rate)
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

void ProcessorLPF::process(void* buf, size_t size, size_t count)
{
    if(true == m_enabled)
    {
        int16_t* bufstart = (int16_t*)buf;
        int16_t* bufstop = (int16_t*)buf + count*2;
        
        for(uint32_t i = 0; i < count*2; i+=2)
        {
            
        }
    }
}