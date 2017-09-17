#include "processor_slow_agc.h"

#include <cstdlib>
#include <cmath>

ProcessorSlowAgc::ProcessorSlowAgc(uint32_t samp_rate)
{
    m_log = new Logger(string("ProcessorSlowAgc"));
    m_samp_rate = samp_rate;
}

ProcessorSlowAgc::~ProcessorSlowAgc()
{
    delete(m_log);
}

void ProcessorSlowAgc::set_attack_release(float attack, float release)
{
    m_time_attack = attack;
    m_time_release = release;
    m_gain_master = 1.0;
    
    m_comp_energy_buf = new float[ENERGY_BUF_LEN];
    
    m_log->log_msg(string("Setting attack/release times to " + to_string(m_time_attack) + "/" + to_string(m_time_release)));
}

void ProcessorSlowAgc::set_target_lin(float target_lin)
{
    m_target_lin = target_lin;
    m_target_log = 20.0*log10f(target_lin/32767);
    m_log->log_msg(string("Setting target level to lin/log ") + to_string(m_target_lin) + "/" + to_string(m_target_log));
}

void ProcessorSlowAgc::process(void* buf, size_t size, size_t count)
{
    int16_t* bufstart = (int16_t*)buf;
    int16_t* bufstop = (int16_t*)buf + count;
    
    
    for(int16_t* samp = bufstart; samp < bufstop; samp++)
    {
        energy_accum += powf(*samp, 2);
        i_energy_accum++;
        
        if(ENERGY_BUF_LEN == i_energy_accum)
        {
            energy = sqrtf(energy_accum);
            m_log->log_msg(string("Short term energy: ") + to_string(energy));
            
            i_energy_accum = 0;
            energy_accum = 0;
        }
        *samp =  *samp * m_gain_master;
    }
    
}