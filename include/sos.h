/*
 * sos.h
 *
 *  Created on: Apr 18, 2020
 *      Author: zaremba
 */

#ifndef INCLUDE_SOS_H_
#define INCLUDE_SOS_H_

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




#endif /* INCLUDE_SOS_H_ */
