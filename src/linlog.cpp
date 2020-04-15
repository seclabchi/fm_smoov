#include "linlog.h"

float lin2log(float linval)
{
    linval += EPS;
    float dB = 20 * log10f(fabs(linval));

//    if(dB < -99)
//    {
//      dB = -99;
//    }

    return dB;
}

float log2lin(float logval)
{
  float linval = powf(10.0, logval/20.0);
  return linval;
}
