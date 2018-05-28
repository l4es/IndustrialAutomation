function setup()

if ~length(strfind(path,pwd))
    disp(['Adding ' pwd ' to $MATLABPATH']);
    addpath(pwd);
end

disp(['Precompiling functions in ' pwd]);
mex world_time.c
mex raise.c
mex rtipc_tx.c
mex rtipc_rx.c
mex event.c
mex findidx.c
mex message.c

run EtherCAT/setup.m

return
