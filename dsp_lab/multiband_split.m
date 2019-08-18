function out = multiband_split(invec)
  global Fs;
  
  #crossover frequencies in Hz
  fcross = [153, 860, 4900];  
  
  Wn = fcross ./ (Fs/2);
  
  [b1lo, a1lo] = butter(3, Wn(1), 'low');
  [b1hi, a1hi] = butter(3, Wn(1), 'high');
  [b2lo, a2lo] = butter(3, Wn(2), 'low');
  [b2hi, a2hi] = butter(3, Wn(2), 'high');
  [b3lo, a3lo] = butter(3, Wn(3), 'low');
  [b3hi, a3hi] = butter(3, Wn(3), 'high');

  stage1lo = filter(b1lo,a1lo,invec);
  stage1hi = filter(b1hi,a1hi,invec);
  
  stage2lo = filter(b2lo,a2lo,stage1hi);
  b2_out = -stage2lo;
  b2_out_apl = filter(b3lo,a3lo,b2_out);
  b2_out_aph = filter(b3hi,a3hi,b2_out);
  b2_out = b2_out_apl + b2_out_aph;
  
  stage2hi = filter(b2hi,a2hi,stage1hi);
  
  stage1loapl = filter(b2lo,a2lo,stage1lo);
  stage1loaph = filter(b2hi,a2hi,stage1lo);
  b1_out = stage1loapl + stage1loaph;
  b1_out_apl = filter(b3lo,a3lo,b1_out);
  b1_out_aph = filter(b3hi,a3hi,b1_out);
  b1_out = b1_out_apl + b1_out_aph;
  
  stage3_hi = filter(b3hi,a3hi,stage2hi);
  b3_out = filter(b3lo,a3lo,stage2hi);
  
  b4_out = -stage3_hi;
  
  
  
#  H1lo = tf(b1lo,a1lo);
#  H1hi = tf(b1hi,a1hi);
#  H2lo = tf(b2lo,a2lo);
#  H2hi = tf(b2hi,a2hi);
#  H3lo = tf(b3lo,a3lo);
#  H3hi = tf(b3hi,a3hi);
  
#  bode(H1lo, {.1, 10000})
#  bode(H1hi, {.1, 10000})
#  bode(H2lo, {.1, 10000})
#  bode(H2hi, {.1, 10000})
#  bode(H3lo, {.1, 10000})
#  bode(H3hi, {.1, 10000})

  out = [b1_out, b2_out, b3_out, b4_out];
  
  endfunction