#include <octave/oct.h>

using namespace std;

DEFUN_DLD (oct_log2lin, args, nargout,
           "log2lin Help String")
{
  int nargin = args.length();

  octave_stdout << "oct_log2lin has "
                << nargin << " input arguments and "
                << nargout << " output arguments.\n";
  
  NDArray in = args(0).array_value();
  dim_vector in_dims = in.dims();
  int rows = in_dims.elem(0);
  int cols = in_dims.elem(1);
  double samp = 0.0;
  
  NDArray out = NDArray(in_dims);
 
  octave_stdout << "input has dimensions " << in_dims.elem(0) << "x" << in_dims.elem(1) << endl;
    
  for(int col = 0; col < cols; col++)
  {
    for(int row = 0; row < rows; row++)
    {
      samp = in(row,col);
      out(row,col) = powf(10.0, samp/20.0);
    }
  }
  
  return octave_value(out);
}