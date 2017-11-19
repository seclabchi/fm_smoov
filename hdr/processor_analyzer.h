#ifndef __PROCESSOR_ANALYZER_H__
#define __PROCESSOR_ANALYZER_H__

#include "processor.h"
#include "logger.h"

class ProcessorAnalyzer : public Processor
{
public:
    ProcessorAnalyzer(audio_params_t* params);
    virtual ~ProcessorAnalyzer();
    virtual void process(float* buf, size_t frames);
private:
    int16_t m_target;
    Logger* m_log;
};

#endif