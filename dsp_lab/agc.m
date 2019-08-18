#SlowAGC - input is already in dBFS

function out = agc(in, TL, GT, Tatt, Trel)
  
  global Fs;
  out = oct_agc(in, TL, GT, Tatt, Trel, Fs);
  
  endfunction