

function out = dynamics_compress(in, R, T, G, Tatt, Trel)
  global Fs;
  
  alphaA = exp(-log10(9)/(Fs * Tatt));
  alphaR = exp(-log10(9)/(Fs * Trel));
  
  in_db = 20 * log10(abs(in));
  Gsc = oct_sc(in_db, R, T);
  Ggs = oct_gs(Gsc, alphaA, alphaR);
  
  M = -((T + (-T/R)));
  
  GM = Ggs .+ M;
  GM = GM .+ G;
  
  Glin = oct_log2lin(GM);
  
  out = in .* Glin;
      
endfunction
