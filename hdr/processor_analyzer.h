#ifndef __PROCESSOR_ANALYZER_H__
#define __PROCESSOR_ANALYZER_H__

#include "processor.h"
#include "logger.h"

class ProcessorAnalyzer : public Processor
{
public:
    ProcessorAnalyzer(uint32_t samp_rate);
    virtual ~ProcessorAnalyzer();
    virtual void process(void* buf, size_t size, size_t count);
    virtual void set_target_gain(int16_t target);
private:
    int16_t m_target;
    Logger* m_log;
};

#endif