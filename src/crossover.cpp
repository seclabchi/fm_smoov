/*
 * Crossover.cpp
 *
 *  Created on: Aug 18, 2019
 *      Author: zaremba
 */

#include <crossover.h>
#include <iostream>

using namespace std;

/*
b1lo_sos =

   1.00000   2.00001   1.00001   1.00000  -1.97978   0.98017
   1.00000   0.99999  -0.00000   1.00000  -0.98017   0.00000

g1lo =  0.00000098434
============================================================
b1hi_sos =

   1.00000  -2.00001   1.00001   1.00000  -1.97978   0.98017
   1.00000  -0.99999   0.00000   1.00000  -0.98017   0.00000

g1hi =  0.98017
============================================================
b2lo_sos =

   1.00000   2.00001   1.00001   1.00000  -1.88165   0.89364
   1.00000   0.99999  -0.00000   1.00000  -0.89332   0.00000

g2lo =  0.00015984
============================================================
b2hi_sos =

   1.00000  -2.00001   1.00001   1.00000  -1.88165   0.89364
   1.00000  -0.99999   0.00000   1.00000  -0.89332   0.00000

g2hi =  0.89348
============================================================
b3lo_sos =

   1.00000   2.00001   1.00001   1.00000  -1.23349   0.53945
   1.00000   0.99999  -0.00000   1.00000  -0.50131   0.00000

g3lo =  0.019072
============================================================
b3hi_sos =

   1.00000  -2.00001   1.00001   1.00000  -1.23349   0.53945
   1.00000  -0.99999   0.00000   1.00000  -0.50131   0.00000

g3hi =  0.52038
*/

Crossover::Crossover(uint32_t bufsize) {
	m_bufsize = bufsize;
	s1loL = new float[m_bufsize];
	s1loR = new float[m_bufsize];
	s1hiL = new float[m_bufsize];
	s1hiR = new float[m_bufsize];
	s2loL = new float[m_bufsize];
	s2loR = new float[m_bufsize];
	s2hiL = new float[m_bufsize];
	s2hiR = new float[m_bufsize];
	s3hiL = new float[m_bufsize];
	s3hiR = new float[m_bufsize];

	/*  b1lo_sos =
	1.00000   2.00001   1.00001   1.00000  -1.97978   0.98017
	1.00000   0.99999  -0.00000   1.00000  -0.98017   0.00000
	g1lo =  0.00000098434
	*/
	b1lo_sos1 = new SOS(0.00000098434, 1.00000, 2.00001, 1.00001, -1.97978, 0.98017);
	b1lo_sos2 = new SOS(0.00000098434, 1.00000, 0.99999, -0.00000, -0.98017, 0.00000);

	/* b1hi_sos =

	1.00000  -2.00001   1.00001   1.00000  -1.97978   0.98017
	1.00000  -0.99999   0.00000   1.00000  -0.98017   0.00000

	g1hi =  0.98017
	*/
	b1hi_sos1 = new SOS(0.98017, 1.00000, -2.00001, 1.00001, -1.97978, 0.98017);
	b1hi_sos2 = new SOS(0.98017, 1.00000, -0.99999, 0.00000, -0.98017, 0.00000);

	/* b2lo_sos =

	1.00000   2.00001   1.00001   1.00000  -1.88165   0.89364
	1.00000   0.99999  -0.00000   1.00000  -0.89332   0.00000

	g2lo =  0.00015984
	*/
	b2lo_sos1 = new SOS(0.00015984 , 1.00000, 2.00001, 1.00001, -1.88165, 0.89364);
	b2lo_sos2 = new SOS(0.00015984 , 1.00000, 0.99999, -0.00000, -0.89332, 0.00000);

	/* b2hi_sos =

	1.00000  -2.00001   1.00001   1.00000  -1.88165   0.89364
	1.00000  -0.99999   0.00000   1.00000  -0.89332   0.00000

	g2hi =  0.89348
	*/
	b2hi_sos1 = new SOS(0.89348, 1.00000, -2.00001, 1.00001, -1.88165, 0.89364);
	b2hi_sos2 = new SOS(0.89348, 1.00000, -0.99999, 0.00000, -0.89332, 0.00000);

	/* b3lo_sos =

	1.00000   2.00001   1.00001   1.00000  -1.23349   0.53945
	1.00000   0.99999  -0.00000   1.00000  -0.50131   0.00000

	g3lo =  0.019072
	*/
	b3lo_sos1 = new SOS(0.019072, 1.00000, 2.00001, 1.00001, -1.23349, 0.53945);
	b3lo_sos2 = new SOS(0.019072, 1.00000, 0.99999, -0.00000, -0.50131, 0.00000);

	/* b3hi_sos =

	1.00000  -2.00001   1.00001   1.00000  -1.23349   0.53945
	1.00000  -0.99999   0.00000   1.00000  -0.50131   0.00000

	g3hi =  0.52038
	*/
	b3hi_sos1 = new SOS(0.52038, 1.00000, -2.00001, 1.00001, -1.23349, 0.53945);
	b3hi_sos2 = new SOS(0.52038, 1.00000, -0.99999, 0.00000, -0.50131, 0.00000);

	b1loL = new Filter(b1lo_sos1, b1lo_sos2, m_bufsize);
	b1loR = new Filter(b1lo_sos1, b1lo_sos2, m_bufsize);
	b1hiL = new Filter(b1hi_sos1, b1hi_sos2, m_bufsize);
	b1hiR = new Filter(b1hi_sos1, b1hi_sos2, m_bufsize);
	b2loL = new Filter(b2lo_sos1, b2lo_sos2, m_bufsize);
	b2loR = new Filter(b2lo_sos1, b2lo_sos2, m_bufsize);
	b2hiL = new Filter(b2hi_sos1, b2hi_sos2, m_bufsize);
	b2hiR = new Filter(b2hi_sos1, b2hi_sos2, m_bufsize);
	b3loL = new Filter(b3lo_sos1, b3lo_sos2, m_bufsize);
	b3loR = new Filter(b3lo_sos1, b3lo_sos2, m_bufsize);
	b3hiL = new Filter(b3hi_sos1, b3hi_sos2, m_bufsize);
	b3hiR = new Filter(b3hi_sos1, b3hi_sos2, m_bufsize);

	b2apL = new AllpassFilter(b2lo_sos1, b2lo_sos2, b2hi_sos1, b2hi_sos2, m_bufsize);
	b2apR = new AllpassFilter(b2lo_sos1, b2lo_sos2, b2hi_sos1, b2hi_sos2, m_bufsize);
	b3apL = new AllpassFilter(b3lo_sos1, b3lo_sos2, b3hi_sos1, b3hi_sos2, m_bufsize);
	b3apR = new AllpassFilter(b3lo_sos1, b3lo_sos2, b3hi_sos1, b3hi_sos2, m_bufsize);
	b3ap2L = new AllpassFilter(b3lo_sos1, b3lo_sos2, b3hi_sos1, b3hi_sos2, m_bufsize);
	b3ap2R = new AllpassFilter(b3lo_sos1, b3lo_sos2, b3hi_sos1, b3hi_sos2, m_bufsize);


	apbuf1 = new float[m_bufsize];
	apbuf2 = new float[m_bufsize];
}

Crossover::~Crossover() {
	delete[] s1loL;
	delete[] s1loR;
	delete[] s1hiL;
	delete[] s1hiR;
	delete[] s2loL;
	delete[] s2loR;
	delete[] s2hiL;
	delete[] s2hiR;
	delete[] s3hiL;
	delete[] s3hiR;

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

	delete b1loL;
	delete b1loR;
	delete b1hiL;
	delete b1hiR;
	delete b2loL;
	delete b2loR;
	delete b2hiL;
	delete b2hiR;
	delete b3hiL;
	delete b3hiR;

	delete apbuf1;
	delete apbuf2;
}

/* Outs must be a float**, which is allocated by the caller, and is
 * a 2xn array of float vals corresponding to the L/R channels */
void Crossover::process(float* inL, float* inR, float** outb1, float** outb2, float** outb3, float** outb4, int samps)
{
  /*
   * OK, now to port over my Octave work:  The octave code will be interspersed
   *  with the C++ code
  */
	//  stage1lo = filter(b1lo,a1lo,invec);
		b1loL->process(inL, samps, s1loL);
		b1loR->process(inR, samps, s1loR);
	//  stage1hi = filter(b1hi,a1hi,invec);
		b1hiL->process(inL, samps, s1hiL);
		b1hiR->process(inR, samps, s1hiR);
	//
	//  stage2lo = filter(b2lo,a2lo,stage1hi);
		b2loL->process(s1hiL, samps, s2loL);
		b2loR->process(s1hiR, samps, s2loR);
	//  b2_out = -stage2lo;
		invert(s2loL, samps, s2loL);
		invert(s2loR, samps, s2loR);
	//  b2_out_apl = filter(b3lo,a3lo,b2_out);
	//  b2_out_aph = filter(b3hi,a3hi,b2_out);
	//  b2_out = b2_out_apl + b2_out_aph;
		b3apL->process(s2loL, samps, outb2[0]);
		b3apR->process(s2loR, samps, outb2[1]);

	//  stage2hi = filter(b2hi,a2hi,stage1hi);
		b2hiL->process(s1hiL, samps, s2hiR);
		b2hiR->process(s1hiR, samps, s2hiL);
	//  stage1loapl = filter(b2lo,a2lo,stage1lo);
	//  stage1loaph = filter(b2hi,a2hi,stage1lo);
	//  b1_out = stage1loapl + stage1loaph;
		b2apL->process(s1loL, samps, s1loL);
		b2apR->process(s1loR, samps, s1loR);
	//  b1_out_apl = filter(b3lo,a3lo,b1_out);
	//  b1_out_aph = filter(b3hi,a3hi,b1_out);
	//  b1_out = b1_out_apl + b1_out_aph;
		b3ap2L->process(s1loL, samps, outb1[0]);
		b3ap2R->process(s1loR, samps, outb1[1]);
	//
	//  stage3_hi = filter(b3hi,a3hi,stage2hi);
	//  b3_out = filter(b3lo,a3lo,stage2hi);
		b3hiL->process(s2hiL, samps, s3hiL);
		b3hiR->process(s2hiR, samps, s3hiR);
		b3loL->process(s2hiL, samps, outb3[0]);
		b3loR->process(s2hiR, samps, outb3[1]);

	//
	//  b4_out = -stage3_hi;
		invert(s3hiL, samps, outb4[0]);
		invert(s3hiR, samps, outb4[1]);

}

void Crossover::invert(float* in, uint32_t samps, float* out)
{
  for(uint32_t i = 0; i < samps; i++)
  {
    out[i] = -in[i];
  }
}

void Crossover::allpass(float* in, uint32_t samps, Filter* f1, Filter* f2, float* out)
{
  f1->process(in, samps, apbuf1);
  f2->process(in, samps, apbuf2);

  for(uint32_t i = 0; i < samps; i++)
  {
    out[i] = apbuf1[i] + apbuf2[i];
  }
}
