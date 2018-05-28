function out = filesep()
  if ~MSDOS then
    out='/';
  else
    out='\';
  end;
endfunction