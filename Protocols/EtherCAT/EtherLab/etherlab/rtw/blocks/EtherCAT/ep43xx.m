function ep43xx(method, varargin)

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
    [ud.SlaveConfig,status] = slave_config(model);
    ud.PortConfig = port_config(ud.SlaveConfig,status);
    set_param(gcbh, 'UserData', ud);

    update_gui

case 'check'
    % If UserData.SlaveConfig does not exist, this is an update
    % Convert this block and return
    model = get_param(gcbh,'model');

    ud = get_param(gcbh, 'UserData');

    % Get slave and port configuration based on product code and revision
    [sc,status] = slave_config(ud.SlaveConfig.product, ud.SlaveConfig.revision);
    pc = port_config(sc,status);

    if isequal(sc.sm, ud.SlaveConfig.sm) && ~isequal(sc, ud.SlaveConfig)
        % The slave has a new name
        warning('ep43xx:NewName', ...
                '%s: Renaming device from %s to %s', ...
                gcb, get_param(gcbh,'model'), sc.description)
        set_param(gcbh, 'model', sc.description)
        return;
    end

    if ~isequal(pc, ud.PortConfig)
        errordlg('Configuration error. Please replace this block', gcb);
        %error('ep43xx:PortConfig', 'Configuration error on %s. Replace it',...
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
function [rv,status] = slave_config(varargin)

% directly write the syncmanager configuration
output_sm       = {2, 0, { {hex2dec('1600') [hex2dec('7020'),17,16]},...
                           {hex2dec('1601') [hex2dec('7030'),17,16]}}};

input_sm        = {3, 1, { {hex2dec('1a01') [hex2dec('6000'),17,16]},...
                           {hex2dec('1a03') [hex2dec('6010'),17,16]}}};

status_input_sm = {3, 1, { {hex2dec('1a00') [hex2dec('6000'), 1, 1;...
                                             hex2dec('6000'), 2, 1;...
                                             hex2dec('6000'), 3, 2;...
                                             hex2dec('6000'), 5, 2;...
                                             hex2dec('6000'), 7, 1;...
                                             0              , 0, 1;...
                                             0              , 0, 5;...
                                             hex2dec('6000'),14, 1;...
                                             hex2dec('6000'),15, 1;...
                                             hex2dec('6000'),16, 1;...
                                             hex2dec('6000'),17,16]},...
                           {hex2dec('1a02') [hex2dec('6010'), 1, 1;...
                                             hex2dec('6010'), 2, 1;...
                                             hex2dec('6010'), 3, 2;...
                                             hex2dec('6010'), 5, 2;...
                                             hex2dec('6010'), 7, 1;...
                                             0              , 0, 1;...
                                             0              , 0, 5;...
                                             hex2dec('6010'),14, 1;...
                                             hex2dec('6010'),15, 1;...
                                             hex2dec('6010'),16, 1;...
                                             hex2dec('6010'),17,16]}}};

%   Model       ProductCode          Revision          IndexOffset, function
models = {...
  'EP4374-0002',  hex2dec('11164052'), hex2dec('00100002'); ...
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
    status = [];
    return
end

status = strcmp(get_param(gcbh,'status'), 'on');

if isempty(product)
    rv = [];
    return;
end

rv.vendor = 2;
rv.description = product{1};
rv.product = product{2};
rv.revision = product{3};

if status
    rv.sm = {output_sm, status_input_sm};
else
    rv.sm = {output_sm, input_sm};
end

switch get_param(gcbh,'dcmode')
case 'DC-Synchron'
    rv.dc = [hex2dec('700'),0,1,0,0,0,0,1,80000,0];
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
    error(['ep43xx:' field], 'Value is not numeric');
end

return

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function rv = port_config(SlaveConfig,status)
% Populate the blocks output port(s)

vector    = strcmp(get_param(gcbh,'vector'), 'on');
pdo_count = numel(SlaveConfig.sm{1}{3});

% Get the value of 'scale'. If this is numeric, use it for input scaling
i_scale  = get_val('i_scale');
o_scale  = get_val('o_scale');
o_offset = get_val('o_offset');
tau      = get_val('tau');

if vector
    rv.input.pdo      = zeros(pdo_count, 4);
    rv.input.pdo(:,2) = 0:pdo_count-1;
    rv.input.pdo_data_type = 2016;

    if isnumeric(i_scale)
        rv.input.full_scale = 2^15;
        if ~isempty(i_scale)
            rv.input.gain     = {'InputGain', i_scale};
        end
    end

    rv.output(1).pdo = zeros(pdo_count,4);
    rv.output(1).pdo(:,1) = ones(pdo_count,1);
    rv.output(1).pdo(:,2) = 0:pdo_count-1;
    rv.output(1).pdo_data_type = 2016;

    if status
        rv.output(1).pdo(:,3) = 10*ones(pdo_count,1);

        rv.output(2).pdo = rv.output(1).pdo;
        rv.output(2).pdo(:,3) = 5*ones(pdo_count,1);
        rv.output(2).pdo_data_type = 1001;
    end

    if isnumeric(o_scale) || isnumeric(o_offset) || isnumeric(tau)
        rv.output(1).full_scale = 2^15;

        if ~isempty(o_scale)
            rv.ouput(1).gain     = {'OutputGain', o_scale};
        end

        if ~isempty(o_offset)
            rv.ouput(1).offset   = {'OutputOffset', o_offset};
        end

        if ~isempty(tau)
            rv.ouput(1).filter   = {'OutputFilter', tau};
        end
    end
else
    rv.input = arrayfun(@(x) struct('pdo', [0 x 0 0], ...
                                    'pdo_data_type', 2016), 0:pdo_count-1);

    if isnumeric(i_scale)
        for i = 1:pdo_count
            rv.input(i).full_scale = 2^15;
        end

        if ~isempty(i_scale)
            if numel(i_scale) == 1
                i_scale = repmat(i_scale(1), pdo_count,1);
            end

            for i = 1:min(numel(i_scale),pdo_count)
                rv.input(i).i_scale = {strcat('InputGain', int2str(i)), i_scale(i)};
            end
        end
    end

    rv.output = arrayfun(@(x) struct('pdo', [1 x 0 0], ...
                                     'pdo_data_type', 2016), 0:pdo_count-1);

    if status
        rv.output = [rv.output, rv.output];
        for i = 1:pdo_count
            rv.output(i).pdo(:,3) = 10;
            rv.output(i+pdo_count).pdo(:,3) = 5;
            rv.output(i+pdo_count).pdo_data_type = 1001;
        end
    end

    if isnumeric(o_scale) || isnumeric(o_offset) || isnumeric(tau)
        for i = 1:pdo_count
            rv.output(i).full_scale = 2^15;
        end

        if ~isempty(o_scale)
            if numel(o_scale) == 1
                o_scale = repmat(o_scale(1), pdo_count,1);
            end

            for i = 1:min(numel(o_scale),pdo_count)
                rv.output(i).o_scale = ...
                        {strcat('OutputGain', int2str(i)), o_scale(i)};
            end
        end

        if ~isempty(o_offset)
            if numel(o_offset) == 1
                o_offset = repmat(o_offset(1), pdo_count,1);
            end

            for i = 1:min(numel(o_offset),pdo_count)
                rv.output(i).o_offset = ...
                        {strcat('OutputOffset', int2str(i)), o_offset(i)};
            end
        end

        if ~isempty(tau)
            if numel(tau) == 1
                tau = repmat(tau(1), pdo_count,1);
            end

            for i = 1:min(numel(tau),pdo_count)
                rv.output(i).filter = ...
                        {strcat('OutputFilter', int2str(i)), tau(i)};
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
