#ifndef __PROCESSOR_LPF_H__
#define __PROCESSOR_LPF_H__

#include "processor.h"

class ProcessorLPF : public Processor
{
public:
    ProcessorLPF(uint32_t samp_rate);
    virtual ~ProcessorLPF();
    virtual void process(void* buf, size_t size, size_t count);
    virtual void enable(bool en);
private:
    bool m_enabled;
    int16_t m_simp_state_l;
    int16_t m_simp_state_r;
    int16_t m_buf[40000];
};

#endif