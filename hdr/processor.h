#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include <stddef.h>
#include <stdint.h>

#include "audio_params.h"

class Processor
{
public:
    Processor(audio_params_t* params);
    virtual ~Processor();
    virtual void process(float* buf, size_t frames) = 0;
protected:
    Processor() {};
    audio_params_t* m_params;
    float* m_buf_in;
    float* m_buf_out;
};

#endif