/*
 * APF200Hz.cpp
 *
 *  Created on: May 21, 2020
 *      Author: zaremba
 */

#include <apf_200hz.h>
#include <iostream>

using namespace std;

/*****************************************
 * b0: 200 Hz HPF
 *****************************************
b0lo_sos =

   1.00000   2.00001   1.00001   1.00000  -1.97348   0.97416
   1.00000   0.99999  -0.00000   1.00000  -0.97416   0.00000

g0lo =  0.0000021853

/*****************************************
 * b0: 200 Hz LPF
 *****************************************

b0hi_sos =

   1.00000  -2.00001   1.00001   1.00000  -1.97348   0.97416
   1.00000  -0.99999   0.00000   1.00000  -0.97416   0.00000

g0hi =  0.97416
*/

APF200Hz::APF200Hz(uint32_t bufsize) {
	m_bufsize = bufsize;

	b0lo_sos1 = new SOS(0.0000021853, 1.00000, 2.00001, 1.00001, -1.97348, 0.97416);
	b0lo_sos2 = new SOS(0.0000021853, 1.00000, 0.99999, -0.00000, -0.97416, 0.00000);
	b0hi_sos1 = new SOS(0.97416, 1.00000, -2.00001, 1.00001, -1.97348, 0.97416);
	b0hi_sos2 = new SOS(0.97416, 1.00000, -0.99999, 0.00000, -0.97416, 0.00000);

	b0loL = new Filter(b0lo_sos1, b0lo_sos2, m_bufsize);
	b0loR = new Filter(b0lo_sos1, b0lo_sos2, m_bufsize);

	b0hiL = new Filter(b0hi_sos1, b0hi_sos2, m_bufsize);
	b0hiR = new Filter(b0hi_sos1, b0hi_sos2, m_bufsize);

	tmpbufLlo = new float[m_bufsize];
	tmpbufRlo = new float[m_bufsize];
	tmpbufLhi = new float[m_bufsize];
	tmpbufRhi = new float[m_bufsize];
}

APF200Hz::~APF200Hz() {
	delete b0lo_sos1;
	delete b0lo_sos2;
	delete b0hi_sos1;
	delete b0hi_sos2;
	delete b0loL;
	delete b0loR;
	delete b0hiL;
	delete b0hiR;
	delete[] tmpbufLlo;
	delete[] tmpbufRlo;
	delete[] tmpbufLhi;
	delete[] tmpbufRhi;
}

void APF200Hz::process(float* inL, float* inR, float* outL, float* outR, uint32_t samps)
{
	b0loL->process(inL, samps, tmpbufLlo);
	b0loR->process(inR, samps, tmpbufRlo);
	b0hiL->process(inL, samps, tmpbufLhi);
	b0hiR->process(inR, samps, tmpbufRhi);

	for(uint32_t i = 0; i < samps; i++)
	{
		outL[i] = tmpbufLlo[i] + tmpbufLhi[i];
		outR[i] = tmpbufRlo[i] + tmpbufRhi[i];
	}

}

