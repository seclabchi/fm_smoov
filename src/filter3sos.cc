#include "filter3sos.h"
#include <string.h>
#include <iostream>


using namespace std;


Filter3SOS::Filter3SOS(SOS* _s1, SOS* _s2, SOS* _s3, uint32_t bufsize)
{
  s1 = new SOS(*_s1);
  s2 = new SOS(*_s2);
  s3 = new SOS(*_s3);
  
//  octave_stdout << "Creating new biquad with params " << s1->g << " " << s1->b0 << " " << s1->b1 << " " << s1->b2 << " " << s1->a1 << " " << s1->a2 << endl;
  b1 = new Biquad(s1->g, s1->b0, s1->b1, s1->b2, 1.0, s1->a1, s1->a2);
//  octave_stdout << "Creating new biquad with params " << s2->g << " " << s2->b0 << " " << s2->b1 << " " << s2->b2 << " " << s2->a1 << " " << s2->a2 << endl;
  b2 = new Biquad(s2->g, s2->b0, s2->b1, s2->b2, 1.0, s2->a1, s2->a2);
  b3 = new Biquad(s3->g, s3->b0, s3->b1, s3->b2, 1.0, s3->a1, s3->a2);
  m_bufsize = bufsize;
  tmpbuf = new float[m_bufsize];
}

Filter3SOS::~Filter3SOS()
{
}

void Filter3SOS::process(float* in, uint32_t nsamp, float* out)
{
    memcpy(tmpbuf, in, nsamp * sizeof(float));
    b1->process(tmpbuf, nsamp, false);
    b2->process(tmpbuf, nsamp, false); 
    b3->process(tmpbuf, nsamp, false);
  
  for(uint32_t i = 0; i < nsamp; i++)
  {
    out[i] = s1->g * tmpbuf[i];
  }
}


