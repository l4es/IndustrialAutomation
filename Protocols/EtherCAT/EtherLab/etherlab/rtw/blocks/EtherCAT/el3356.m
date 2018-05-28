%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Encapsulation for DMS Slave EL3356
%
% Copyright (C) 2013 Richard Hacker
% License: LGPL
%
classdef el3356 < EtherCATSlave

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
methods
    %====================================================================
    function obj = el3356(id)
        if nargin > 0
            obj.slave = obj.find(id);
        end
    end

    %========================================================================
    function rv = configure(obj,adc,dc_spec,scaling,sdo_config)
        % Whether this is EL3356, not EL3356-0010
        % The predefined PDO and DC structures are for EL3356-0010
        % The EL3356 does not have all these features

        % General information
        rv.SlaveConfig.vendor = 2;
        rv.SlaveConfig.product = obj.slave{2};
        rv.SlaveConfig.description = obj.slave{1};

        % Input and output syncmanager
        rv.SlaveConfig.sm = {{2,0,el3356.ctrl_pdo}, {3,1,{}}};
        if obj.slave{4}
            % Remove PDO Entry 7000:04 for EL3356
            rv.SlaveConfig.sm{1}{3}{1}{2}(4,1:2) = [0,0];
        end

        % Control input port
        % simply all pdo entries
        rows = find(rv.SlaveConfig.sm{1}{3}{1}{2}(:,1)) - 1;
        rv.PortConfig.input = struct(...
                'pdo',horzcat(zeros(size(rows,1),2), ...
                              rows, ...
                              zeros(size(rows))), ...
                'pdo_data_type', uint(1), ...
                'portname','Ctrl' ...
        );

        if adc
            % 2xADC Converter
            rv.SlaveConfig.sm{2}{3} = el3356.adc_status_pdo;
            if obj.slave{4}
                % Modifications for EL3356
                % modify entry row 5 [0,0,1] to bitlen 8
                % remove entry row 6 [0,0,7]
                rv.SlaveConfig.sm{2}{3}{1}{2}(5,3) = 8;
                rv.SlaveConfig.sm{2}{3}{1}{2}(6,:) = [];
                rv.SlaveConfig.sm{2}{3}{2}{2}(5,3) = 8;
                rv.SlaveConfig.sm{2}{3}{2}{2}(6,:) = [];
            end

            % Signal output port; only entry with bitlen of 32
            row = find(rv.SlaveConfig.sm{2}{3}{1}{2}(:,3) == 32) - 1;
            rv.PortConfig.output = struct(...
                'pdo', [1,0,row,0;
                        1,1,row,0], ...
                'pdo_data_type', sint(32), ...
                'portname', 'ADC' ...
            );

            % Status 1 output port; all rows with bitlen of 1, except last
            % TxPDO toggle
            pdo = el3356.adc_status_pdo;
            row = find(  pdo{1}{2}(:,3) == 1 ...
                       & pdo{1}{2}(:,1)) - 1;
            row(end) = [];      % Remove TxPDO toggle 60x0:16
            rv.PortConfig.output(2) = struct(...
                'pdo', horzcat(ones(size(row)),...
                               zeros(size(row)), ...
                               row,...
                               zeros(size(row))), ...
                'pdo_data_type', uint(1), ...
                'portname', 'Status 1' ...
            );

            % Status 2 output port; similar to Status 1
            rv.PortConfig.output(3) = rv.PortConfig.output(2);
            rv.PortConfig.output(3).pdo(:,2) = ones(size(row));
            rv.PortConfig.output(3).portname = 'Status 2';

        else
            % Bridge measurement
            rv.SlaveConfig.sm{2}{3} = el3356.rmb_status_pdo;

            % Signal output port; only entry of PDO 1a01
            rv.PortConfig.output = struct(...
                'pdo', [1,1,0,0], ...
                'pdo_data_type', sint(32), ...
                'portname', 'RMB' ...
            );

            % Status output port; all entries with bitlen of 1,
            % except for last 2 rows: Sync_error and TxPDO_toggle
            pdo = el3356.rmb_status_pdo;
            row = find(  pdo{1}{2}(:,3) == 1 ...
                       & pdo{1}{2}(:,1)) - 1;
            row(end-1:end) = [];      % Remove Sync error 1c32:32 
                                      % and TxPDO toggle 1800:9
            rv.PortConfig.output(2) = struct(...
                'pdo', horzcat(ones(size(row)),...
                               zeros(size(row)), ...
                               row,...
                               zeros(size(row))), ...
                'pdo_data_type', uint(1), ...
                'portname', 'Status' ...
            );

        end

        % Distributed clock for EL3356-0010 only
        if ~obj.slave{4} && dc_spec(1)
            if dc_spec(1) ~= 4
                % DC Configuration from the default list
                dc = el3356.dc;
                rv.SlaveConfig.dc = dc(dc_spec(1),:);
            else
                % Custom DC
                rv.SlaveConfig.dc = dc_spec(2:end);
            end

            if rv.SlaveConfig.dc(1)
                % Add Timestamp PDO when using DC - required
                n = numel(rv.SlaveConfig.sm{2}{3});
                rv.SlaveConfig.sm{2}{3}{n+1} = el3356.time_pdo;

                % Output the timestamp as 2 uint32's
                rv.PortConfig.output(end+1) = struct(...
                    'pdo', [1,n,0,0;
                            1,n,0,1], ...
                    'pdo_data_type', uint(32), ...
                    'portname', 'Time' ...
                );
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
        sdo = el3356.sdo;
        rv.SlaveConfig.sdo = num2cell([sdo(obj.slave{5},:), ...
                                       reshape(sdo_config(obj.slave{5}),[],1)]);
    end
end

methods (Static)
    %========================================================================
    function modelChanged()
        obj = el3356(get_param(gcbh,'model'));
        obj.updateDCVisibility(~obj.slave{4});
        obj.updateSDOVisibility(dec2base(obj.slave{5},10,2));
        obj.updateRevision();
    end

    %====================================================================
    function test(p)
        ei = EtherCATInfo(fullfile(p,'Beckhoff EL33xx.xml'));
        for i = 1:size(el3356.models,1)
            fprintf('Testing %s\n', el3356.models{i,1});
            slave = ei.getSlave(el3356.models{i,2},...
                    'revision', el3356.models{i,3});
            model = el3356.models{i,1};

            rv = el3356(model).configure(0,1:10,...
                    EtherCATSlave.configureScale(2^31,''),1:17);
            slave.testConfig(rv.SlaveConfig,rv.PortConfig);

            rv = el3356(model).configure(1,2,...
                    EtherCATSlave.configureScale(2^31,'6'),1:17);
            slave.testConfig(rv.SlaveConfig,rv.PortConfig);
        end
    end
end     % methods

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
properties (Constant)
    %  name          product code         basic_version, sdo
    models = {...
      'EL3356',      hex2dec('0d1c3052'), hex2dec('00110000'), true, [1,3,5,6,8:17];
      'EL3356-0010', hex2dec('0d1c3052'), hex2dec('0011000a'), false, 1:17;
    };

    % All known sdo's
    % For EL3356, remove rows 2,4,7
    sdo = [hex2dec('8000'), hex2dec('1'),   8;
           hex2dec('8000'), hex2dec('2'),   8;
           hex2dec('8000'), hex2dec('3'),   8;
           hex2dec('8000'), hex2dec('4'),   8;
           hex2dec('8000'), hex2dec('5'),   8;
           hex2dec('8000'), hex2dec('11'), 16;
           hex2dec('8000'), hex2dec('12'), 16;
           hex2dec('8000'), hex2dec('13'), 16;
           hex2dec('8000'), hex2dec('14'), 32;
           hex2dec('8000'), hex2dec('29'), 16;
           hex2dec('8000'), hex2dec('2a'), 32;
           hex2dec('8000'), hex2dec('31'), 16;
           hex2dec('8000'), hex2dec('32'), 16;
           hex2dec('8010'), hex2dec('06'),  8;
           hex2dec('8010'), hex2dec('15'), 16;
           hex2dec('8020'), hex2dec('06'),  8;
           hex2dec('8020'), hex2dec('15'), 16];
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
properties (Access = private, Constant)
    % Control port pdo
    % for EL3356, PdoEntry 7000:4 must be zeroed
    ctrl_pdo =       { {hex2dec('1600'), [hex2dec('7000'),  1,  1;
                                          hex2dec('7000'),  2,  1;
                                          hex2dec('7000'),  3,  1;
                                          hex2dec('7000'),  4,  1;
                                          hex2dec('7000'),  5,  1;
                                          0              ,  0, 11]} };

    % ADC and status PDO
    % for EL3356, pdo entry 5 must be removed and entry 6 is occupies 8 bits
    adc_status_pdo = { {hex2dec('1a04'), [hex2dec('6010'),  1,  1;
                                          hex2dec('6010'),  2,  1;
                                          0              ,  0,  4;
                                          hex2dec('6010'),  7,  1;
                                          0              ,  0,  1;
                                          0              ,  0,  7;
                                          hex2dec('6010'), 16,  1;
                                          hex2dec('6010'), 17, 32]}, ...
                       {hex2dec('1a06'), [hex2dec('6020'),  1,  1;
                                          hex2dec('6020'),  2,  1;
                                          0              ,  0,  4;
                                          hex2dec('6020'),  7,  1;
                                          0              ,  0,  1;
                                          0              ,  0,  7;
                                          hex2dec('6020'), 16,  1;
                                          hex2dec('6020'), 17, 32]} };

    % RMB only pdo
    rmb_status_pdo = { {hex2dec('1a00'), [0              ,  0,  1;
                                          hex2dec('6000'),  2,  1;
                                          0              ,  0,  1;
                                          hex2dec('6000'),  4,  1;
                                          0              ,  0,  2;
                                          hex2dec('6000'),  7,  1;
                                          hex2dec('6000'),  8,  1;
                                          hex2dec('6000'),  9,  1;
                                          0              ,  0,  4;
                                          hex2dec('1c32'), 32,  1;
                                          0              ,  0,  1;
                                          hex2dec('1800'),  9,  1]}, ...
                       {hex2dec('1a01'), [hex2dec('6000'), 17, 32]} };

    % Clock PDO, required by EL3356-0010 when using DC
    time_pdo =         {hex2dec('1a03'), [hex2dec('6000'), 19, 64]};

    % Distributed Clock (EL3356-0010 only)
    dc = [              0,0,0,0,0,0,0,0,0,0;    % SM-Synchron
           hex2dec('320'),0,1,0,0,0,0,1,0,0;    % DC-Synchron
           hex2dec('320'),0,1,0,0,1,0,1,0,0];   % DC-Synchron (input based)
end     % properties

end     % classdef
