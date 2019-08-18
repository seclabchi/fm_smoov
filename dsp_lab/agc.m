#SlowAGC - input is already in dBFS

function out = agc(in, TL, GT, mode, RMSw, Tatt, Trel)
  
  global Fs;
  out = oct_agc(in, TL, GT, mode, RMSw, Tatt, Trel, Fs);
  
  endfunction