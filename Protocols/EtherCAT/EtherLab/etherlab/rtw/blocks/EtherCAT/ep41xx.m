function ep41xx(method, varargin)

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
    [ud.SlaveConfig] = slave_config(model);
    ud.PortConfig = port_config(ud.SlaveConfig);
    set_param(gcbh, 'UserData', ud);

    update_gui

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
        warning('ep41xx:NewName', ...
                '%s: Renaming device from %s to %s', ...
                gcb, get_param(gcbh,'model'), sc.description)
        set_param(gcbh, 'model', sc.description)
        return;
    end

    if ~isequal(pc, ud.PortConfig)
        errordlg('Configuration error. Please replace this block', gcb);
        %error('ep41xx:PortConfig', 'Configuration error on %s. Replace it',...
                %gcb);
    end

case 'update'
    update_devices(varargin{1}, slave_config());

case 'ui'
    update_gui;

otherwise
    display([gcb, ': Unknown method ', method])
end

return

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function rv = slave_config(varargin)

% directly write the syncmanager configuration
sm = {...
        {2, 0, { {hex2dec('1600') [hex2dec('7000'), 17, 16]},...
                 {hex2dec('1601') [hex2dec('7010'), 17, 16]},...
                 {hex2dec('1602') [hex2dec('7020'), 17, 16]},...
                 {hex2dec('1603') [hex2dec('7030'), 17, 16]}}}...
};

%   Model       ProductCode          Revision          IndexOffset, function
models = {...
  'EP4174-0002',  hex2dec('104e4052'), hex2dec('00110002'); ...
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
rv.sm = sm;

switch get_param(gcbh,'dcmode')
case 'DC-Synchron'
    rv.dc = [hex2dec('730'),0,1,0,0,0,0,1,140000,0];
case 'DC-Customized'
    rv.dc = evalin('base',get_param(gcbh,'dccustom'));
end

return

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function rv = get_val(field)
rv = get_param(gcbh, field);
if isempty(rv)
    % Nothing was entered for the field. rv now contains the empty string ''
    return
end
rv = evalin('base',rv);

if ~isnumeric(rv)
    rv = [];
    error(['ep41xx:' field], 'Value is not numeric');
end

return

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function rv = port_config(SlaveConfig)
% Populate the blocks output port(s)

vector    = strcmp(get_param(gcbh,'vector'), 'on');
pdo_count = numel(SlaveConfig.sm{1}{3});

% Get the value of 'scale'. If this is numeric, use it for input scaling
gain = get_val('scale');

if vector
    rv.input.pdo      = zeros(pdo_count, 4);
    rv.input.pdo(:,2) = 0:pdo_count-1;
    rv.input.pdo_data_type = 2016;

    if isnumeric(gain)
        rv.input.full_scale = 2^15;
        if ~isempty(gain)
            rv.input.gain     = {'Gain', gain};
        end
    end
else
    rv.input = arrayfun(@(x) struct('pdo', [0 x 0 0], ...
                                    'pdo_data_type', 2016), 0:pdo_count-1);

    if isnumeric(gain)
        for i = 1:pdo_count
            rv.input(i).full_scale = 2^15;
        end

        if ~isempty(gain)
            if numel(gain) == 1
                gain = repmat(gain(1), pdo_count,1);
            end

            for i = 1:min(numel(gain),pdo_count)
                rv.input(i).gain = {strcat('Gain', int2str(i)), gain(i)};
            end
        end
    end
end

return

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function update_gui
mask_enables = cell2struct(...
    get_param(gcbh,'MaskEnables'),...
    get_param(gcbh,'MaskNames')...
);

choice = {'off','on'};
value = choice{strcmp(get_param(gcbh,'dcmode'),'DC-Customized') + 1};

if ~strcmp(mask_enables.dccustom,value)
    mask_enables.dccustom = value;
    set_param(gcbh,'MaskEnables',struct2cell(mask_enables));
end
