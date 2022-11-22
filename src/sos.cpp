/*
 * sos.cpp
 *
 *  Created on: Apr 18, 2020
 *      Author: zaremba
 */

#include "sos.h"

SOS::SOS()
{
}

SOS::SOS(float _g, float _b0, float _b1, float _b2, float _a0, float _a1, float _a2): g(_g),
  b0(_b0), b1(_b1), b2(_b2), a0(_a0), a1(_a1), a2(_a2)
{
}

SOS::SOS(SOS& rhs)
{
  g = rhs.g;
  b0 = rhs.b0;
  b1 = rhs.b1;
  b2 = rhs.b2;
  a0 = rhs.a0;
  a1 = rhs.a1;
  a2 = rhs.a2;
}



