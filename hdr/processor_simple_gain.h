#ifndef __PROCESSOR_SIMPLE_GAIN_H__
#define __PROCESSOR_SIMPLE_GAIN_H__

#include "processor.h"

class ProcessorSimpleGain : public Processor
{
public:
    ProcessorSimpleGain();
    virtual ~ProcessorSimpleGain();
    virtual void process(void** buf, size_t size, size_t count);
    virtual void set_gain(float l, float r);
private:
    float m_gain_l;
    float m_gain_r;
};

#endif