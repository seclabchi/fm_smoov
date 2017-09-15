#ifndef __TONE_GENERATOR_H__
#define __TONE_GENERATOR_H__

#include "processor.h"

class ToneGenerator : public Processor
{
public:
    ToneGenerator(uint32_t samp_rate);
    virtual ~ToneGenerator();
    virtual void process(void* buf, size_t size, size_t count);
    virtual void set_frequency(double freq);
    virtual void enable_channels(bool left, bool right);
private:
    double m_freq;
    uint32_t m_samp_rate;
    uint32_t m_tone_pos;
    
    bool m_enable_left;
    bool m_enable_right;
};

#endif
