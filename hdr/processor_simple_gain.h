#ifndef __PROCESSOR_SIMPLE_GAIN_H__
#define __PROCESSOR_SIMPLE_GAIN_H__

#include "processor.h"
#include "logger.h"

#include <string>

using namespace std;
using namespace Tonekids;

class ProcessorSimpleGain : public Processor
{
public:
    ProcessorSimpleGain(audio_params_t* params, string name);
    virtual ~ProcessorSimpleGain();
    virtual void process(float* buf, size_t frames);
    virtual void set_gain(float l, float r);
    virtual string do_command(vector<string> cmds);
private:
    float m_gain_l;
    float m_gain_r;
    Logger* m_log;
};

#endif