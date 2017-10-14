#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include <stddef.h>
#include <stdint.h>

class Processor
{
public:
    Processor(uint32_t samp_rate);
    virtual ~Processor() {};
    virtual void process(void* buf, size_t size, size_t count) = 0;
protected:
    Processor() {};
    uint32_t m_samp_rate;
};

#endif