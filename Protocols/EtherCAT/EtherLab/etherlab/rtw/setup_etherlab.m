function setup_etherlab()

if ~length(strfind(path,pwd))
    disp(['Adding ' pwd ' to $MATLABPATH']);
    addpath(pwd);
    addpath([pwd '/etherlab']);
else
    disp('EtherLAB search directories are already in $MATLABPATH')
end

run blocks/setup.m

try
    savepath;
catch
    disp(['ERROR: Could not save MATLABPATH. Probably ' ...
	'you cannot write the file. Check that ' ...
	'you have write permissions for '...
	'$MATLABPATH/toolbox/local/pathdef.m']);
end


return
