/*
 * CrossoverTwoband.cpp
 *
 *  Created on: Apr 16, 2020
 *      Author: zaremba
 */

#include <crossover_twoband.h>
#include <iostream>

using namespace std;

/*****************************************
 * b0: 200 Hz crossover (low/high pair)
 *****************************************
b0lo_sos =

   1.00000   2.00001   1.00001   1.00000  -1.97348   0.97416
   1.00000   0.99999  -0.00000   1.00000  -0.97416   0.00000

g0lo =  0.0000021853
b0hi_sos =

   1.00000  -2.00001   1.00001   1.00000  -1.97348   0.97416
   1.00000  -0.99999   0.00000   1.00000  -0.97416   0.00000

g0hi =  0.97416
*/

CrossoverTwoband::CrossoverTwoband(uint32_t bufsize) {
	m_bufsize = bufsize;

	/*  b0lo_sos =

   1.00000   2.00001   1.00001   1.00000  -1.97348   0.97416
   1.00000   0.99999  -0.00000   1.00000  -0.97416   0.00000

	g0lo =  0.0000021853
	*/
	b0lo_sos1 = new SOS(0.0000021853, 1.00000, 2.00001, 1.00001, -1.97348, 0.97416);
	b0lo_sos2 = new SOS(0.0000021853, 1.00000, 0.99999, -0.00000, -0.97416, 0.00000);

	/* b0hi_sos =

   1.00000  -2.00001   1.00001   1.00000  -1.97348   0.97416
   1.00000  -0.99999   0.00000   1.00000  -0.97416   0.00000

	g0hi =  0.97416
	*/
	b0hi_sos1 = new SOS(0.97416, 1.00000, -2.00001, 1.00001, -1.97348, 0.97416);
	b0hi_sos2 = new SOS(0.97416, 1.00000, -0.99999, 0.00000, -0.97416, 0.00000);

	b0loL = new Filter(b0lo_sos1, b0lo_sos2, m_bufsize);
	b0loR = new Filter(b0lo_sos1, b0lo_sos2, m_bufsize);

	b0hiL = new Filter(b0hi_sos1, b0hi_sos2, m_bufsize);
	b0hiR = new Filter(b0hi_sos1, b0hi_sos2, m_bufsize);

}

CrossoverTwoband::~CrossoverTwoband() {

	delete b0lo_sos1;
	delete b0lo_sos2;
	delete b0hi_sos1;
	delete b0hi_sos2;
}

/* The ins and outs should be a 2x array of float pointers, for the lo and hi band */
void CrossoverTwoband::process(float* inL, float* inR, float** outL, float** outR, uint32_t samps)
{

	b0loL->process(inL, samps, outL[0]);
	b0hiL->process(inL, samps, outL[1]);
	b0loR->process(inR, samps, outR[0]);
	b0hiR->process(inR, samps, outR[1]);
}

