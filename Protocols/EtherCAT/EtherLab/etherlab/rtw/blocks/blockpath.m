function name = blockpath(path)
%% Strip leading <modelname> from path

[forget, name] = strtok(path,'/');
