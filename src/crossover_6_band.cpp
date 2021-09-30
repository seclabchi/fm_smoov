/*
 * Crossover6band.cpp
 *
 *  Created on: Jun 12, 2020
 *      Author: zaremba
 */

#include <crossover_6_band.h>
#include <iostream>

using namespace std;

/*
orban 8100 xt/a2 crossovers
fcross = [153, 430, 1100, 2400, 5900];

####################################################
Cross 3: 153 Hz
b3lo_sos =

   1.00000   2.00001   1.00001   1.00000  -1.97978   0.98017
   1.00000   0.99999  -0.00000   1.00000  -0.98017   0.00000

g3lo =  0.00000098434
b3hi_sos =

   1.00000  -2.00001   1.00001   1.00000  -1.97978   0.98017
   1.00000  -0.99999   0.00000   1.00000  -0.98017   0.00000

g3hi =  0.98017

####################################################
Cross 1: 430 Hz
b1lo_sos =

   1.00000   2.00001   1.00001   1.00000  -1.94220   0.94528
   1.00000   0.99999  -0.00000   1.00000  -0.94524   0.00000

g1lo =  0.000021088
b1hi_sos =

   1.00000  -2.00001   1.00001   1.00000  -1.94220   0.94528
   1.00000  -0.99999   0.00000   1.00000  -0.94524   0.00000

g1hi =  0.94526

####################################################
Cross 4: 1100 Hz
b4lo_sos =

   1.00000   2.00001   1.00001   1.00000  -1.84680   0.86611
   1.00000   0.99999  -0.00000   1.00000  -0.86546   0.00000

g4lo =  0.00032477
b4hi_sos =

   1.00000  -2.00001   1.00001   1.00000  -1.84680   0.86611
   1.00000  -0.99999   0.00000   1.00000  -0.86546   0.00000

g4hi =  0.86579

####################################################
Cross 0: 2400 Hz
b0lo_sos =

   1.00000   2.00001   1.00001   1.00000  -1.64755   0.73234
   1.00000   0.99999  -0.00000   1.00000  -0.72654   0.00000

g0lo =  0.0028982
b0hi_sos =

   1.00000  -2.00001   1.00001   1.00000  -1.64755   0.73234
   1.00000  -0.99999   0.00000   1.00000  -0.72654   0.00000

g0hi =  0.72944

####################################################
Cross 2: 5900 Hz
b2lo_sos =

   1.00000   2.00001   1.00001   1.00000  -1.06206   0.48269
   1.00000   0.99999  -0.00000   1.00000  -0.42190   0.00000

g2lo =  0.030396
b2hi_sos =

   1.00000  -2.00001   1.00001   1.00000  -1.06206   0.48269
   1.00000  -0.99999   0.00000   1.00000  -0.42190   0.00000

g2hi =  0.45230

*/

Crossover6band::Crossover6band(uint32_t bufsize) {
	m_bufsize = bufsize;
	s0loL = new float[m_bufsize];
	s0loR = new float[m_bufsize];
	s0hiL = new float[m_bufsize];
	s0hiR = new float[m_bufsize];
	s1loL = new float[m_bufsize];
	s1loR = new float[m_bufsize];
	s1hiL = new float[m_bufsize];
	s1hiR = new float[m_bufsize];

	/*  cross 3 - 153 Hz b3lo_sos =

   1.00000   2.00001   1.00001   1.00000  -1.97978   0.98017
   1.00000   0.99999  -0.00000   1.00000  -0.98017   0.00000

	g3lo =  0.00000098434
	*/
	b3lo_sos1 = new SOS(0.00000098434, 1.00000, 2.00001, 1.00001, -1.97978, 0.98017);
	b3lo_sos2 = new SOS(0.00000098434, 1.00000, 0.99999, -0.00000, -0.98017, 0.00000);

	/* b3hi_sos =

   1.00000  -2.00001   1.00001   1.00000  -1.97978   0.98017
   1.00000  -0.99999   0.00000   1.00000  -0.98017   0.00000

    g3hi =  0.98017
	*/
	b3hi_sos1 = new SOS(0.98017, 1.00000, -2.00001, 1.00001, -1.97978, 0.98017);
	b3hi_sos2 = new SOS(0.98017, 1.00000, -0.99999, 0.00000, -0.98017, 0.00000);

	/* cross 1 - 430 Hz b1lo_sos =

   1.00000   2.00001   1.00001   1.00000  -1.94220   0.94528
   1.00000   0.99999  -0.00000   1.00000  -0.94524   0.00000

	g1lo =  0.000021088
	*/
	b1lo_sos1 = new SOS(0.000021088, 1.00000, 2.00001, 1.00001, -1.94220, 0.94528);
	b1lo_sos2 = new SOS(0.000021088, 1.00000, 0.99999, -0.00000, -0.94524, 0.00000);

	/* b1hi_sos =

   1.00000  -2.00001   1.00001   1.00000  -1.94220   0.94528
   1.00000  -0.99999   0.00000   1.00000  -0.94524   0.00000

	g1hi =  0.94526
	*/
	b1hi_sos1 = new SOS(0.94526, 1.00000, -2.00001, 1.00001, -1.94220, 0.94528);
	b1hi_sos2 = new SOS(0.94526, 1.00000, -0.99999, 0.00000, -0.94524, 0.00000);

	/* cross 4 - 1.1 kHz b4lo_sos =

   1.00000   2.00001   1.00001   1.00000  -1.84680   0.86611
   1.00000   0.99999  -0.00000   1.00000  -0.86546   0.00000

	g4lo =  0.00032477
	*/
	b4lo_sos1 = new SOS(0.00032477, 1.00000, 2.00001, 1.00001, -1.84680, 0.86611);
	b4lo_sos2 = new SOS(0.00032477, 1.00000, 0.99999, -0.00000, -0.86546, 0.00000);

	/* b4hi_sos =

   1.00000  -2.00001   1.00001   1.00000  -1.84680   0.86611
   1.00000  -0.99999   0.00000   1.00000  -0.86546   0.00000

	g4hi =  0.86579
	*/
	b4hi_sos1 = new SOS(0.86579, 1.00000, -2.00001, 1.00001, -1.84680, 0.86611);
	b4hi_sos2 = new SOS(0.86579, 1.00000, -0.99999, 0.00000, -0.86546, 0.00000);

	/* cross 0 - 2.4 kHz b0lo_sos =

   1.00000   2.00001   1.00001   1.00000  -1.64755   0.73234
   1.00000   0.99999  -0.00000   1.00000  -0.72654   0.00000

	g0lo =  0.0028982
	*/
	b0lo_sos1 = new SOS(0.0028982, 1.00000, 2.00001, 1.00001, -1.64755, 0.73234);
	b0lo_sos2 = new SOS(0.0028982, 1.00000, 0.99999, -0.00000, -0.72654, 0.00000);

	/* b0hi_sos =

   1.00000  -2.00001   1.00001   1.00000  -1.64755   0.73234
   1.00000  -0.99999   0.00000   1.00000  -0.72654   0.00000

	g0hi =  0.72944
	*/
	b0hi_sos1 = new SOS(0.72944, 1.00000, -2.00001, 1.00001, -1.64755, 0.73234);
	b0hi_sos2 = new SOS(0.72944, 1.00000, -0.99999, 0.00000, -0.72654, 0.00000);

	/* cross 2 - 5.9 kHz b2lo_sos =

   1.00000   2.00001   1.00001   1.00000  -1.06206   0.48269
   1.00000   0.99999  -0.00000   1.00000  -0.42190   0.00000

	g2lo =  0.030396
	*/
	b2lo_sos1 = new SOS(0.030396, 1.00000, 2.00001, 1.00001, -1.06206, 0.48269);
	b2lo_sos2 = new SOS(0.030396, 1.00000, 0.99999, -0.00000, -0.42190, 0.00000);

	/* b2hi_sos =

   1.00000  -2.00001   1.00001   1.00000  -1.06206   0.48269
   1.00000  -0.99999   0.00000   1.00000  -0.42190   0.00000

	g4hi =  0.45230
	*/
	b2hi_sos1 = new SOS(0.45230, 1.00000, -2.00001, 1.00001, -1.06206, 0.48269);
	b2hi_sos2 = new SOS(0.45230, 1.00000, -0.99999, 0.00000, -0.42190, 0.00000);


	f0loL = new Filter(b0lo_sos1, b0lo_sos2, m_bufsize);
	f0loR = new Filter(b0lo_sos1, b0lo_sos2, m_bufsize);
	f0hiL = new Filter(b0hi_sos1, b0hi_sos2, m_bufsize);
	f0hiR = new Filter(b0hi_sos1, b0hi_sos2, m_bufsize);
	f1loL = new Filter(b1lo_sos1, b1lo_sos2, m_bufsize);
	f1loR = new Filter(b1lo_sos1, b1lo_sos2, m_bufsize);
	f1hiL = new Filter(b1hi_sos1, b1hi_sos2, m_bufsize);
	f1hiR = new Filter(b1hi_sos1, b1hi_sos2, m_bufsize);
	f2loL = new Filter(b2lo_sos1, b2lo_sos2, m_bufsize);
	f2loR = new Filter(b2lo_sos1, b2lo_sos2, m_bufsize);
	f2hiL = new Filter(b2hi_sos1, b2hi_sos2, m_bufsize);
	f2hiR = new Filter(b2hi_sos1, b2hi_sos2, m_bufsize);
	f3loL = new Filter(b3lo_sos1, b3lo_sos2, m_bufsize);
	f3loR = new Filter(b3lo_sos1, b3lo_sos2, m_bufsize);
	f3hiL = new Filter(b3hi_sos1, b3hi_sos2, m_bufsize);
	f3hiR = new Filter(b3hi_sos1, b3hi_sos2, m_bufsize);
	f4loL = new Filter(b4lo_sos1, b4lo_sos2, m_bufsize);
	f4loR = new Filter(b4lo_sos1, b4lo_sos2, m_bufsize);
	f4hiL = new Filter(b4hi_sos1, b4hi_sos2, m_bufsize);
	f4hiR = new Filter(b4hi_sos1, b4hi_sos2, m_bufsize);

	ap2L = new AllpassFilter(b2lo_sos1, b2lo_sos2, b2hi_sos1, b2hi_sos2, m_bufsize);
	ap2R = new AllpassFilter(b2lo_sos1, b2lo_sos2, b2hi_sos1, b2hi_sos2, m_bufsize);
	ap1Lp = new AllpassFilter(b1lo_sos1, b1lo_sos2, b1hi_sos1, b1hi_sos2, m_bufsize);
	ap1Rp = new AllpassFilter(b1lo_sos1, b1lo_sos2, b1hi_sos1, b1hi_sos2, m_bufsize);
	ap3L = new AllpassFilter(b3lo_sos1, b3lo_sos2, b3hi_sos1, b3hi_sos2, m_bufsize);
	ap3Lp = new AllpassFilter(b3lo_sos1, b3lo_sos2, b3hi_sos1, b3hi_sos2, m_bufsize);
	ap3R = new AllpassFilter(b3lo_sos1, b3lo_sos2, b3hi_sos1, b3hi_sos2, m_bufsize);
	ap3Rp = new AllpassFilter(b3lo_sos1, b3lo_sos2, b3hi_sos1, b3hi_sos2, m_bufsize);
	ap4L = new AllpassFilter(b4lo_sos1, b4lo_sos2, b4hi_sos1, b4hi_sos2, m_bufsize);
	ap4Lp = new AllpassFilter(b4lo_sos1, b4lo_sos2, b4hi_sos1, b4hi_sos2, m_bufsize);
	ap4R = new AllpassFilter(b4lo_sos1, b4lo_sos2, b4hi_sos1, b4hi_sos2, m_bufsize);
	ap4Rp = new AllpassFilter(b4lo_sos1, b4lo_sos2, b4hi_sos1, b4hi_sos2, m_bufsize);

}

Crossover6band::~Crossover6band() {
	delete[] s0loL;
	delete[] s0loR;
	delete[] s0hiL;
	delete[] s0hiR;
	delete[] s1loL;
	delete[] s1loR;
	delete[] s1hiL;
	delete[] s1hiR;

	delete f0loL;
	delete f0loR;
	delete f0hiL;
	delete f0hiR;
	delete f1loL;
	delete f1loR;
	delete f1hiL;
	delete f1hiR;
	delete f2loL;
	delete f2loR;
	delete f2hiL;
	delete f2hiR;
	delete f3loL;
	delete f3loR;
	delete f3hiL;
	delete f3hiR;
	delete f4loL;
	delete f4loR;
	delete f4hiL;
	delete f4hiR;

	delete ap2L;
	delete ap2R;
	delete ap1Lp;
	delete ap1Rp;
	delete ap3L;
	delete ap3Lp;
	delete ap3R;
	delete ap3Rp;
	delete ap4L;
	delete ap4Lp;
	delete ap4R;
	delete ap4Rp;

	delete b0lo_sos1;
	delete b0lo_sos2;
	delete b0hi_sos1;
	delete b0hi_sos2;
	delete b1lo_sos1;
	delete b1lo_sos2;
	delete b1hi_sos1;
	delete b1hi_sos2;
	delete b2lo_sos1;
	delete b2lo_sos2;
	delete b2hi_sos1;
	delete b2hi_sos2;
	delete b3lo_sos1;
	delete b3lo_sos2;
	delete b3hi_sos1;
	delete b3hi_sos2;
	delete b4lo_sos1;
	delete b4lo_sos2;
	delete b4hi_sos1;
	delete b4hi_sos2;
}

void Crossover6band::band_enable(bool _b0, bool _b1, bool _b2, bool _b3, bool _b4, bool _b5)
{
	be0 = _b0;
	be1 = _b1;
	be2 = _b2;
	be3 = _b3;
	be4 = _b4;
	be5 = _b5;
}

/* Outs must be a float**, which is allocated by the caller, and is
 * a 2xn array of float vals corresponding to the L/R channels */
void Crossover6band::process(float* inL, float* inR, float** outb1, float** outb2, float** outb3,
		                     float** outb4, float** outb5, float** outb6, int samps)
{
	band0outL = outb1[0];
	band0outR = outb1[1];
	band1outL = outb2[0];
	band1outR = outb2[1];
	band2outL = outb3[0];
	band2outR = outb3[1];
	band3outL = outb4[0];
	band3outR = outb4[1];
	band4outL = outb5[0];
	band4outR = outb5[1];
	band5outL = outb6[0];
	band5outR = outb6[1];

	/* OK, we're gonna look at this from a standpoint of processing stages.
	 *
	 * S0LO:  f0LP, f2AP
	 * S0HI:  f0HP, f1AP, f3AP, f4AP
	 *
	 * S0LO-S1LO: f1LP, f4AP
	 * S0LO-S1LO-S2LO: f3LP = BAND 0 OUT
	 * S0LO-S1LO-S2HI: f3HP = BAND 1 OUT
	 *
	 * S0LO-S1HI: f1HP, f3AP
	 * S0LO-S1HI-S2LO: f4LP = BAND 2 OUT
	 * S0LO-S1HI-S2HI: f4HP = BAND 3 OUT
	 *
	 * S0HI-S1LO:  f4LP = BAND 4 OUT
	 * S0HI-S1HI:  f4HP = BAND 5 OUT
	 *
	 */

	/* S0 split 2.4 kHz */

	/* Low-pass */
	f0loL->process(inL, samps, s0loL);
	f0loR->process(inR, samps, s0loR);
	ap2L->process(s0loL, samps, s0loL);
	ap2R->process(s0loR, samps, s0loR);

	/* High-pass */
	f0hiL->process(inL, samps, s0hiL);
	f0hiR->process(inR, samps, s0hiR);
	ap1Lp->process(s0hiL, samps, s0hiL);
	ap1Rp->process(s0hiR, samps, s0hiR);
	ap3Lp->process(s0hiL, samps, s0hiL);
	ap3Rp->process(s0hiR, samps, s0hiR);
	ap4Lp->process(s0hiL, samps, s0hiL);
	ap4Rp->process(s0hiR, samps, s0hiR);

	/* S0 HP S1 split 5.9 kHz */

	/* Low-pass */
	if(true == be4)
	{
		f2loL->process(s0hiL, samps, band4outL);
		f2loR->process(s0hiR, samps, band4outR);
	}

	/* High-pass */
	if(true == be5)
	{
		f2hiL->process(s0hiL, samps, band5outL);
		f2hiR->process(s0hiR, samps, band5outR);
	}

	/* S0 LP S1 split 430 Hz */

	/* Low-pass */
	f1loL->process(s0loL, samps, s1loL);
	f1loR->process(s0loR, samps, s1loR);
	ap4L->process(s1loL, samps, s1loL);
	ap4R->process(s1loR, samps, s1loR);

	/* High-pass */
	f1hiL->process(s0loL, samps, s1hiL);
	f1hiR->process(s0loL, samps, s1hiR);
	ap3L->process(s1hiL, samps, s1hiL);
	ap3R->process(s1hiR, samps, s1hiR);

	/* S0 LP S1 LP S2 LP split 153 Hz */
	if(true == be0)
	{
		f3loL->process(s1loL, samps, band0outL);
		f3loR->process(s1loR, samps, band0outR);
	}
	if(true == be1)
	{
		f3hiL->process(s1loL, samps, band1outL);
		f3hiR->process(s1loR, samps, band1outR);
	}

	/* S0 LP S1 LP S2 HP split 1.1 kHz */
	if(true == be2)
	{
		f4loL->process(s1hiL, samps, band2outL);
		f4loR->process(s1hiR, samps, band2outR);
	}
	if(true == be3)
	{
		f4hiL->process(s1hiL, samps, band3outL);
		f4hiR->process(s1hiR, samps, band3outR);
	}
}

