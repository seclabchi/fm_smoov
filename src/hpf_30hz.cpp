/*
 * HPF30Hz.cpp
 *
 *  Created on: Apr 18, 2020
 *      Author: zaremba
 */

#include <hpf_30hz.h>
#include <iostream>

using namespace std;

/*****************************************
 * b0: 30 Hz HPF
 *****************************************
hipass30_sos =

   1.00000  -2.00001   1.00001   1.00000  -1.99607   0.99608
   1.00000  -0.99999   0.00000   1.00000  -0.99608   0.00000

g_hipass30 =  0.99608
*/

HPF30Hz::HPF30Hz(uint32_t bufsize) {
	m_bufsize = bufsize;

	b0hi_sos1 = new SOS(0.99608, 1.00000, -2.00001, 1.00001, -1.99607, 0.99608);
	b0hi_sos2 = new SOS(0.99608, 1.00000, -0.99999, 0.00000, -0.99608, 0.00000);

	b0hiL = new Filter(b0hi_sos1, b0hi_sos2, m_bufsize);
	b0hiR = new Filter(b0hi_sos1, b0hi_sos2, m_bufsize);

}

HPF30Hz::~HPF30Hz() {
	delete b0hi_sos1;
	delete b0hi_sos2;
	delete b0hiL;
	delete b0hiR;
}

void HPF30Hz::process(float* inL, float* inR, float* outL, float* outR, uint32_t samps)
{
	b0hiL->process(inL, samps, outL);
	b0hiR->process(inR, samps, outR);
}

