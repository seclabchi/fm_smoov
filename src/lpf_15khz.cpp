/*
 * LPF15kHz.cpp
 *
 *  Created on: Apr 18, 2020
 *      Author: zaremba
 */

#include <lpf_15khz.h>
#include <iostream>

using namespace std;

/*****************************************
 * b0: 15 kHz LPF
 *****************************************
lopass15_sos =

   1.000000   2.004794   1.004817   1.000000   0.617671   0.614052
   1.000000   1.995200   0.995223   1.000000   0.462938   0.209715
   1.000000   2.000006   0.999983   1.000000   0.404443   0.056860

g_lopass15 =  0.085233
*/

/*****************************************
 * b0: 16 kHz LPF
 *****************************************
lopass15_sos =

   1.000000   2.003378   1.003390   1.000000   0.816897   0.633795
   1.000000   1.996591   0.996603   1.000000   0.620204   0.240408
   1.000000   2.000030   1.000019   1.000000   0.544509   0.089018

g_lopass15 =  0.11638
*/

LPF15kHz::LPF15kHz(uint32_t bufsize) {
	m_bufsize = bufsize;

	sos1 = new SOS(0.085233, 1.00000, 2.004794, 1.004817, 0.617671, 0.614052);
	sos2 = new SOS(0.085233, 1.00000, 1.995200, 0.995223, 0.462938, 0.209715);
	sos3 = new SOS(0.085233, 1.00000, 2.000006, 0.999983, 0.404443, 0.056860);

	fL = new Filter3SOS(sos1, sos2, sos3, m_bufsize);
	fR = new Filter3SOS(sos1, sos2, sos3, m_bufsize);

}

LPF15kHz::~LPF15kHz() {
	delete sos1;
	delete sos2;
	delete sos3;
	delete fL;
	delete fR;
}

void LPF15kHz::process(float* inL, float* inR, float* outL, float* outR, uint32_t samps)
{
	fL->process(inL, samps, outL);
	fR->process(inR, samps, outR);
}

