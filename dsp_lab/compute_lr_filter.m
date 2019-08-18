function compute_lr_filter(fcross, type)
  global Fs;
  
  Wn = fcross/(Fs/2);
  
  [b,a] = butter(2, Wn, type);
  
  
  endfunction