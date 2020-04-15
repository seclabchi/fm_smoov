#include "filter.h"
#include <string.h>
#include <iostream>


using namespace std;

SOS::SOS()
{
}

SOS::SOS(float _g, float _b0, float _b1, float _b2, float _a1, float _a2): g(_g),
  b0(_b0), b1(_b1), b2(_b2), a1(_a1), a2(_a2)
{
}

SOS::SOS(SOS& rhs)
{
  g = rhs.g;
  b0 = rhs.b0;
  b1 = rhs.b1;
  b2 = rhs.b2;
  a1 = rhs.a1;
  a2 = rhs.a2;
}

Filter::Filter(SOS* _s1, SOS* _s2, uint32_t bufsize)
{
  s1 = new SOS(*_s1);
  s2 = new SOS(*_s2);
  
//  octave_stdout << "Creating new biquad with params " << s1->g << " " << s1->b0 << " " << s1->b1 << " " << s1->b2 << " " << s1->a1 << " " << s1->a2 << endl;
  b1 = new Biquad(s1->g, s1->b0, s1->b1, s1->b2, 1.0, s1->a1, s1->a2);
//  octave_stdout << "Creating new biquad with params " << s2->g << " " << s2->b0 << " " << s2->b1 << " " << s2->b2 << " " << s2->a1 << " " << s2->a2 << endl;
  b2 = new Biquad(s2->g, s2->b0, s2->b1, s2->b2, 1.0, s2->a1, s2->a2);
  m_bufsize = bufsize;
  tmpbuf = new float[m_bufsize];
}

Filter::~Filter()
{
}

void Filter::process(float* in, uint32_t nsamp, float* out)
{
    memcpy(tmpbuf, in, nsamp * sizeof(float));
    b1->process(tmpbuf, nsamp, false);
    b2->process(tmpbuf, nsamp, false); 
  
  for(uint32_t i = 0; i < nsamp; i++)
  {
    out[i] = s1->g * tmpbuf[i];
  }
}

AllpassFilter::AllpassFilter(SOS* _s1l, SOS* _s2l, SOS* _s1h, SOS* _s2h, uint32_t bufsize)
{
	filterL = new Filter(_s1l, _s2l, bufsize);
	filterH = new Filter(_s1h, _s2h, bufsize);
	tmpbufL = new float[bufsize];
	tmpbufH = new float[bufsize];
}

AllpassFilter::~AllpassFilter()
{
	delete filterL;
	delete filterH;
	delete[] tmpbufL;
	delete[] tmpbufH;
}

void AllpassFilter::process(float* in, uint32_t nsamp, float* out)
{
	filterL->process(in, nsamp, tmpbufL);
	filterH->process(in, nsamp, tmpbufH);

	for(int i = 0; i < nsamp; i++)
	{
		out[i] = tmpbufL[i] + tmpbufH[i];
	}
}

