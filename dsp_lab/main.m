
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
  raw_audio_in = audioread("/media/sf_vbox_share/samp_audio_in5.wav");
#  raw_audio_in = raw_audio_in';q

  #Convert inputs to dbFS
  inDB = oct_lin2log(raw_audio_in);

  #Input AGC params
  TL = -12.0; #Target level
  GT = -50.0;  #Gain gate threshold
  Tatt = 0.100;  #attack time constant
  Trel = 0.500;  #release time constant
  
  chain = agc(inDB, TL, GT, Tatt, Trel, Fs);
  
  #OK, go back to linear...this is a loss of efficiency but I don't know
  #enough math at this point to make the mid/side stereo and filters work in
  #dB
  
  chain = oct_log2lin(chain);
  chain = chain .* raw_audio_in;
  
#  chain = oct_stereo_enhance(chain, 1.5);
  chain = multiband_split(chain);
   
  chain1 = 0.3 * dynamics_compress([chain(:,1),chain(:,2)], 8.0, -45.0, 0.000100, 0.350);
  chain2 = 0.2 * dynamics_compress([chain(:,3),chain(:,4)], 8.0, -35.0, 0.000200, 0.300);
  chain3 = 0.2 * dynamics_compress([chain(:,5),chain(:,6)], 10.0, -35.0, 0.000010, 0.250);
  chain4 = 0.2 * dynamics_compress([chain(:,7),chain(:,8)], 10.0, -35.0, 0.000001, 0.100);
 
  output = (chain1 + chain2 + chain3 + chain4);
  
# spectrum_analyzer(raw_audio_in, output);
   
#   output = pre_stereo_lin;  
#  audiowrite ("/media/sf_vbox_share/samp_audio_in.wav", raw_audio_in, 48000, 'BitsPerSample', 24);
  audiowrite ("/media/sf_vbox_share/processed_band1.wav", chain1, 48000, 'BitsPerSample', 24);
  audiowrite ("/media/sf_vbox_share/processed_band2.wav", chain2, 48000, 'BitsPerSample', 24);
  audiowrite ("/media/sf_vbox_share/processed_band3.wav", chain3, 48000, 'BitsPerSample', 24);
  audiowrite ("/media/sf_vbox_share/processed_band4.wav", chain4, 48000, 'BitsPerSample', 24);
  audiowrite ("/media/sf_vbox_share/processed.wav", output, 48000, 'BitsPerSample', 24);
  
#endif




 
