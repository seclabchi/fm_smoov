#ifndef __FILTER_H__
#define __FILTER_H__

#include "biquad.h"

#define BUFSIZE 100000000

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
    Filter(SOS* _s1, SOS* _s2);
    virtual ~Filter();
    void process(float* in, uint32_t nsamp, float* out);

  private:
    SOS* s1, *s2;
    Biquad* b1, *b2;
    float tmpbuf1[BUFSIZE];
    float tmpbuf2[BUFSIZE];
};

#endif