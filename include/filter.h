#ifndef __FILTER_H__
#define __FILTER_H__

#include "biquad.h"

class SOS
{
  public:
    SOS();
    SOS(float _g, float _b0, float _b1, float _b2, float _a1, float _a2);
    SOS(SOS& rhs);
    float b0;
    float b1;
    float b2;
    float a1;
    float a2;
    float g;
};

class Filter
{
  public:
    Filter(SOS* _s1, SOS* _s2, uint32_t bufsize);
    virtual ~Filter();
    virtual void process(float* in, uint32_t nsamp, float* out);

  private:
    SOS* s1, *s2;
    Biquad* b1, *b2;
    float* tmpbuf;
    uint32_t m_bufsize;
};

class AllpassFilter
{
public:
	AllpassFilter(SOS* _s1l, SOS* _s2l, SOS* _s1h, SOS* _s2h, uint32_t bufsize);
    virtual ~AllpassFilter();
    virtual void process(float* in, uint32_t nsamp, float* out);

  private:
    Filter* filterL;
    Filter* filterH;
    float* tmpbufL;
    float* tmpbufH;
};

#endif
