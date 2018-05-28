%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Encapsulation for oversampling analog input slave EL37x2
%
% Copyright (C) 2013 Richard Hacker
% License: LGPL
%
classdef el37x2 < EtherCATSlave

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
methods
    %====================================================================
    function obj = el37x2(id)
        if nargin > 0
            obj.slave = obj.find(id);
        end
    end

    %========================================================================
    function rv = configure(obj,one_ch,dc_spec,scaling)

        % General information
        rv.SlaveConfig.vendor = 2;
        rv.SlaveConfig.product = obj.slave{2};
        rv.SlaveConfig.description = obj.slave{1};

        % Distributed clock
        if dc_spec(1) ~= 15
            % DC Configuration from the default list
            dc = el37x2.dc;
            rv.SlaveConfig.dc = dc(dc_spec(1),:);
        else
            % Custom DC
            rv.SlaveConfig.dc = dc_spec(2:end);
        end

        os_fac = -rv.SlaveConfig.dc(3);
        if os_fac <= 0
            os_fac = 0;
        end

        if one_ch
            channels = 1;
        else
            channels = 1:2;
        end

        % output syncmanager
        for i = channels
            rv.SlaveConfig.sm{i} = {i-1,1,el37x2.pdos{i}};
            entries = rv.SlaveConfig.sm{i}{3}{2}{2};
            rv.SlaveConfig.sm{i}{3}{2}{2} = horzcat(...
                    entries(1)+el37x2.os_idx_inc*(0:os_fac-1)', ...
                    repmat(entries(2:end),os_fac,1));
        end

        fs = [];

        gain = repmat({[]},size(channels));
        if isfield(scaling,'gain') && ~isempty(scaling.gain)
            fs = obj.slave{4};
            gain = arrayfun(@(x) {{strcat('Gain',num2str(x)),
                                  scaling.gain(min(end,x))}}, ...
                            channels);
        end

        offset = repmat({[]},size(channels));
        if isfield(scaling,'offset') && ~isempty(scaling.offset)
            fs = obj.slave{4};
            offset = arrayfun(@(x) {{strcat('Offset',num2str(x)),
                                  scaling.offset(min(end,x))}}, ...
                            channels);
        end

        rv.PortConfig.output = arrayfun( ...
            @(i) struct('pdo',horzcat(repmat([i-1,1],os_fac,1),...
                                      (0:os_fac-1)',...
                                      repmat(0,os_fac,1)),...
                        'pdo_data_type',sint(16),...
                        'full_scale',fs,...
                        'gain',gain(i), ...
                        'offset',offset(i), ...
                        'portname', strcat('Ch.',num2str(i))), ...
            channels);
    end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
methods (Static)
    %====================================================================
    function test(p)
        ei = EtherCATInfo(fullfile(p,'Beckhoff EL37xx.xml'));
        for i = 1:size(el37x2.models,1)
            fprintf('Testing %s\n', el37x2.models{i,1});
            slave = ei.getSlave(el37x2.models{i,2},...
                    'revision', el37x2.models{i,3});
            model = el37x2.models{i,1};

            for j = 1:14
                rv = el37x2(model).configure(j&1,j,...
                EtherCATSlave.configureScale(2^15,'4'));
                slave.testConfig(rv.SlaveConfig,rv.PortConfig);
            end
        end
    end
end     % methods

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
properties (Constant)
    %  name          product code         basic_version
    models = {...
      'EL3702', hex2dec('0e763052'), hex2dec('00020000'), 2^15;
      'EL3742', hex2dec('0e9e3052'), hex2dec('00010000'), 2^15;
    };
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
properties (Access = private, Constant)
    % ADC and status PDO
    pdos = {{ {hex2dec('1b00'), [hex2dec('6800'),  1, 16]},
              {hex2dec('1a00'), [hex2dec('6000'),  1, 16]}}, 
            { {hex2dec('1b01'), [hex2dec('6800'),  2, 16]},
              {hex2dec('1a80'), [hex2dec('6000'),  2, 16]}}};

    os_idx_inc = 16;

    % Distributed Clock
    dc = [hex2dec('730'),0,  -1,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0,  -2,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0,  -3,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0,  -4,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0,  -5,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0,  -8,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0, -10,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0, -16,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0, -20,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0, -25,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0, -32,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0, -40,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0, -50,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0,-100,-10000,1,1,0,-1,0,0];

end     % properties

end     % classdef
