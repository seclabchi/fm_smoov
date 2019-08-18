clear all;

pkg load signal;

global Fs = 48000;  #Hz

#The original Texar Audio Prism crossover points, according to 
#drs2006.com

Fcross = [64, 362, 2040, 11700]; #Hz

compute_lr_filter(Fcross(1), 'low');
