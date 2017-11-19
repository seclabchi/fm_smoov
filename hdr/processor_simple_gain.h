#ifndef __PROCESSOR_SIMPLE_GAIN_H__
#define __PROCESSOR_SIMPLE_GAIN_H__

#include "processor.h"

class ProcessorSimpleGain : public Processor
{
public:
    ProcessorSimpleGain(audio_params_t* params);
    virtual ~ProcessorSimpleGain();
    virtual void process(float* buf, size_t frames);
    virtual void set_gain(float l, float r);
private:
    float m_gain_l;
    float m_gain_r;
};

#endif