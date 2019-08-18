#include <octave/oct.h>

using namespace std;

DEFUN_DLD (oct_gs, args, nargout,
           "Gain Smoothing Help String")
{
  int nargin = args.length();

  octave_stdout << "oct_gs has "
                << nargin << " input arguments and "
                << nargout << " output arguments.\n";
  
  NDArray in = args(0).array_value ();
  dim_vector in_dims = in.dims();
  double alphaA = args(1).double_value ();
  double alphaR = args(2).double_value ();
  
  NDArray out = NDArray(in_dims);
  
  octave_stdout << "input has dimensions " << in_dims.elem(0) << "x" << in_dims.elem(1) << endl;
  octave_stdout << "Attack alpha " << alphaA << ", release alpha " << alphaR << endl;
  
  double gc = 0.0;
  double gs = 0.0;
  double gsPrev = 0.0;
  
  for(int col = 0; col < in_dims.elem(1); col++)
  {
    for(int row = 0; row < in_dims.elem(0); row++)
    {
      gc = in(row,col);
    
      //calculate smoothing
      if(gc <= gsPrev)
      {
        gs = alphaA*gsPrev + (1-alphaA)*gc;
      }
      else
      {
        gs = alphaR*gsPrev + (1-alphaR)*gc;
      }
      
      out(row,col) = gs;
      gsPrev = gs;
      
    }
  }
  
  return octave_value(out);
}