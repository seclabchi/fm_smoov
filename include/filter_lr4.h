#ifndef __FILTER_LR4_H__
#define __FILTER_LR4_H__

#include "biquad.h"
#include "sos.h"


class FilterLR4
{
  public:
	FilterLR4(SOS* _sos, uint32_t _bufsize);
    virtual ~FilterLR4();
    virtual void process(float* in, float* out);

  private:
    SOS* sos;
    Biquad* bq1;
    Biquad* bq2;
    float* tmpbuf1;
    float* tmpbuf2;
    uint32_t m_bufsize;
};

class AllpassFilterLR4
{
public:
	AllpassFilterLR4(SOS* _soslo, SOS* _soshi, uint32_t _bufsize);
    virtual ~AllpassFilterLR4();
    virtual void process(float* in, float* out);

  private:
    FilterLR4* filterL;
    FilterLR4* filterH;
    float* tmpbufL;
    float* tmpbufH;
    uint32_t m_bufsize;
};

#endif
