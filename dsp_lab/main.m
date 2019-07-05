fd_raw_audio_in = fopen("/opt/repos/rpi_cross/fmsmoov/dsp_lab/samp_audio_in.pcm", "rb");

num_samp_to_capture = 1920000;

if (fd_raw_audio_in < 0)
  printf("Error opening raw audio input file.\n");
else
  raw_audio_in = fread(fd_raw_audio_in, [2,num_samp_to_capture], "float32");
  raw_audio_in = raw_audio_in';
  
  numcols = columns(raw_audio_in);
  numrows = rows(raw_audio_in);
  
  i = 1;
  
  sig_pwr = 1;
  output = zeros(num_samp_to_capture, 2);
  z = 0;
  e = 0;
  g = 0;
  g_prev = 0;
  
  A = 0.9;
  K = 0.3;
  
  out_l = 0;
  tmp_pwr = 0;
  
  do
    tmp_pwr = tmp_pwr + raw_audio_in(i,1)^2;
    
    if(mod(i, 4800) == 0)
      sig_pwr = tmp_pwr / 4800;
      tmp_pwr = 0;
    endif
    
    z = sig_pwr * exp(2 * g_prev);
    e = A - log(z);
    g = g_prev + K * e;
    g_prev = g;
    out_l = z * raw_audio_in(i,1);
    output(i,1) = out_l;
    output(i,2) = raw_audio_in(i,1);
    i = i + 1;
  until(i > numrows);
  
  #plot(output)
  
  audiowrite ("/media/sf_vbox_share/samp_audio_out.wav", output, 48000, 'BitsPerSample', 24);
  
endif




 