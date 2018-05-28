function el5101_0010(method, varargin)

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
        warning('el5101:NewName', ...
                '%s: Renaming device from %s to %s', ...
                gcb, get_param(gcbh,'model'), sc.description)
        set_param(gcbh, 'model', sc.description)
        return;
    end

    if ~isequal(pc, ud.PortConfig)
        errordlg('Configuration error. Please replace this block', gcb);
        %error('el5101:PortConfig', 'Configuration error on %s. Replace it',...
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
function rv = pdo_config(arg,varargin)

% return a cell array with all pdo's
% each cell contains: {PdoIndex, Excludes, PdoEntryArray}
pdo = {...
        hex2dec('1600'),...
           hex2dec('1601'), ...
           [ hex2dec('7000'), 1, 1;
             hex2dec('7000'), 2, 1;
             hex2dec('7000'), 3, 1;
             hex2dec('7000'), 4, 1;
             0              , 0, 4;
             0              , 0, 8;
             hex2dec('7000'),17,32],...
           { 0:3, 6 };
        hex2dec('1601'),...
           hex2dec('1600'), ...
           [ hex2dec('7000'), 1, 1;
             hex2dec('7000'), 2, 1;
             hex2dec('7000'), 3, 1;
             hex2dec('7000'), 4, 1;
             0              , 0, 4;
             0              , 0, 8;
             hex2dec('7000'),17,16],...
           { 0:3, 6 };
        hex2dec('1a00'),...
           hex2dec('1a01'),...
           [ hex2dec('6000'), 1, 1;
             hex2dec('6000'), 2, 1;
             hex2dec('6000'), 3, 1;
             0              , 0, 2;
             hex2dec('6000'), 6, 1;
             hex2dec('6000'), 7, 1;
             0              , 0, 1;
             hex2dec('6000'), 9, 1;
             hex2dec('6000'),10, 1;
             hex2dec('6000'),11, 1;
             hex2dec('6000'),12, 1;
             hex2dec('6000'),13, 1;
             hex2dec('1c32'),32, 1;
             hex2dec('1800'), 7, 1;
             hex2dec('1800'), 9, 1;
             hex2dec('6000'),17,32;
             hex2dec('6000'),18,32],...
           { [0:2,4,5,7:11], 15, 16 };
        hex2dec('1a01'),...
           hex2dec('1a00'),...
           [ hex2dec('6000'), 1, 1;
             hex2dec('6000'), 2, 1;
             hex2dec('6000'), 3, 1;
             0              , 0, 2;
             hex2dec('6000'), 6, 1;
             hex2dec('6000'), 7, 1;
             0              , 0, 1;
             hex2dec('6000'), 9, 1;
             hex2dec('6000'),10, 1;
             hex2dec('6000'),11, 1;
             hex2dec('6000'),12, 1;
             hex2dec('6000'),13, 1;
             hex2dec('1c32'),32, 1;
             hex2dec('1801'), 7, 1;
             hex2dec('1801'), 9, 1;
             hex2dec('6000'),17,16;
             hex2dec('6000'),18,16],...
           { [0:2,4,5,7:11], 15, 16 };
        hex2dec('1a02'),...
           hex2dec('1a03'),...
           [ hex2dec('6000'),20,32],...
           { 0 };
        hex2dec('1a03'),...
           hex2dec('1a02'),...
           [ hex2dec('6000'),19,32],...
           { 0 };
        hex2dec('1a04'),...
           hex2dec('1a05'),...
           [ hex2dec('6000'),22,64],...
           { 0 };
        hex2dec('1a05'),...
           hex2dec('1a04'),...
           [ hex2dec('6000'),22,64],...
           { 0 };
};

index = [pdo{:,1}];
selected = arrayfun(@(x) strcmp(get_param(gcbh,['x',dec2hex(x)]),'on'), index);

switch arg
case {'TxPdo', 'RxPdo'}
    % Return a cell vector {PdoIndex [PdoEntries]} of all selected pdos
    selected = selected & (  arg(1) == 'T' & index >= hex2dec('1a00') ...
                           | arg(1) == 'R' & index <  hex2dec('1a00'));
    rv = arrayfun(@(x) pdo(x,[1 3]), find(selected), 'UniformOutput', false);

case 'Exclude'
    exclude = ismember(index,unique(vertcat(pdo{selected,2})));
    onoff = {'on','off'};
    rv = vertcat(num2cell(index),onoff(exclude+1));

case 'PortConfig'
    idx = cellfun(@(x) find([pdo{:,1}] == x{1}), varargin{1});
    count = sum(arrayfun(@(x) numel(pdo{x,4}), idx));
    rv = repmat(struct('pdo', [], 'pdo_data_type', 0), 1, count);
    count = 1;
    for i = 1:numel(idx)
        spec = pdo{idx(i),4};
        for j = 1:numel(spec)
            n = numel(spec{j});
            rv(count).pdo = repmat([varargin{2},i-1,0,0], numel(spec{j}), 1);
            rv(count).pdo(:,3) = spec{j};
            rv(count).pdo(:,3) = spec{j};
            rv(count).pdo_data_type = 1000 + pdo{idx(i),3}(spec{i}(1)+1,3);
            count = count + 1;
        end
    end
end

return


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function rv = slave_config(varargin)

% directly write the syncmanager configuration
%   Model       ProductCode          Revision          IndexOffset, function
models = {...
  'EL5101-0010',  hex2dec('13ed3052'), hex2dec('0011000a'); ...
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

rv.sm = {...
        {2, 0, pdo_config('RxPdo')},...
        {3, 1, pdo_config('TxPdo')},...
};

switch get_param(gcbh,'dcmode')
case 'DC-Synchron'
    rv.dc = [hex2dec('320'),0,1,0,0,0,0,1,0,0];
case 'DC-Synchron (input based)'
    rv.dc = [hex2dec('320'),0,1,0,0,1,0,1,0,0];
case 'DC-Customized'
    %rv.dc = evalin('base',get_param(gcbh,'dccustom'));
end

return

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function rv = port_config(SlaveConfig)
% Populate the blocks output port(s)

for i = 1:numel(SlaveConfig.sm)
    sm = SlaveConfig.sm{i};
    switch sm{2}
    case 0
        rv.input  = pdo_config('PortConfig',sm{3},i-1);
    case 1
        rv.output = pdo_config('PortConfig',sm{3},i-1);
    end
end

return

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function checked = update_gui

mask_enables = cell2struct(...
    get_param(gcbh,'MaskEnables'),...
    get_param(gcbh,'MaskNames')...
);

old_enables = mask_enables;

exclude = pdo_config('Exclude');

for i = 1:size(exclude,2)
    mask_enables.(strcat('x',dec2hex(exclude{1,i}))) = exclude{2,i};
end

if ~isequal(old_enables, mask_enables)
    set_param(gcbh,'MaskEnables',struct2cell(mask_enables));
end

return
