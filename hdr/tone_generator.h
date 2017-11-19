#ifndef __TONE_GENERATOR_H__
#define __TONE_GENERATOR_H__

#include "processor.h"

class ToneGenerator : public Processor
{
public:
    ToneGenerator(audio_params_t* params);
    virtual ~ToneGenerator();
    virtual void process(float* buf, size_t frames);
    virtual void set_frequency(double freq);
    virtual void enable_channels(bool left, bool right);
private:
    double m_freq;
    uint32_t m_tone_pos;
    
    bool m_enable_left;
    bool m_enable_right;
};

#endif
