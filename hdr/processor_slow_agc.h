#ifndef __PROCESSOR_SLOW_AGC_H__
#define __PROCESSOR_SLOW_AGC_H__

#include "processor.h"
#include "logger.h"

class ProcessorSlowAgc : public Processor
{
public:
    ProcessorSlowAgc(uint32_t samp_rate);
    void set_attack_release(float attack, float release);
    void set_target_lin(float target_lin);
    virtual ~ProcessorSlowAgc();
    virtual void process(void* buf, size_t size, size_t count);
private:
    Logger* m_log;
    float m_peak_window;
    float m_time_attack;
    float m_time_release;
    float m_target_lin;
    float m_target_log;
    float m_samp_rate;
    
    float m_gain_master;
    float* m_comp_energy_buf;
    
    int32_t i_energy_accum = 0;
    float energy_accum = 0;
    float energy = 0;
        
    static const uint32_t ENERGY_BUF_LEN = 24000;
};

#endif