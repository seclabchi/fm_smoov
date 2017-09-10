#ifndef __PROCESSOR_SLOW_AGC_H__
#define __PROCESSOR_SLOW_AGC_H__

#include "processor.h"

class ProcessorSlowAGC : public Processor
{
public:
    ProcessorSlowAGC();
    virtual ~ProcessorSlowAGC();
    virtual void process(void** buf, size_t size, size_t count);
    virtual void set_target_gain(int16_t target);
private:
    int16_t m_target;
};

#endif