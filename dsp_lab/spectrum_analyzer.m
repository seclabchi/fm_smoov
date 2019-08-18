function spectrum_analyzer(raw_audio_in, output)
  global Fs;
  
  Li = rows(raw_audio_in);
  infft = fft(raw_audio_in);
  Pi2 = abs(infft/Li);
  Pi1 = Pi2(1:Li/2+1);
  Pi1(2:end-1) = 2*Pi1(2:end-1);
  
  L = rows(output);
  outfft = fft(output);
  P2 = abs(outfft/L);
  P1 = P2(1:L/2+1);
  P1(2:end-1) = 2*P1(2:end-1);
  
  f = Fs*(0:(L/2))/L;
  subplot(2,1,1);
  plot(f,Pi1) 
  title('Single-Sided Amplitude Spectrum of X(t)')
  xlabel('f (Hz)')
  ylabel('|P1(f)|')
  
  subplot(2,1,2);
  plot(f,P1)
  
  endfunction