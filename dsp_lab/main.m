
#clear all;

pkg load signal;

#fd_raw_audio_in = fopen("/opt/repos/fm_smoov/dsp_lab/samp_audio_in.pcm", "rb");

global Fs;
Fs = 48000;
num_samp_to_capture = 1920000;

#if (fd_raw_audio_in < 0)
#  printf("Error opening raw auffdio input file.\n");
#else
#  raw_audio_in = fread(fd_raw_audio_in, [2,num_samp_to_capture], "float32");

#  raw_audio_in = audioread("/media/sf_vbox_share/white_noise_0db_10s.wav");
  raw_audio_in = audioread("/Users/zaremba/vbox_share/samp_audio_in7.wav");
#  raw_audio_in = raw_audio_in';q

  #Convert inputs to dbFS
  pre_in_AGC_DB = oct_lin2log(raw_audio_in);

  #Input AGC params
  TL = -18.0; #Target level
  GT = -30.0;  #Gain gate threshold
  mode = 'peak';  #peak or RMS
  RMSw = 1.0;   #moving RMS window length in seconds
  Tatt = 0.100;  #attack time constant
  Trel = 0.500;  #release time constant
  
  pre_AGC_out = agc(pre_in_AGC_DB, TL, GT, mode, RMSw, Tatt, Trel, Fs);
  
  #OK, go back to linear...this is a loss of efficiency but I don't know
  #enough math at this point to make the mid/side stereo and filters work in
  #dB
  
  pre_AGC_out_lin = oct_log2lin(pre_AGC_out);
  pre_AGC_out = pre_AGC_out_lin .* raw_audio_in;
  
  stereo_enh_out = oct_stereo_enhance(pre_AGC_out, 1.5);
  bands = multiband_split(stereo_enh_out);
   
  band1 = 0.2 * dynamics_compress([bands(:,1),bands(:,2)], 5.0, -20.0, 2.0, 0.0001, 0.075);
  band2 = 0.2 * dynamics_compress([bands(:,3),bands(:,4)], 4.0, -17.0, 0.0, 0.00001, 0.100);
  band3 = 0.2 * dynamics_compress([bands(:,5),bands(:,6)], 4.0, -25.0, 0.0, 0.00001, 0.100);
  band4 = 0.2 * dynamics_compress([bands(:,7),bands(:,8)], 5.0, -25.0, 2.0, 0.00001, 0.100);
 
  output = (band1 + band2 + band3 + band4);
#  output = pre_AGC_out;
  
# spectrum_analyzer(raw_audio_in, output);
   
#   output = pre_stereo_lin;  
#  audiowrite ("/media/sf_vbox_share/samp_audio_in.wav", raw_audio_in, 48000, 'BitsPerSample', 24);
#  audiowrite ("/Users/zaremba/vbox_share/processed_band1.wav", band1, 48000, 'BitsPerSample', 24);
#  audiowrite ("/Users/zaremba/vbox_share/processed_band2.wav", band2, 48000, 'BitsPerSample', 24);
#  audiowrite ("/Users/zaremba/vbox_share/processed_band3.wav", band3, 48000, 'BitsPerSample', 24);
#  audiowrite ("/Users/zaremba/vbox_share/processed_band4.wav", band4, 48000, 'BitsPerSample', 24);
  audiowrite ("/Users/zaremba/vbox_share/processed.wav", output, 48000, 'BitsPerSample', 24);
  
#endif




 