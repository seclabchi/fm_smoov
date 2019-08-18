#include <octave/oct.h>

using namespace std;

DEFUN_DLD (oct_agc, args, nargout,
           "AGC Help String")
{
  int nargin = args.length();

  octave_stdout << "oct_agc has "
                << nargin << " input arguments and "
                << nargout << " output arguments.\n";
  
  NDArray in = args(0).array_value();
  dim_vector in_dims = in.dims();
  int rows = in_dims.elem(0);
  int cols = in_dims.elem(1);
  double TL = args(1).double_value();
  double GT = args(2).double_value();
  string mode = args(3).string_value();
  double RMSw = args(4).double_value();
  double Tatt = args(5).double_value();
  double Trel = args(6).double_value();
  double Fs = args(7).double_value();
    
  int rmswinlen = (int)(Fs/RMSw);
  
  octave_stdout << "RMS window length of " << RMSw << " seconds = " << rmswinlen << " samples." << endl;
    
  NDArray out = NDArray(in_dims);
  double alphaA = exp(-log10(9)/(Fs * Tatt));
  double alphaR = exp(-log10(9)/(Fs * Trel));
  
  octave_stdout << "input has dimensions " << in_dims.elem(0) << "x" << in_dims.elem(1) << endl;
  octave_stdout << "Attack time " << Tatt << ", release time " << Trel << endl;
  octave_stdout << "Gain gate " << GT << ", target level " << TL << endl;
  
  double insampDB = 0.0;
  double gc = 0.0;
  double gs = 0.0;
  double gsPrev = 0.0;
  
  double N = 0.0;  
  double RMSinst = 0.0;
      
  for(int col = 0; col < cols; col++)
  {
    gsPrev = 0.0;
    gs = 0.0;
    
    for(int row = 0; row < rows; row++)
    {
      insampDB = in(row,col);
   
      if(insampDB > GT)
      {
        gc = TL-insampDB;
        
        //calculate smoothing
        if(gc <= gsPrev)
        {
          gs = alphaA*gsPrev + (1-alphaA)*gc;
        }
        else
        {
          gs = alphaR*gsPrev + (1-alphaR)*gc;
        }
      }
      
      out(row,col) = gs;
      gsPrev = gs;
   
    }
  }
  
  return octave_value(out);
}