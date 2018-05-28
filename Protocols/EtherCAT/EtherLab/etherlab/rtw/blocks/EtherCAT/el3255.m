%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Encapsulation for Potentiometer Slave EL3255
%
% Copyright (C) 2013 Richard Hacker
% License: LGPL
%
classdef el3255 < EtherCATSlave

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
methods
    %====================================================================
    function obj = el3255(id)
        if nargin > 0
            obj.slave = obj.find(id);
        end
    end

    %========================================================================
    function rv = configure(obj,count,vector,dc_spec,scaling,sdo)

        % General information
        rv.SlaveConfig.vendor = 2;
        rv.SlaveConfig.product = obj.slave{2};
        rv.SlaveConfig.description = obj.slave{1};

        % Input syncmanager
        rv.SlaveConfig.sm = {{3,1,el3255.pdo}};

        rv.SlaveConfig.sm{1}{3}(count+1:end) = [];

        range = (0:count-1)';
        if vector
            rv.PortConfig.output = struct(...
                'pdo', horzcat(zeros(count,1), range,zeros(count,2)),...
                'pdo_data_type', sint(16) ...
            );
            if count > 1
                rv.PortConfig.output.portname = ...
                        strcat('Ch. 1..',int2str(count));
            else
                rv.PortConfig.output.portname = strcat('Ch. 1');
            end
        else    % vector
            rv.PortConfig.output = struct(...
                'pdo', arrayfun(@(x) [0,x,0,0], range, ...
                                'UniformOutput', false), ...
                'pdo_data_type', sint(16), ...
                'portname', strcat({'Ch. '}, int2str(range+1)) ...
            );
        end     % vector

        % Distributed clocks
        if dc_spec(1)
            if dc_spec(1) ~= 4
                % DC Configuration from the default list
                dc = el3255.dc;
                rv.SlaveConfig.dc = dc(dc_spec(1),:);
            else
                % Custom DC
                rv.SlaveConfig.dc = dc_spec(2:end);
            end
        end

        % Scaling and filter of output port 1
        if isstruct(scaling)
            rv.PortConfig.output(1).full_scale = scaling.full_scale;
            rv.PortConfig.output(1).gain = {'Gain', scaling.gain};
            rv.PortConfig.output(1).offset = {'Offset', scaling.offset};
            rv.PortConfig.output(1).filter = {'Filter', scaling.filter};
        end

        % SDO Configuration
        rv.SlaveConfig.sdo = num2cell(horzcat(el3255.sdo, reshape(sdo,[],1)));
        rv.SlaveConfig.sdo(count+3:end,:) = [];

    end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
methods (Static)
    %====================================================================
    function test(p)
        ei = EtherCATInfo(fullfile(p,'Beckhoff EL32xx.xml'));
        for i = 1:size(el3255.models,1)
            fprintf('Testing %s\n', el3255.models{i,1});
            slave = ei.getSlave(el3255.models{i,2},...
                    'revision', el3255.models{i,3});
            model = el3255.models{i,1};

            for j = 1:4
                rv = el3255(model).configure(j,j&1,1:11,...
                    EtherCATSlave.configureScale(2^31,'4'),1:7);
                slave.testConfig(rv.SlaveConfig,rv.PortConfig);
            end
        end
    end
end     % methods

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
properties (Constant)
    %  name          product code
    models = {
      'EL3255',      hex2dec('0cb73052'), hex2dec('00100000');
    };
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
properties (Access = private, Constant)
    % All known sdo's
    sdo = [hex2dec('8000'), hex2dec('06'),  8;
           hex2dec('8000'), hex2dec('15'), 16;
           hex2dec('8000'), hex2dec('1c'), 16;
           hex2dec('8010'), hex2dec('1c'), 16;
           hex2dec('8020'), hex2dec('1c'), 16;
           hex2dec('8030'), hex2dec('1c'), 16;
           hex2dec('8040'), hex2dec('1c'), 16];

    % PDO
    pdo = { {hex2dec('1a01'), [hex2dec('6000'), 17, 16]}, ...
            {hex2dec('1a03'), [hex2dec('6010'), 17, 16]}, ...
            {hex2dec('1a05'), [hex2dec('6020'), 17, 16]}, ...
            {hex2dec('1a07'), [hex2dec('6030'), 17, 16]}, ...
            {hex2dec('1a09'), [hex2dec('6040'), 17, 16]} };

    % Distributed Clock
    dc = [              0,0,0,0,0,0,0,0,    0,0;    % SM-Synchron
           hex2dec('700'),0,1,0,0,0,0,1,20000,0;    % DC-Synchron
           hex2dec('700'),0,1,0,0,1,0,1,10000,0];   % DC-Synchron (input based)
end     % properties

end     % classdef
