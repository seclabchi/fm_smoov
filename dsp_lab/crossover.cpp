/* 





*/

#include <octave/oct.h>
#include "filter.h"

using namespace std;
void invert(float* in, uint32_t samps, float* out)
{
  for(int i = 0; i < samps; i++)
  {
    out[i] = -in[i];
  }
}

void allpass(float* in, uint32_t samps, Filter* f1, Filter* f2, float* out)
{
  float* buf1 = new float[samps];
  float* buf2 = new float[samps];
  
  f1->process(in, samps, buf1);
  f2->process(in, samps, buf2);
  
  for(int i = 0; i < samps; i++)
  {
    out[i] = buf1[i] + buf2[i];
  }
  
  delete[] buf1;
  delete[] buf2;
}
  
DEFUN_DLD (crossover, args, nargout,
           "Crossover Help String")
{
  int nargin = args.length();

  octave_stdout << "crossover has "
                << nargin << " input arguments and "
                << nargout << " output arguments.\n";
  
  NDArray in = args(0).array_value();
  dim_vector in_dims = in.dims();
  int rows = in_dims.elem(0);
  int cols = in_dims.elem(1);
 
  NDArray out = NDArray(dim_vector(rows, 8));
  
  /* Convert from Octave style to FM SMOOV style */
  float** convbuf = new float*[4];
  float* input = new float[rows*2];
  float* b1 = new float[rows*2];
  float* b2 = new float[rows*2];
  float* b3 = new float[rows*2];
  float* b4 = new float[rows*2];
  
  convbuf[0] = b1;
  convbuf[1] = b2;
  convbuf[2] = b3;
  convbuf[3] = b4;
  
  int convbufpos = 0;
  for(uint32_t i = 0; i < rows; i++)
  {
    input[convbufpos] = in(i,0);
    input[convbufpos+1] = in(i,1);
    convbufpos+=2;
  }
    
  octave_stdout << "input has dimensions " << in_dims.elem(0) << "x" << in_dims.elem(1) << endl;
  
  /*  b1lo_sos =
   1.00000   2.00001   1.00001   1.00000  -1.97978   0.98017
   1.00000   0.99999  -0.00000   1.00000  -0.98017   0.00000
   g1lo =  0.00000098434 
  */
  SOS* b1lo_sos1 = new SOS(0.00000098434, 1.00000, 2.00001, 1.00001, -1.97978, 0.98017);
  SOS* b1lo_sos2 = new SOS(0.00000098434, 1.00000, 0.99999, -0.00000, -0.98017, 0.00000);
  
  /* b1hi_sos =

   1.00000  -2.00001   1.00001   1.00000  -1.97978   0.98017
   1.00000  -0.99999   0.00000   1.00000  -0.98017   0.00000

   g1hi =  0.98017
  */  
  SOS* b1hi_sos1 = new SOS(0.98017, 1.00000, -2.00001, 1.00001, -1.97978, 0.98017);
  SOS* b1hi_sos2 = new SOS(0.98017, 1.00000, -0.99999, 0.00000, -0.98017, 0.00000);
  
  /* b2lo_sos =

   1.00000   2.00001   1.00001   1.00000  -1.88165   0.89364
   1.00000   0.99999  -0.00000   1.00000  -0.89332   0.00000

  g2lo =  0.00015984 
  */
  SOS* b2lo_sos1 = new SOS(0.00015984 , 1.00000, 2.00001, 1.00001, -1.88165, 0.89364);
  SOS* b2lo_sos2 = new SOS(0.00015984 , 1.00000, 0.99999, -0.00000, -0.89332, 0.00000);
  
  /* b2hi_sos =

   1.00000  -2.00001   1.00001   1.00000  -1.88165   0.89364
   1.00000  -0.99999   0.00000   1.00000  -0.89332   0.00000

  g2hi =  0.89348 
  */
  SOS* b2hi_sos1 = new SOS(0.89348, 1.00000, -2.00001, 1.00001, -1.88165, 0.89364);
  SOS* b2hi_sos2 = new SOS(0.89348, 1.00000, -0.99999, 0.00000, -0.89332, 0.00000);
  
  /* b3lo_sos =

   1.00000   2.00001   1.00001   1.00000  -1.23349   0.53945
   1.00000   0.99999  -0.00000   1.00000  -0.50131   0.00000

  g3lo =  0.019072
  */
  SOS* b3lo_sos1 = new SOS(0.019072, 1.00000, 2.00001, 1.00001, -1.23349, 0.53945);
  SOS* b3lo_sos2 = new SOS(0.019072, 1.00000, 0.99999, -0.00000, -0.50131, 0.00000);
  
  /* b3hi_sos =

   1.00000  -2.00001   1.00001   1.00000  -1.23349   0.53945
   1.00000  -0.99999   0.00000   1.00000  -0.50131   0.00000

  g3hi =  0.52038
  */
  SOS* b3hi_sos1 = new SOS(0.52038, 1.00000, -2.00001, 1.00001, -1.23349, 0.53945);
  SOS* b3hi_sos2 = new SOS(0.52038, 1.00000, -0.99999, 0.00000, -0.50131, 0.00000);
  
  Filter* b1lo = new Filter(b1lo_sos1, b1lo_sos2);
  Filter* b1hi = new Filter(b1hi_sos1, b1hi_sos2);
  Filter* b2lo = new Filter(b2lo_sos1, b2lo_sos2);
  Filter* b2hi = new Filter(b2hi_sos1, b2hi_sos2);
  Filter* b3lo = new Filter(b3lo_sos1, b3lo_sos2);
  Filter* b3hi = new Filter(b3hi_sos1, b3hi_sos2);
  
  /* 
   * OK, now to port over my Octave work:  The octave code will be interspersed
   *  with the C++ code
  */
  
//  stage1lo = filter(b1lo,a1lo,invec);
    float* s1lo = new float[rows*2];
    b1lo->process(input, rows*2, s1lo);
//  stage1hi = filter(b1hi,a1hi,invec);
    float* s1hi = new float[rows*2];
    b1hi->process(input, rows*2, s1hi);
//  
//  stage2lo = filter(b2lo,a2lo,stage1hi);
    float* s2lo = new float[rows*2];
    b2lo->process(s1hi, rows*2, s2lo);
//  b2_out = -stage2lo;
    invert(s2lo, rows*2, s2lo);
//  b2_out_apl = filter(b3lo,a3lo,b2_out);
//  b2_out_aph = filter(b3hi,a3hi,b2_out);
//  b2_out = b2_out_apl + b2_out_aph;
    allpass(s2lo, rows*2, b3lo, b3hi, convbuf[1]);

//  stage2hi = filter(b2hi,a2hi,stage1hi);
    float* s2hi = new float[rows*2];
    b2hi->process(s1hi, rows*2, s2hi);
//  stage1loapl = filter(b2lo,a2lo,stage1lo);
//  stage1loaph = filter(b2hi,a2hi,stage1lo);
//  b1_out = stage1loapl + stage1loaph;
    allpass(s1lo, rows*2, b2lo, b2hi, s1lo);
//  b1_out_apl = filter(b3lo,a3lo,b1_out);
//  b1_out_aph = filter(b3hi,a3hi,b1_out);
//  b1_out = b1_out_apl + b1_out_aph;
    allpass(s1lo, rows*2, b2lo, b2hi, convbuf[0]);
//  
//  stage3_hi = filter(b3hi,a3hi,stage2hi);
//  b3_out = filter(b3lo,a3lo,stage2hi);
    float* s3hi = new float[rows*2];
    b3hi->process(s2hi, rows*2, s3hi);
    b3lo->process(s2hi, rows*2, convbuf[2]);
//  
//  b4_out = -stage3_hi;
    invert(s3hi, rows*2, convbuf[3]);
  
  
  
  /* Put it back in Octave-style */
  convbufpos = 0;
  for(uint32_t i = 0; i < rows; i++)
  {
    out(i,0) = convbuf[0][convbufpos];
    out(i,1) = convbuf[0][convbufpos+1];
    out(i,2) = convbuf[1][convbufpos];
    out(i,3) = convbuf[1][convbufpos+1];
    out(i,4) = convbuf[2][convbufpos];
    out(i,5) = convbuf[2][convbufpos+1];
    out(i,6) = convbuf[3][convbufpos];
    out(i,7) = convbuf[3][convbufpos+1];
    convbufpos+=2;
  }
  
  return octave_value(out);
}