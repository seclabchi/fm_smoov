#ifndef __FILTER3SOS_H__
#define __FILTER3SOS_H__

#include "biquad.h"
#include "sos.h"

class Filter3SOS
{
  public:
	Filter3SOS(SOS* _s1, SOS* _s2, SOS* _s3, uint32_t bufsize);
    virtual ~Filter3SOS();
    virtual void process(float* in, uint32_t nsamp, float* out);

  private:
    SOS* s1, *s2, *s3;
    Biquad* b1, *b2, *b3;
    float* tmpbuf;
    uint32_t m_bufsize;
};


#endif /* __FILTER3SOS_H__ */
