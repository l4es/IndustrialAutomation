function el31xx(method, varargin)

if ~nargin
    return
end

%display([gcb ' ' method]);

switch lower(method)
case 'set'
    model = get_param(gcbh, 'model');

    if model(1) ~= 'E'
        errordlg('Please choose a correct slave', gcb);
        return
    end

    ud = get_param(gcbh,'UserData');
    ud.SlaveConfig = slave_config(model);
    ud.PortConfig = port_config(ud.SlaveConfig);
    set_param(gcbh, 'UserData', ud);

case 'check'
    % If UserData.SlaveConfig does not exist, this is an update
    % Convert this block and return
    model = get_param(gcbh,'model');

    ud = get_param(gcbh, 'UserData');

    % Get slave and port configuration based on product code and revision
    sc = slave_config(ud.SlaveConfig.product, ud.SlaveConfig.revision);
    pc = port_config(sc);

    if isequal(sc.sm, ud.SlaveConfig.sm) && ~isequal(sc, ud.SlaveConfig)
        % The slave has a new name
        warning('el31xx:NewName', ...
                '%s: Renaming device from %s to %s', ...
                gcb, get_param(gcbh,'model'), sc.description)
        set_param(gcbh, 'model', sc.description)
        return;
    end

    if ~isequal(pc, ud.PortConfig)
        errordlg('Configuration error. Please replace this block', gcb);
        %error('el31xx:PortConfig', 'Configuration error on %s. Replace it',...
                %gcb);
    end

case 'update'
    update_devices(varargin{1}, slave_config());

otherwise
    display([gcb, ': Unknown method ', method])
end

return

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function rv = slave_config(varargin)

%      PdoIdx            PdoEntry
% Note: the PdoIdx field will be incremented by IndexOffset (column 3)
%       from the models struct below
pdo = [...
        hex2dec('1a00'), hex2dec('6000'); ...
        hex2dec('1a02'), hex2dec('6010'); ...
        hex2dec('1a04'), hex2dec('6020'); ...
        hex2dec('1a06'), hex2dec('6040'); ...
        ];

%   Model       ProductCode          Revision          IndexOffset, function
models = {...
  'EL3101', hex2dec('0c1d3052'), hex2dec('00100000'), 1, '+-10V' ; ...
  'EL3102', hex2dec('0c1e3052'), hex2dec('00110000'), 3, '+-10V' ; ...
  'EL3104', hex2dec('0c203052'), hex2dec('00100000'), 1, '+-10V' ; ...
  'EL3111', hex2dec('0c273052'), hex2dec('00100000'), 1, '0-20mA'; ...
  'EL3112', hex2dec('0c283052'), hex2dec('00110000'), 3, '0-20mA'; ...
  'EL3114', hex2dec('0c2a3052'), hex2dec('00110000'), 1, '0-20mA'; ...
  'EL3121', hex2dec('0c313052'), hex2dec('00100000'), 1, '4-20mA'; ...
  'EL3122', hex2dec('0c323052'), hex2dec('00110000'), 3, '4-20mA'; ...
  'EL3124', hex2dec('0c343052'), hex2dec('00100000'), 1, '4-20mA'; ...
  'EL3141', hex2dec('0c453052'), hex2dec('00100000'), 1, '0-20mA'; ...
  'EL3142', hex2dec('0c463052'), hex2dec('00110000'), 3, '0-20mA'; ...
  'EL3142-010', hex2dec('0c463052'), hex2dec('0011000A'), 3, '+-10mA'; ...
  'EL3144', hex2dec('0c483052'), hex2dec('00100000'), 1, '0-20mA'; ...
  'EL3151', hex2dec('0c4f3052'), hex2dec('00100000'), 1, '4-20mA'; ...
  'EL3152', hex2dec('0c503052'), hex2dec('00110000'), 3, '4-20mA'; ...
  'EL3154', hex2dec('0c523052'), hex2dec('00100000'), 1, '4-20mA'; ...
  'EL3161', hex2dec('0c593052'), hex2dec('00100000'), 1, '0-10V' ; ...
  'EL3162', hex2dec('0c5a3052'), hex2dec('00110000'), 3, '0-10V' ; ...
  'EL3164', hex2dec('0c5c3052'), hex2dec('00100000'), 1, '0-10V' ; ...
};

dc = {'Free Run', 0, 0, 0, 0, 0, 0, 0, 1, 0, 0;...
        'DC-Synchron',  hex2dec('700'), 0, 1, 0, 0, 0, 0, 1, 5000, 0;...
        'DC-Synchron (input based)',...
                        hex2dec('700'), 0, 1, 0, 0, 1, 0, 1, 5000, 0;...
};

switch nargin
case 2
    pos = cell2mat(models(:,2)) == varargin{1}...
        & cell2mat(models(:,3)) == varargin{2};
    product = models(pos,:);

case 1
    product = models(strcmp(models(:,1),varargin{1}),:);

otherwise
    fields = models(:,1);
    obsolete = cellfun(@length, fields) > 11;
    rv = vertcat(sort(fields(~obsolete)), sort(fields(obsolete)));
    return
end

if isempty(product)
    rv = [];
    return;
end

rv.vendor = 2;
rv.description = product{1};
rv.product = product{2};
rv.revision = product{3};
rv.function = product{5};

% Find out which rows of pdo matrix we need
rows = 1:str2double(rv.description(6));

% Populate the PDO structure
rv.sm = {{3, 1, {}}};
rv.sm{1}{3} = arrayfun(...
    @(x) {pdo(x,1) + product{4}, [pdo(x,2), 17, 16]},...
    rows, 'UniformOutput', 0);

dc_idx = find(strcmp(get_param(gcbh,'dc'), dc(:,1)));
if (~isempty(dc_idx) && dc_idx > 1)
    rv.dc = cell2mat(dc(dc_idx, 2:end));
end

return

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function rv = get_val(field)
str = get_param(gcbh, field);
rv = [];
if isempty(str)
    return
end
rv = evalin('base',str);
if ~isnumeric(rv)
    rv = [];
    error(['el31xx:',field], 'Value is not numeric');
end

return

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function rv = port_config(SlaveConfig)
% Populate the blocks output port(s)

vector  = strcmp(get_param(gcbh,'vector'), 'on');
pdo_count = numel(SlaveConfig.sm{1}{3});
raw = strcmp(get_param(gcbh,'raw'),'on');

gain   = get_val('scale');
offset = get_val('offset');
tau    = get_val('tau');

if (vector)
    rv.output.pdo      = zeros(pdo_count, 4);
    rv.output.pdo_data_type = 2016;
    rv.output.pdo(:,2) = 0:pdo_count-1;

    if ~raw
        rv.output.full_scale = 2^15;

        if ~isempty(gain)
            rv.output.gain = {'Gain', gain};
        end
        if ~isempty(offset)
            rv.output.offset = {'Offset', offset};
        end
        if ~isempty(tau)
            rv.output.tau = {'Filter', tau};
        end
    end

else

    rv.output  = repmat(struct('pdo',[0 0 0 0], 'pdo_data_type', 2016), 1, pdo_count);

    if ~raw
        if (numel(gain) == 1)
            gain = repmat(gain,1,pdo_count);
        end
        if (numel(offset) == 1)
            offset = repmat(offset,1,pdo_count);
        end
        if (numel(tau) == 1)
            tau = repmat(tau,1,pdo_count);
        end
    end

    for i = 1:pdo_count
        rv.output(i).pdo(2)  = i-1;

        if ~raw

            rv.output(i).full_scale = 2^15;

            if i <= numel(gain)
                rv.output(i).gain = {['Gain', int2str(i)], gain(i)};
            end
            if i <= numel(offset)
                rv.output(i).offset = {['Offset', int2str(i)], offset(i)};
            end
            if i <= numel(tau)
                rv.output(i).tau = {['Filter', int2str(i)], tau(i)};
            end
        end
    end
end

return
