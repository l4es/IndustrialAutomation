function switch_etherlab(el_path)
% FUNCTION Switch to new EtherlabPath
%   Replaces all paths containing 'etherlab' with new path
%   The new path is not saved.
%   
%   switch_etherlab('/new/etherlab/path')

p = strjoin({el_path; 'rtw';'etherlab';'Contents.m'}, filesep);
try
    run(p);
catch
    error('%s is not a valid EtherLab path', el_path);
    return
end

        
% Split the path into a cell array
p = cellstr(char(java.lang.String(path).split(':')));

% Find all strings that contain etherlab
v = find(cell2mat(cellfun(@(x) ~isempty(x), ...
            strfind(p,'etherlab'),'uniformoutput',0)'));
        
rmpath(p{v});
addpath(strjoin({el_path; 'rtw'; 'etherlab'}, filesep));
addpath(strjoin({el_path; 'rtw'; 'blocks'}, filesep));
addpath(strjoin({el_path; 'rtw'; 'blocks'; 'EtherCAT'}, filesep));

return

%%
function s = strjoin(x,delim)

% Delimiter cell array aufbauen
b = repmat({delim},size(x));
b{numel(b)} = ''; % Leztes element leeren

s = cell2mat(strcat(x,b)');
