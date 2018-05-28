function setup()

if ~length(strfind(path,pwd))
    disp(['Adding ' pwd ' to $MATLABPATH']);
    addpath(pwd);
end

disp(['Precompiling functions in ' pwd]);
mex master_state.c
mex domain_state.c
mex ec_slave3.c

return
