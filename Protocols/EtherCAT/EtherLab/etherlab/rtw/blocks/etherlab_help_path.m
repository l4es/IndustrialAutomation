function rv = etherlab_help_path(page)
% returns the help page for etherlab_lib

rv = fullfile(fileparts(which('etherlab_lib')),'html',page);
