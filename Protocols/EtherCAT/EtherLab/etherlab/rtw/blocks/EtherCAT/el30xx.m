function el30xx(method, varargin)

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
        warning('el30xx:NewName', ...
                '%s: Renaming device from %s to %s', ...
                gcb, get_param(gcbh,'model'), sc.description)
        set_param(gcbh, 'model', sc.description)
        return;
    end

    if ~isequal(pc, ud.PortConfig)
        errordlg('Configuration error. Please replace this block', gcb);
        %error('el30xx:PortConfig', 'Configuration error on %s. Replace it',...
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

SmIdx = 3;
PdoSubIdx = 17;

%      PdoIdx          EntryIdx
pdo = [...
        hex2dec('1a01'), hex2dec('6000');...
        hex2dec('1a03'), hex2dec('6010');...
        hex2dec('1a05'), hex2dec('6020');...
        hex2dec('1a07'), hex2dec('6030');...
        hex2dec('1a09'), hex2dec('6040');...
        hex2dec('1a0b'), hex2dec('6050');...
        hex2dec('1a0d'), hex2dec('6060');...
        hex2dec('1a0f'), hex2dec('6070');...
        ];

%   Model       ProductCode          Revision          N, Type
models = {...
  'EL3001', hex2dec('0bb93052'), hex2dec('00120000'), '+-10V' ; ...
  'EL3002', hex2dec('0bba3052'), hex2dec('00120000'), '+-10V' ; ...
  'EL3004', hex2dec('0bbc3052'), hex2dec('00120000'), '+-10V' ; ...
  'EL3008', hex2dec('0bc03052'), hex2dec('00120000'), '+-10V' ; ...
  'EL3011', hex2dec('0bc33052'), hex2dec('00100000'), '0-20mA'; ...
  'EL3012', hex2dec('0bc43052'), hex2dec('00100000'), '0-20mA'; ...
  'EL3014', hex2dec('0bc63052'), hex2dec('00100000'), '0-20mA'; ...
  'EL3021', hex2dec('0bcd3052'), hex2dec('00100000'), '4-20mA'; ...
  'EL3022', hex2dec('0bce3052'), hex2dec('00100000'), '4-20mA'; ...
  'EL3024', hex2dec('0bd03052'), hex2dec('00100000'), '4-20mA'; ...
  'EL3041', hex2dec('0be13052'), hex2dec('00110000'), '0-20mA'; ...
  'EL3042', hex2dec('0be23052'), hex2dec('00110000'), '0-20mA'; ...
  'EL3044', hex2dec('0be43052'), hex2dec('00110000'), '0-20mA'; ...
  'EL3048', hex2dec('0be83052'), hex2dec('00110000'), '0-20mA'; ...
  'EL3051', hex2dec('0beb3052'), hex2dec('00110000'), '4-20mA'; ...
  'EL3052', hex2dec('0bec3052'), hex2dec('00110000'), '4-20mA'; ...
  'EL3054', hex2dec('0bee3052'), hex2dec('00110000'), '4-20mA'; ...
  'EL3058', hex2dec('0bf23052'), hex2dec('00110000'), '4-20mA'; ...
  'EL3061', hex2dec('0bf53052'), hex2dec('00110000'), '0-10V' ; ...
  'EL3062', hex2dec('0bf63052'), hex2dec('00110000'), '0-10V' ; ...
  'EL3062-0030', hex2dec('0bf63052'), hex2dec('0011001e'), '0-30V'; ...
  'EL3064', hex2dec('0bf83052'), hex2dec('00110000'), '0-10V' ; ...
  'EL3068', hex2dec('0bfc3052'), hex2dec('00110000'), '0-10V' ; ...
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
rv.function = product{4};

% Find out which rows of pdo matrix we need
rows = 1:str2double(rv.description(6));

% Populate the PDO structure
rv.sm = {{SmIdx, 1, {}}};
rv.sm{1}{3} = arrayfun(...
    @(x) {pdo(x,1), [pdo(x,2), PdoSubIdx, 16]}, rows, 'UniformOutput', 0);

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
    error(['el320x:', field], 'Value is not numeric');
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
