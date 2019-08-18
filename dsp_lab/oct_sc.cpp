#include <octave/oct.h>

using namespace std;

DEFUN_DLD (oct_sc, args, nargout,
           "Gain Static Characteristics Help String")
{
  int nargin = args.length();

  octave_stdout << "oct_sc has "
                << nargin << " input arguments and "
                << nargout << " output arguments.\n";
  
  NDArray in = args(0).array_value ();
  dim_vector in_dims = in.dims();
  double R = args(1).double_value ();
  double T = args(2).double_value ();
  
  NDArray out = NDArray(in_dims);
  
  octave_stdout << "input has dimensions " << in_dims.elem(0) << "x" << in_dims.elem(1) << endl;
  octave_stdout << "Compression ratio: " << R << ", Threshold " << T << endl;
  
  double xdb = 0.0;
  
  for(int col = 0; col < in_dims.elem(1); col++)
  {
    for(int row = 0; row < in_dims.elem(0); row++)
    {
      xdb = in(row,col);
    
      //calculate static characteristic
      if(xdb < T)
      {
        out(row,col) = 0;
      }
      else
      {
        out(row,col) = T + ((xdb - T)/R) - xdb;
      }
    }
  }
  
  return octave_value(out);
}