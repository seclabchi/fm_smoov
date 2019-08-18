pkg load signal;

Fs = 48000;
  
#crossover frequencies in Hz
fcross = [153, 860, 4900];  

Wn = fcross ./ (Fs/2);

[b1lo, a1lo] = butter(3, Wn(1), 'low');
[b1hi, a1hi] = butter(3, Wn(1), 'high');
[b2lo, a2lo] = butter(3, Wn(2), 'low');
[b2hi, a2hi] = butter(3, Wn(2), 'high');
[b3lo, a3lo] = butter(3, Wn(3), 'low');
[b3hi, a3hi] = butter(3, Wn(3), 'high');

b1lo_sos = tf2sos(b1lo, a1lo)
b1hi_sos = tf2sos(b1hi, a1hi)
b2lo_sos = tf2sos(b2lo, a2lo)
b2hi_sos = tf2sos(b2hi, a2hi)
b3lo_sos = tf2sos(b3lo, a3lo)
b3hi_sos = tf2sos(b3hi, a3hi)
