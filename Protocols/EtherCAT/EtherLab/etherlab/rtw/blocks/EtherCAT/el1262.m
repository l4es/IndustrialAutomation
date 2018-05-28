%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Encapsulation for oversampling digital input slave EL1262
%
% Copyright (C) 2013 Richard Hacker,
%               2015 Wilhelm Hagemeister (modified EL37x2)
% License: LGPL
%
%
classdef el1262 < EtherCATSlave

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
methods
    %====================================================================
    function obj = el1262(id)
        if nargin > 0
            obj.slave = obj.find(id);
        end
    end

    %========================================================================
    function rv = configure(obj,one_ch,dc_spec)

        % General information
        rv.SlaveConfig.vendor = 2;
        rv.SlaveConfig.product = obj.slave{2};
        rv.SlaveConfig.description = obj.slave{1};

        % Distributed clock
        if dc_spec(1) ~= 18  %%Hm changed from 15
            % DC Configuration from the default list
            dc = el1262.dc;
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

        % output syncmanager  Hm /8
        for i = channels
            rv.SlaveConfig.sm{i} = {i-1,1,el1262.pdos{i}};

            entries = rv.SlaveConfig.sm{i}{3}{2}{2};
            rv.SlaveConfig.sm{i}{3}{2}{2} = horzcat(...
                    entries(1) + el1262.os_idx_inc*(0:os_fac-1)', ...
                    repmat(entries(2:end), os_fac, 1));

            gap = rem(os_fac,8);
            if (gap)
                rv.SlaveConfig.sm{i}{3}{3} = ...
                    {el1262.gap_pdo_idx(i) + gap - 1, [0,0,gap]};
            end
        end

        rv.PortConfig.output = arrayfun( ...
            @(i) struct('pdo',horzcat(repmat([i-1,1],os_fac,1),...
                                      (0:os_fac-1)',...
                                      repmat(0,os_fac,1)),...
                        'pdo_data_type',uint(1),...
                        'portname', strcat('Ch.',num2str(i))), ...
            channels);
    end
end

methods (Static)

    %====================================================================
% Hm
%    function test(p)
%        ei = EtherCATInfo(fullfile(p,'Beckhoff EL37xx.xml'));
%        for i = 1:size(el1262.models,1)
%            fprintf('Testing %s\n', el1262.models{i,1});
%            for j = 1:14
%                rv = el1262.configure(el1262.models{i,1},j&1,j,...
%                EtherCATSlave.configureScale(2^15,'4'));
%                ei.testConfiguration(rv.SlaveConfig,rv.PortConfig);
%            end
%        end
%    end
end     % methods

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
properties (Constant)
    %  name          product code         basic_version
    models = {...
      'EL1262',      hex2dec('04ee3052'), hex2dec('00030000');
      'EL1262-0050', hex2dec('04ee3052'), hex2dec('00030032');
    };
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
properties (Access = private, Constant)

    os_idx_inc = 8;
    %change this to 8 to get the data in bytes (os_fac has to fit in 8 bit than)
    
    % ADC and status PDO
    pdos = {{ {hex2dec('1b00'), [hex2dec('6800'),              1, 16]},
              {hex2dec('1a00'), [hex2dec('6000'),              1,  1]}}, 
            { {hex2dec('1b01'), [hex2dec('6800'),              2, 16]},
              {hex2dec('1a00'), [hex2dec('6000'),              2,  1]}},
            { {hex2dec('1b05'), [hex2dec('1d09'),  hex2dec('98'), 32]}}};

    % Starting index of gap PDO
    gap_pdo_idx = [hex2dec('1b10'), hex2dec('1b18')];

    % Distributed Clock
    dc = [hex2dec('730'),0,   -1,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0,   -2,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0,   -3,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0,   -4,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0,   -5,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0,   -8,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0,  -10,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0,  -16,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0,  -20,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0,  -25,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0,  -32,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0,  -40,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0,  -50,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0, -100,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0, -200,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0, -400,-10000,1,1,0,-1,0,0;
          hex2dec('730'),0,-1000,-10000,1,1,0,-1,0,0];

end     % properties

end     % classdef

