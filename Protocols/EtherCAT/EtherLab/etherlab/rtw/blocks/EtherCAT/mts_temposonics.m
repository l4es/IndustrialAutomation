function rv = mts_temposonics(model, offset, filter, tau, ...
                             invert, velocity_output, status_output)

entries = [...
           hex2dec('3101'),  1, 16; ...
           hex2dec('3101'),  2, 32; ...
           hex2dec('3101'),  3, 32; ...

           hex2dec('3101'),  5, 16; ...
           hex2dec('3101'),  6, 32; ...
           hex2dec('3101'),  7, 32; ...

           hex2dec('3101'),  9, 16; ...
           hex2dec('3101'), 10, 32; ...
           hex2dec('3101'), 11, 32; ...

           hex2dec('3101'), 13, 16; ...
           hex2dec('3101'), 14, 32; ...
           hex2dec('3101'), 15, 32; ...

           hex2dec('3101'), 17, 16; ...
           hex2dec('3101'), 18, 32; ...
           hex2dec('3101'), 19, 32; ...
];

pdo = [...
        hex2dec('1a00'), 1, 15 ; ...
];

rv.SlaveConfig.vendor = 64;
rv.SlaveConfig.product = hex2dec('26483052');
rv.SlaveConfig.sm = { ...
        {3, 1, {}} ...  % Only 1 input SyncManager
};

% set numbers of magnets
num_magnets = str2num(model(1));

% RxPdo SyncManager
rv.SlaveConfig.sm{1}{3} = {{pdo(1, 1), entries(1 : 3 * num_magnets, :)}};

 % Fill in Offsets
if (isempty(offset) || numel(offset) == 1 || numel(offset) == num_magnets)
    rv.PortConfig.output(1).offset = {'OffsetPosition', offset};
else
      warning('EtherLab:MTS:offset', ['The dimension of the'...
    ' offset output does not match to the number of elements of the'...
    ' terminal. Please choose a valid output, or the offset is being ignored'])
end

 % Fill in Filters
if (isempty(tau) || numel(tau) == 1 || numel(tau) == num_magnets)
    if isempty(find(tau <= 0))
        rv.PortConfig.output(1).filter = {'Filter', tau};
    else
        errordlg(['Specify a nonzero time constant '...
                  'for the output filter'],'Filter Error');
    end
else
      warning('EtherLab:MTS:tau', ['The dimension of the'...
    ' filter output does not match to the number of elements of the'...
    ' terminal. Please choose a valid output, or the filter is being ignored'])
end

% Populate the block's output port(s)

r = 0 : num_magnets - 1;

% Position output
rv.PortConfig.output(1).pdo = [zeros(numel(r), 4)];
rv.PortConfig.output(1).pdo(:, 3) = [3 * r + 1];
rv.PortConfig.output(1).pdo_data_type = 1032;

if invert
    rv.PortConfig.output(1).gain = -1e-6;
else
    rv.PortConfig.output(1).gain = 1e-6;
end

if velocity_output
    status_pdo = 3;
else
    status_pdo = 2;
end

% Port Config velocity and status

if velocity_output
    rv.PortConfig.output(2).pdo = [zeros(numel(r), 4)];
    rv.PortConfig.output(2).pdo(:, 3) = [3 * r + 2];
    rv.PortConfig.output(2).pdo_data_type = 2032;
    rv.PortConfig.output(2).gain = 1e-6;
end

if status_output
    rv.PortConfig.output(status_pdo).pdo = [zeros(numel(r), 4)];
    rv.PortConfig.output(status_pdo).pdo(:, 3) = [3 * r];
    rv.PortConfig.output(status_pdo).pdo_data_type = 1016;
end
