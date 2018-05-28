%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Encapsulation for oversampling digital input slave EL1262
%
% Copyright (C) 2016 Richard Hacker,
% License: LGPL
%
%
classdef el2262 < EtherCATSlave

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
methods
    %====================================================================
    function obj = el2262(id)
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
        if dc_spec(1) ~= 18
            % DC Configuration from the default list
            dc_ = el2262.dc;
            rv.SlaveConfig.dc = dc_(dc_spec(1),:);
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

        % Clear all SyncManagers
        rv.SlaveConfig.sm{1} = {0, 0, []};
        rv.SlaveConfig.sm{2} = {1, 0, []};
        rv.SlaveConfig.sm{3} = {2, 1, []};
        
        % Input syncmanager
        for i = channels
            rv.SlaveConfig.sm{i} = {i-1,0,el2262.pdos{i}};

            entries = rv.SlaveConfig.sm{i}{3}{2}{2};
            rv.SlaveConfig.sm{i}{3}{2}{2} = horzcat(...
                    entries(1) + (0:os_fac-1)', ...
                    repmat(entries(2:end), os_fac, 1));

            gap = rem(os_fac,8);
            if (gap)
                rv.SlaveConfig.sm{i}{3}{3} = ...
                    {el2262.gap_pdo_idx(i) + gap - 1, [0,0,8-gap]};
            end
        end

        rv.PortConfig.input = arrayfun( ...
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
      'EL2262',      hex2dec('08d63052'), hex2dec('00030000');
    };
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
properties (Constant)

    % PDO list
    pdos = {{ {hex2dec('1700'), [hex2dec('7800'),             1, 16]}, ...
              {hex2dec('1600'), [hex2dec('7000'),             1,  1]}}, ...
            { {hex2dec('1701'), [hex2dec('7800'),             2, 16]}, ...
              {hex2dec('1601'), [hex2dec('7000'),             2,  1]}}, ...
            { {hex2dec('1702'), [hex2dec('1d09'), hex2dec('98'), 32]}}};

    % Starting index of gap PDO
    gap_pdo_idx = [hex2dec('1710'), hex2dec('1718')];

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

