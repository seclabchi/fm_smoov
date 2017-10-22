#ifndef __PROCESSOR_LPF_H__
#define __PROCESSOR_LPF_H__

#include "processor.h"
#include "biquad.h"

/*
 * Second-order sections for 1st-order 200 Hz butterworth lowpass
 * 0.01292  -0.01292   0.00000   1.00000  -1.00002   0.00000
 * 
 * The rows correspond to biquad sections, the columns 
 * correspond to respectively b0, b1, b2, a0, a1 and a2. 
 * Note that the coefficients have already been normalised.
 */
 
class ProcessorLPF : public Processor
{
public:
    ProcessorLPF(uint32_t samp_rate);
    virtual ~ProcessorLPF();
    virtual void process(void* buf, size_t size, size_t count);
    virtual void enable(bool en);
private:
    bool m_enabled;
    BiQuad* m_bq;
};

#endif