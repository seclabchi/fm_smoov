#ifndef __LINLOG_H__
#define __LINLOG_H__

#include <math.h>

#define EPS 0.0000000000001

inline float lin2log(float linval)
{
    linval += EPS;
    float dB = 20 * log10f(fabs(linval));

//    if(dB < -99)
//    {
//      dB = -99;
//    }

    return dB;
}

inline float log2lin(float logval)
{
  float linval = powf(10.0, logval/20.0);
  return linval;
}


#endif
