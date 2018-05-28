%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Encapsulation for Sin/Cos Encoder EL5021
%
% Copyright (C) 2013 Richard Hacker
% License: LGPL
%
classdef el5021 < EtherCATSlave

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
methods
    %====================================================================
    function obj = el5021(id)
        if nargin > 0
            obj.slave = obj.find(id);
        end
    end

    %========================================================================
    function rv = configure(obj,dc_spec,sdo)

        % General information
        rv.SlaveConfig.vendor = 2;
        rv.SlaveConfig.product = obj.slave{2};
        rv.SlaveConfig.description = obj.slave{1};

        % Input syncmanager
        rv.SlaveConfig.sm = {{2, 0, {{obj.pdo{1}{1} []}}},
                             {3, 1, {{obj.pdo{2}{1} []}}}};
        rv.SlaveConfig.sm{1}{3}{1}{2} = cell2mat(obj.pdo{1}{2}(:,1:3));
        rv.SlaveConfig.sm{2}{3}{1}{2} = cell2mat(obj.pdo{2}{2}(:,1:3));

        % CoE Configuration
        rv.SlaveConfig.sdo = num2cell(horzcat(el5021.sdo,sdo'));

        % Distributed clocks
        if dc_spec(1) ~= 4
            % DC Configuration from the default list
            rv.SlaveConfig.dc = obj.dc(dc_spec(1),:);
        else
            % Custom DC
            rv.SlaveConfig.dc = dc_spec(2:end);
        end

        % Input port
        rv.PortConfig.input(1).pdo = [0,0,0,0; 0,0,2,0];
        rv.PortConfig.input(1).pdo_data_type = uint(1);
        rv.PortConfig.input(1).portname = 'bool[2]';
        rv.PortConfig.input(2).pdo = [0,0,4,0];
        rv.PortConfig.input(2).pdo_data_type = uint(32);
        rv.PortConfig.input(2).portname = 'Value';

        rv.PortConfig.output(1).pdo = repmat([1,0,0,0], 5, 1);
        rv.PortConfig.output(1).pdo(:,3) = [0,2,3,4,6];
        rv.PortConfig.output(1).pdo_data_type = uint(1);
        rv.PortConfig.output(1).portname = 'bool[5]';
        rv.PortConfig.output(2).pdo = [1,0,11,0];
        rv.PortConfig.output(2).pdo_data_type = uint(32);
        rv.PortConfig.output(2).portname = 'Counter';
        rv.PortConfig.output(3).pdo = [1,0,12,0];
        rv.PortConfig.output(3).pdo_data_type = uint(32);
        rv.PortConfig.output(3).portname = 'Latch';

    end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
methods (Static)
    %====================================================================
    function test(p)
        ei = EtherCATInfo(fullfile(p,'Beckhoff EL5xxx.xml'));
        for i = 1:size(el5021.models,1)
            fprintf('Testing %s\n', el5021.models{i,1});
            slave = ei.getSlave(el5021.models{i,2},...
                    'revision', el5021.models{i,3});
            model = el5021.models{i,1};

            rv = el5021(model).configure(2,1:5);
            slave.testConfig(rv.SlaveConfig,rv.PortConfig);
        end
    end
end     % methods

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
properties (Constant)
    %  name          product code
    models = {...
      'EL5021',      hex2dec('139d3052'), hex2dec('00100000');
    };

    % PDO class
    pdo = { {hex2dec('1600'), {hex2dec('7000'),  1,  1
                               hex2dec('0'),     0,  1
                               hex2dec('7000'),  3,  1
                               hex2dec('0'),     0, 13
                               hex2dec('7000'), 17, 32}},
            {hex2dec('1a00'), {hex2dec('6000'),  1,  1
                               hex2dec('0'),     0,  1
                               hex2dec('6000'),  3,  1
                               hex2dec('6001'),  4,  1
                               hex2dec('6001'),  5,  1
                               hex2dec('0'),     0,  5
                               hex2dec('6000'), 11,  1
                               hex2dec('0'),     0,  2
                               hex2dec('1c32'), 32,  1
                               hex2dec('1800'),  7,  1
                               hex2dec('1800'),  9,  1
                               hex2dec('6000'), 17, 32
                               hex2dec('6000'), 18, 32}}};

    sdo = [hex2dec('8000'), hex2dec('01'),   8;
           hex2dec('8000'), hex2dec('0E'),   8;
           hex2dec('8001'), hex2dec('01'),   8;
           hex2dec('8001'), hex2dec('02'),   8;
           hex2dec('8001'), hex2dec('11'),   8;];

    dc = [              0,0,0,     0,0,0,0,0,    0,0;    % SM-Synchron
           hex2dec('700'),0,1,-30600,0,0,0,1,25000,0;    % DC-Synchron
           hex2dec('700'),0,1,-30600,0,1,0,1,25000,0];   % DC-Synchron (input based)
end     % properties

end     % classdef
