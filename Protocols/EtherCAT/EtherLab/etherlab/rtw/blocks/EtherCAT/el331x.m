%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Encapsulation for Thermocouple Slave EL331x
%
% Copyright (C) 2015 Richard Hacker
% License: LGPL
%
classdef el331x < EtherCATSlave

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
methods
    %====================================================================
    function obj = el331x(id)
        if nargin > 0
            obj.slave = obj.find(id);
        end
    end

    %========================================================================
    function rv = configure(obj,vector,status,sdo_config)

        % General information
        rv.SlaveConfig.vendor = 2;
        rv.SlaveConfig.product = obj.slave{2};
        rv.SlaveConfig.description = obj.slave{1};

        % Vector with channel numbers
        channels = 1:str2double(obj.slave{1}(6));

        %% Output syncmanager
        pdo = el331x.pdo;
        rv.SlaveConfig.sm = { {3,1,pdo{obj.slave{5}}(channels)} };
        pdo = repmat([0 0 0 0], length(channels), 1);
        pdo(:,2) = channels-1;

        % Temperature has SubIndex 17
        temp_idx = find(rv.SlaveConfig.sm{1}{3}{1}{2}(:,2) == 17);
        pdo(:,3) = temp_idx - 1;

        % Number of bits is in column 3
        datatype = sint(rv.SlaveConfig.sm{1}{3}{1}{2}(temp_idx,3));

        % Scaling is in first column of sdo_config,
        % where 1 = 0.1, 2 = 0.01, 3 = 0.001
        scaling = 10.^-sdo_config(channels,1);

        rv.PortConfig.output = EtherCATSlave.configurePorts('Temp', ...
            pdo, datatype, vector);
        if vector
            rv.PortConfig.output.gain = scaling;
            if status
                % Status has SubIndex 7
                pdo(:,3) = find(rv.SlaveConfig.sm{1}{3}{1}{2}(:,2) == 7) - 1;
                rv.PortConfig.output(end+1) = struct(...
                    'pdo', pdo, ...
                    'pdo_data_type', uint(1), ...
                    'portname', 'Status', ...
                    'gain', []);
            end
        else
            for i = 1:length(channels)
                rv.PortConfig.output(i).gain = scaling(i);
            end
        end

        %% Input syncmanager (mandatory for some slaves)
        if obj.slave{4}
            pdo = el331x.rx_pdo;
            rv.SlaveConfig.sm{end+1} = {2,0,pdo(channels)};

            pdo = repmat([1,0,0,0], length(channels), 1);
            pdo(:,2) = channels-1;
            rv.PortConfig.input = EtherCATSlave.configurePorts('CJ', ...
                pdo, sint(16), vector);

            % Input is always in [0.1 degC]
            for i = 1:length(rv.PortConfig.input)
                rv.PortConfig.input(i).gain = 10;
            end
        end

        %% SDO
        % Columns 2 (80x0:0C), 3 (80x0:15) and 5 (80x0:1A) are indexed
        % correctly, except for an offset of 1
        sdo_config(:,[2,3,5]) = sdo_config(:,[2,3,5]) - 1;

        % Resolution and junction material has to be recoded
        resolution_code = el331x.resolution_code;
        tc_code = el331x.tc_code;
        sdo_config(:,[1,4]) = ...
            [resolution_code(sdo_config(:,1))', tc_code(sdo_config(:,4))'];

        % Add another column with the sdo values
        sdo = [el331x.sdo, reshape(sdo_config',[],1)];

        % Pick out the appropriate rows
        rv.SlaveConfig.sdo = num2cell(sdo(obj.slave{6},:));
    end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
methods (Static)

    %========================================================================
    function modelChanged
        obj = el331x(get_param(gcbh,'model'));
        sdo = el331x.sdo;
        sdoList = sdo(obj.slave{6},:);
        EtherCATSlave.updateSDOVisibility(...
                strcat(dec2hex(sdoList(:,1),4),...
                  '_', dec2hex(sdoList(:,2),2)));
        obj.updateRevision();
    end

    %========================================================================
    function updateVector
        EtherCATSlave.setEnable('status',...
                strcmp(get_param(gcbh,'vector'), 'on'));
    end

    %====================================================================
    function test(p)
        ei = EtherCATInfo(fullfile(p,'Beckhoff EL33xx.xml'));
        for i = 1:size(el331x.models,1)
            fprintf('Testing %s\n', el331x.models{i,1});
            slave = ei.getSlave(el331x.models{i,2},...
                    'revision', el331x.models{i,3});
            model = el331x.models{i,1};

            rv = el331x(model).configure(1,1,ones(8,5)*3);
            slave.testConfig(rv.SlaveConfig,rv.PortConfig);
        end
    end
end     % methods

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
properties (Constant)
    %   Model          ProductCode          Revision        RxPdo, TxPdoIdx, SDO Idx;
    models = {...
        'EL3311',      hex2dec('0cef3052'), hex2dec('00100000'), 1, 1, 1:4;
        'EL3312',      hex2dec('0cf03052'), hex2dec('00100000'), 1, 1, ...
            [1:4,6:9];
        'EL3314',      hex2dec('0cf23052'), hex2dec('00100000'), 1, 1, ...
            [1:4,6:9,11:14,16:19];
        'EL3314-0010', hex2dec('0cf23052'), hex2dec('0010000a'), 0, 2, [1:20];
        'EL3318',      hex2dec('0cf63052'), hex2dec('00100000'), 1, 3, ...
            [1:4,6:9,11:14,16:19,21:24,26:29,31:34,36:39];
    };
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
properties (Access = private, Constant)
    % External cold junction compensation (not mandatory)
    rx_pdo =  { {hex2dec('1600'), [hex2dec('7000'), 17, 16]}, ...
                {hex2dec('1601'), [hex2dec('7010'), 17, 16]}, ...
                {hex2dec('1602'), [hex2dec('7020'), 17, 16]}, ...
                {hex2dec('1603'), [hex2dec('7030'), 17, 16]}, ...
                {hex2dec('1604'), [hex2dec('7040'), 17, 16]}, ...
                {hex2dec('1605'), [hex2dec('7050'), 17, 16]}, ...
                {hex2dec('1606'), [hex2dec('7060'), 17, 16]}, ...
                {hex2dec('1607'), [hex2dec('7070'), 17, 16]} };

    % TxPdos (mandatory)
    % EL3311, EL3312, EL3314
    pdo  = {{{hex2dec('1a00'), [hex2dec('6000'),  1,  1;
                                hex2dec('6000'),  2,  1;
                                hex2dec('6000'),  3,  2;
                                hex2dec('6000'),  5,  2;
                                hex2dec('6000'),  7,  1;
                                0              ,  0,  7;
                                hex2dec('6000'), 15,  1;
                                hex2dec('1800'),  9,  1;
                                hex2dec('6000'), 17, 16]}, ...
             {hex2dec('1a01'), [hex2dec('6010'),  1,  1;
                                hex2dec('6010'),  2,  1;
                                hex2dec('6010'),  3,  2;
                                hex2dec('6010'),  5,  2;
                                hex2dec('6010'),  7,  1;
                                0              ,  0,  7;
                                hex2dec('6010'), 15,  1;
                                hex2dec('1801'),  9,  1;
                                hex2dec('6010'), 17, 16]}, ...
             {hex2dec('1a02'), [hex2dec('6020'),  1,  1;
                                hex2dec('6020'),  2,  1;
                                hex2dec('6020'),  3,  2;
                                hex2dec('6020'),  5,  2;
                                hex2dec('6020'),  7,  1;
                                0              ,  0,  7;
                                hex2dec('6020'), 15,  1;
                                hex2dec('1802'),  9,  1;
                                hex2dec('6020'), 17, 16]}, ...
             {hex2dec('1a03'), [hex2dec('6030'),  1,  1;
                                hex2dec('6030'),  2,  1;
                                hex2dec('6030'),  3,  2;
                                hex2dec('6030'),  5,  2;
                                hex2dec('6030'),  7,  1;
                                0              ,  0,  7;
                                hex2dec('6030'), 15,  1;
                                hex2dec('1803'),  9,  1;
                                hex2dec('6030'), 17, 16]}}, ...
            {{hex2dec('1a00'), [hex2dec('6000'),  1,  1;              % EL3314-0010
                                hex2dec('6000'),  2,  1;
                                0              ,  0,  4;
                                hex2dec('6000'),  7,  1;
                                0              ,  0,  7;
                                hex2dec('6000'), 15,  1;
                                hex2dec('1800'),  9,  1;
                                hex2dec('6000'), 17, 32]}, ...
             {hex2dec('1a01'), [hex2dec('6010'),  1,  1;
                                hex2dec('6010'),  2,  1;
                                0              ,  0,  4;
                                hex2dec('6010'),  7,  1;
                                0              ,  0,  7;
                                hex2dec('6010'), 15,  1;
                                hex2dec('1801'),  9,  1;
                                hex2dec('6010'), 17, 32]}, ...
             {hex2dec('1a02'), [hex2dec('6020'),  1,  1;
                                hex2dec('6020'),  2,  1;
                                0              ,  0,  4;
                                hex2dec('6020'),  7,  1;
                                0              ,  0,  7;
                                hex2dec('6020'), 15,  1;
                                hex2dec('1802'),  9,  1;
                                hex2dec('6020'), 17, 32]}, ...
             {hex2dec('1a03'), [hex2dec('6030'),  1,  1;
                                hex2dec('6030'),  2,  1;
                                0              ,  0,  4;
                                hex2dec('6030'),  7,  1;
                                0              ,  0,  7;
                                hex2dec('6030'), 15,  1;
                                hex2dec('1803'),  9,  1;
                                hex2dec('6030'), 17, 32]}}, ...
            {{hex2dec('1a00'), [hex2dec('6000'),  1,  1;              % EL3318
                                hex2dec('6000'),  2,  1;
                                0              ,  0,  4;
                                hex2dec('6000'),  7,  1;
                                0              ,  0,  7;
                                hex2dec('6000'), 15,  1;
                                hex2dec('1800'),  9,  1;
                                hex2dec('6000'), 17, 16]}, ...
             {hex2dec('1a01'), [hex2dec('6010'),  1,  1;
                                hex2dec('6010'),  2,  1;
                                0              ,  0,  4;
                                hex2dec('6010'),  7,  1;
                                0              ,  0,  7;
                                hex2dec('6010'), 15,  1;
                                hex2dec('1801'),  9,  1;
                                hex2dec('6010'), 17, 16]}, ...
             {hex2dec('1a02'), [hex2dec('6020'),  1,  1;
                                hex2dec('6020'),  2,  1;
                                0              ,  0,  4;
                                hex2dec('6020'),  7,  1;
                                0              ,  0,  7;
                                hex2dec('6020'), 15,  1;
                                hex2dec('1802'),  9,  1;
                                hex2dec('6020'), 17, 16]}, ...
             {hex2dec('1a03'), [hex2dec('6030'),  1,  1;
                                hex2dec('6030'),  2,  1;
                                0              ,  0,  4;
                                hex2dec('6030'),  7,  1;
                                0              ,  0,  7;
                                hex2dec('6030'), 15,  1;
                                hex2dec('1803'),  9,  1;
                                hex2dec('6030'), 17, 16]}, ...
             {hex2dec('1a04'), [hex2dec('6040'),  1,  1;
                                hex2dec('6040'),  2,  1;
                                0              ,  0,  4;
                                hex2dec('6040'),  7,  1;
                                0              ,  0,  7;
                                hex2dec('6040'), 15,  1;
                                hex2dec('1804'),  9,  1;
                                hex2dec('6040'), 17, 16]}, ...
             {hex2dec('1a05'), [hex2dec('6050'),  1,  1;
                                hex2dec('6050'),  2,  1;
                                0              ,  0,  4;
                                hex2dec('6050'),  7,  1;
                                0              ,  0,  7;
                                hex2dec('6050'), 15,  1;
                                hex2dec('1805'),  9,  1;
                                hex2dec('6050'), 17, 16]}, ...
             {hex2dec('1a06'), [hex2dec('6060'),  1,  1;
                                hex2dec('6060'),  2,  1;
                                0              ,  0,  4;
                                hex2dec('6060'),  7,  1;
                                0              ,  0,  7;
                                hex2dec('6060'), 15,  1;
                                hex2dec('1806'),  9,  1;
                                hex2dec('6060'), 17, 16]}, ...
             {hex2dec('1a07'), [hex2dec('6070'),  1,  1;
                                hex2dec('6070'),  2,  1;
                                0              ,  0,  4;
                                hex2dec('6070'),  7,  1;
                                0              ,  0,  7;
                                hex2dec('6070'), 15,  1;
                                hex2dec('1807'),  9,  1;
                                hex2dec('6070'), 17, 16]}}};

    % Numeric value for SDO #x800n0:19
    tc_code = [0:10,100:104];
    resolution_code = [0,2,3];

    % All known sdo's
    % For all except EL3314-0010, remove subindex 1A
    sdo = [hex2dec('8000'), hex2dec('02'),  8; 
           hex2dec('8000'), hex2dec('0C'),  8;
           hex2dec('8000'), hex2dec('15'), 16;
           hex2dec('8000'), hex2dec('19'), 16;
           hex2dec('8000'), hex2dec('1A'), 16;
           hex2dec('8010'), hex2dec('02'),  8;
           hex2dec('8010'), hex2dec('0C'),  8;
           hex2dec('8010'), hex2dec('15'), 16;
           hex2dec('8010'), hex2dec('19'), 16;
           hex2dec('8010'), hex2dec('1A'), 16;
           hex2dec('8020'), hex2dec('02'),  8;
           hex2dec('8020'), hex2dec('0C'),  8;
           hex2dec('8020'), hex2dec('15'), 16;
           hex2dec('8020'), hex2dec('19'), 16;
           hex2dec('8020'), hex2dec('1A'), 16;
           hex2dec('8030'), hex2dec('02'),  8;
           hex2dec('8030'), hex2dec('0C'),  8;
           hex2dec('8030'), hex2dec('15'), 16;
           hex2dec('8030'), hex2dec('19'), 16;
           hex2dec('8030'), hex2dec('1A'), 16;
           hex2dec('8040'), hex2dec('02'),  8;
           hex2dec('8040'), hex2dec('0C'),  8;
           hex2dec('8040'), hex2dec('15'), 16;
           hex2dec('8040'), hex2dec('19'), 16;
           hex2dec('8040'), hex2dec('1A'), 16;
           hex2dec('8050'), hex2dec('02'),  8;
           hex2dec('8050'), hex2dec('0C'),  8;
           hex2dec('8050'), hex2dec('15'), 16;
           hex2dec('8050'), hex2dec('19'), 16;
           hex2dec('8050'), hex2dec('1A'), 16;
           hex2dec('8060'), hex2dec('02'),  8;
           hex2dec('8060'), hex2dec('0C'),  8;
           hex2dec('8060'), hex2dec('15'), 16;
           hex2dec('8060'), hex2dec('19'), 16;
           hex2dec('8060'), hex2dec('1A'), 16;
           hex2dec('8070'), hex2dec('02'),  8;
           hex2dec('8070'), hex2dec('0C'),  8;
           hex2dec('8070'), hex2dec('15'), 16;
           hex2dec('8070'), hex2dec('19'), 16;
           hex2dec('8070'), hex2dec('1A'), 16];

end     % properties

end     % classdef
