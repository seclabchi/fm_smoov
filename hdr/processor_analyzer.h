#ifndef __PROCESSOR_ANALYZER_H__
#define __PROCESSOR_ANALYZER_H__

#include "processor.h"
#include "logger.h"

class ProcessorAnalyzer : public Processor
{
public:
    ProcessorAnalyzer(audio_params_t* params, string name);
    virtual ~ProcessorAnalyzer();
    virtual void process(float* buf, size_t frames);
    virtual void set_modulus(uint32_t modulus);
    virtual string do_command(vector<string> cmds);

private:
    uint32_t m_call_count;
    uint32_t m_modulus;
    Logger* m_log;
};

#endif