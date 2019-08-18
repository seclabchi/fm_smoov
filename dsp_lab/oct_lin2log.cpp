#include <octave/oct.h>

using namespace std;

DEFUN_DLD (oct_lin2log, args, nargout,
           "lin2log Help String")
{
  int nargin = args.length();

  octave_stdout << "oct_lin2log has "
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
      
      if(0.0 == samp)
      {
        out(row,col) = -100.0;
      }
      else
      {
        out(row,col) = 20 * log10f(fabs(samp));
      }
    }
  }
  
  return octave_value(out);
}