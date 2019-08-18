#include <octave/oct.h>

using namespace std;

DEFUN_DLD (oct_stereo_enhance, args, nargout,
           "Stereo Enhancement Help String")
{
  int nargin = args.length();

  octave_stdout << "oct_stereo_enhance has "
                << nargin << " input arguments and "
                << nargout << " output arguments.\n";
  
  NDArray in = args(0).array_value();
  dim_vector in_dims = in.dims();
  
  double ses = args(1).double_value();
  
  NDArray out = NDArray(in_dims);
  
  octave_stdout << "input has dimensions " << in_dims.elem(0) << "x" << in_dims.elem(1) << endl;
  octave_stdout << "Stereo enhance factor: " << ses << endl;
  
  float M, S;
  float l,r;

  for(int col = 0; col < in_dims.elem(1); col+=2)
  {
    for(int row = 0; row < in_dims.elem(0); row++)
    {
        l = in(row,col);
        r = in(row, col+1);
      
        M = ses * (l + r) / 2.0;
        S = ses * (l - r) / 2.0;

        out(row,col) = 1/ses * (M + S);
        out(row,col+1) = 1/ses * (M - S);
    }
  }
  
  return octave_value(out);
}