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

Crossover::Crossover(uint32_t bufsize, float** out) {
	m_bufsize = bufsize;
	s1lo = new float[m_bufsize];
	s1hi = new float[m_bufsize];
	s2lo = new float[m_bufsize];
	s2hi = new float[m_bufsize];
	s3hi = new float[m_bufsize];

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

	b1lo = new Filter(b1lo_sos1, b1lo_sos2, m_bufsize);
	b1hi = new Filter(b1hi_sos1, b1hi_sos2, m_bufsize);
	b2lo = new Filter(b2lo_sos1, b2lo_sos2, m_bufsize);
	b2hi = new Filter(b2hi_sos1, b2hi_sos2, m_bufsize);
	b3lo = new Filter(b3lo_sos1, b3lo_sos2, m_bufsize);
	b3hi = new Filter(b3hi_sos1, b3hi_sos2, m_bufsize);

	b2ap = new AllpassFilter(b2lo_sos1, b2lo_sos2, b2hi_sos1, b2hi_sos2, m_bufsize);
	b3ap = new AllpassFilter(b3lo_sos1, b3lo_sos2, b3hi_sos1, b3hi_sos2, m_bufsize);
	b3ap2 = new AllpassFilter(b3lo_sos1, b3lo_sos2, b3hi_sos1, b3hi_sos2, m_bufsize);


	apbuf1 = new float[m_bufsize];
	apbuf2 = new float[m_bufsize];

	b1 = out[0];
	b2 = out[1];
	b3 = out[2];
	b4 = out[3];

}

Crossover::~Crossover() {
	delete[] s1lo;
	delete[] s1hi;
	delete[] s2lo;
	delete[] s2hi;
	delete[] s3hi;

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

	delete b1lo;
	delete b1hi;
	delete b2lo;
	delete b2hi;
	delete b3hi;

	delete apbuf1;
	delete apbuf2;

}

/* Out must be a float**, which is allocated by the caller, and is
 * a 4xn array of float vals corresponding to the four stereo interleaved channels
 * nsamp is the total number of samples, NOT frames */
void Crossover::process(float* inbuf, int nsamp)
{
  /*
   * OK, now to port over my Octave work:  The octave code will be interspersed
   *  with the C++ code
  */
	//  stage1lo = filter(b1lo,a1lo,invec);
		b1lo->process(inbuf, nsamp, s1lo);
	//  stage1hi = filter(b1hi,a1hi,invec);
		b1hi->process(inbuf, nsamp, s1hi);
	//
	//  stage2lo = filter(b2lo,a2lo,stage1hi);
		b2lo->process(s1hi, nsamp, s2lo);
	//  b2_out = -stage2lo;
		invert(s2lo, nsamp, s2lo);
	//  b2_out_apl = filter(b3lo,a3lo,b2_out);
	//  b2_out_aph = filter(b3hi,a3hi,b2_out);
	//  b2_out = b2_out_apl + b2_out_aph;
		b3ap->process(s2lo, nsamp, b2);

	//  stage2hi = filter(b2hi,a2hi,stage1hi);
		b2hi->process(s1hi, nsamp, s2hi);
	//  stage1loapl = filter(b2lo,a2lo,stage1lo);
	//  stage1loaph = filter(b2hi,a2hi,stage1lo);
	//  b1_out = stage1loapl + stage1loaph;
		b2ap->process(s1lo, nsamp, s1lo);
	//  b1_out_apl = filter(b3lo,a3lo,b1_out);
	//  b1_out_aph = filter(b3hi,a3hi,b1_out);
	//  b1_out = b1_out_apl + b1_out_aph;
		b3ap2->process(s1lo, nsamp, b1);
	//
	//  stage3_hi = filter(b3hi,a3hi,stage2hi);
	//  b3_out = filter(b3lo,a3lo,stage2hi);
		b3hi->process(s2hi, nsamp, s3hi);
		b3lo->process(s2hi, nsamp, b3);
	//
	//  b4_out = -stage3_hi;
		invert(s3hi, nsamp, b4);

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
