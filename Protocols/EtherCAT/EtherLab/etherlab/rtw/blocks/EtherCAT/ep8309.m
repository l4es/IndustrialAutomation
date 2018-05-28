classdef ep8309 < EtherCATSlave
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods
        %====================================================================
        function obj = ep8309(id)
            if nargin > 0
                obj.slave = obj.find(id);
            end
        end

        %====================================================================
        function rv = configure(obj,io)

            rv.SlaveConfig.vendor = 2;
            rv.SlaveConfig.description = obj.slave{1};
            rv.SlaveConfig.product = obj.slave{2};

            % The various functions
            digin   = isfield(io,   'digin');
            digout  = isfield(io,  'digout');
            anain   = isfield(io,   'anain');
            current = isfield(io, 'current');
            pwm     = isfield(io,     'pwm');
            devstat = isfield(io, 'devstat');
            tacho1  = isfield(io,  'tacho1');   % Single channel
            tacho2  = isfield(io,  'tacho2');   % Dual channel
            tacho   = tacho1 || tacho2;

            % SyncManager configuration
            % Dynamically select required PDO's as configured. Some
            % are mandatory
            rv.SlaveConfig.sm = {...
                {2,0,ep8309.pdo2([true,true,pwm,current])},...
                {3,1,ep8309.pdo3([anain,anain,tacho1,tacho2,tacho2,true,pwm,true])},...
            };

            % Initialize some variables. In particular portname is
            % required so that the porname mask update does not barf
            sdo = [];
            rv.PortConfig.input = struct('portname',{});
            rv.PortConfig.output = struct('portname',{});

            % Digital input
            if digin
                pdoIdx = repmat([1,(2*anain + tacho1 + 2*tacho2),0,0], 8, 1);
                pdoIdx(:,3) = 0:7;
                rv.PortConfig.output(end+1).pdo = pdoIdx;
                rv.PortConfig.output(end).pdo_data_type = uint(1);
                rv.PortConfig.output(end).portname = 'DigIn';
            end

            % Digital output
            if digout
                pdoIdx = repmat([0,1,0,0], 8, 1);
                pdoIdx(:,3) = 0:7;
                rv.PortConfig.input(end+1).pdo = pdoIdx;
                rv.PortConfig.input(end).pdo_data_type = uint(1);
                rv.PortConfig.input(end).portname = 'DigOut';
            end

            % Analog input
            if anain
                pdoIdx = [1,0,9,0; 1,1,9,0];

                % Value port
                rv.PortConfig.output(end+1).pdo = pdoIdx;
                rv.PortConfig.output(end).pdo_data_type = sint(16);
                rv.PortConfig.output(end).portname = 'AIn';
                rv.PortConfig.output(end).gain   = {'IGain',   io.anain.gain};
                rv.PortConfig.output(end).offset = {'IOffset', io.anain.offset};
                rv.PortConfig.output(end).filter = {'IFilter', io.anain.filter};
                if isempty(io.anain.gain) && isempty(io.anain.offset)
                    if ~isempty(io.anain.filter)
                        rv.PortConfig.output(end).full_scale = 1;
                    end
                else
                    rv.PortConfig.output(end).full_scale = 2^15;
                end

                % Error port
                pdoIdx(:,3) = 4;
                rv.PortConfig.output(end+1).pdo = pdoIdx;
                rv.PortConfig.output(end).pdo_data_type = uint(1);
                rv.PortConfig.output(end).portname = 'AInErr';

                sdo = ep8309.appendSDO(sdo, ep8309.sdo.anain, io.anain.sdo);
            end

            % Current output
            if current
                pdoIdx = [0,2,0,0];
                rv.PortConfig.input(end+1).pdo = pdoIdx;
                rv.PortConfig.input(end).pdo_data_type = sint(16);
                rv.PortConfig.input(end).portname = 'AOut';
                if ~isempty(io.current.gain)
                    rv.PortConfig.input(end).gain = {'OGain', io.current.gain};
                    rv.PortConfig.input(end).full_scale = 2^15;
                end

                sdo = ep8309.appendSDO(sdo, ...
                                       ep8309.sdo.current, io.current.sdo);
            end

            % PWM ouptut
            if pwm

                % Value input port
                pdoIdx = [0,(2+current),5,0];
                rv.PortConfig.input(end+1).pdo = pdoIdx;
                rv.PortConfig.input(end).pdo_data_type = sint(16);
                rv.PortConfig.input(end).portname = 'PWM';
                if ~isempty(io.pwm.gain)
                    rv.PortConfig.input(end).gain = {'PWMGain', io.pwm.gain};
                    rv.PortConfig.input(end).full_scale = 2^15;
                end

                % Dithering, enable, reset input port
                pdoIdx = repmat([0,(2+current),5,0], 3, 1);
                pdoIdx(:,3) = [0,2,3];
                rv.PortConfig.input(end+1).pdo = pdoIdx;
                rv.PortConfig.input(end).pdo_data_type = uint(1);
                rv.PortConfig.input(end).portname = 'PWMCtl';

                % Status output
                pdoIdx = repmat([1,(2*anain + tacho1 + 2*tacho2 + 1),0,0], ...
                                3, 1);
                pdoIdx(:,3) = 2:4;
                rv.PortConfig.output(end+1).pdo = pdoIdx;
                rv.PortConfig.output(end).pdo_data_type = uint(1);
                rv.PortConfig.output(end).portname = 'PWMStat';

                sdo = ep8309.appendSDO(sdo, ep8309.sdo.pwm, io.pwm.sdo);
            end

            % Tacho input
            if tacho

                % Control input port
                pdoIdx = [0, 0, 1, 0];
                rv.PortConfig.input(end+1).pdo = pdoIdx;
                rv.PortConfig.input(end).pdo_data_type = uint(1);
                rv.PortConfig.input(end).portname = 'TachoCtl';

                % Single channel
                if tacho1
                    % Value
                    pdoIdx = [1, 2*anain, 10, 0];
                    rv.PortConfig.output(end+1).pdo = pdoIdx;
                    rv.PortConfig.output(end).pdo_data_type = sint(16);
                    rv.PortConfig.output(end).portname = 'Tacho';
                    rv.PortConfig.output(end).gain = io.tacho1.gain;
                    rv.PortConfig.output(end).full_scale = 1;

                    % ErrorA, StatusA, ErrorB, StatusB
                    pdoIdx = repmat(pdoIdx, 4, 1);
                    pdoIdx(:,3) = 1:4;
                    rv.PortConfig.output(end+1).pdo = pdoIdx;
                    rv.PortConfig.output(end).pdo_data_type = uint(1);
                    rv.PortConfig.output(end).portname = 'TachoStat';

                    sdo = ep8309.appendSDO(sdo,...
                                           ep8309.sdo.tacho1, io.tacho1.sdo);

                elseif tacho2   % Dual channel mode
                    % Value
                    pdoIdx = [1, 2*anain, 6, 0; 1, 2*anain+1, 6, 0];
                    rv.PortConfig.output(end+1).pdo = pdoIdx;
                    rv.PortConfig.output(end).pdo_data_type = sint(16);
                    rv.PortConfig.output(end).portname = 'Tacho';
                    rv.PortConfig.output(end).gain = io.tacho2.gain;
                    rv.PortConfig.output(end).full_scale = 1;

                    % Digital input
                    pdoIdx(:,3) = 0;
                    rv.PortConfig.output(end+1).pdo = pdoIdx;
                    rv.PortConfig.output(end).pdo_data_type = uint(1);
                    rv.PortConfig.output(end).portname = 'TachoStat';

                    sdo = ep8309.appendSDO(sdo,...
                                           ep8309.sdo.tacho2, io.tacho2.sdo);
                end
            end

            % Device status, Undervoltage Us, Up
            if devstat
                i = 2*anain + tacho1 + 2*tacho2 + 1 + pwm;
                pdoIdx = repmat([1,i,0,0], 2, 1);
                pdoIdx(:,3) = 0:1;
                rv.PortConfig.output(end+1).pdo = pdoIdx;
                rv.PortConfig.output(end).pdo_data_type = uint(1);
                rv.PortConfig.output(end).portname = 'DevStat';
            end

            % Now is time to format sdo as a cell array
            rv.SlaveConfig.sdo = num2cell(sdo);
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods (Static)
        %====================================================================
        function test(p)
            ei = EtherCATInfo(fullfile(p,'Beckhoff EP8xxx.xml'));
            for i = 1:size(ep8309.models,1)
                fprintf('Testing %s\n', ep8309.models{i,1});
                slave = ei.getSlave(ep8309.models{i,2},...
                        'revision', ep8309.models{i,3});
                model = ep8309.models{i,1};

                io.digin = 1;
                io.digout = 1;
                io.devstat = 1;

                io.anain.gain = [];
                io.anain.offset = [];
                io.anain.filter = [];
                io.anain.sdo = [hex2dec('8000'), hex2dec('06'), true;
                                hex2dec('8000'), hex2dec('15'), 3;
                                hex2dec('F800'), hex2dec('01'), 1;
                                hex2dec('F800'), hex2dec('02'), 1];

                % Single shaft mode
                io.tacho1.gain = 5;
                io.tacho1.sdo = [hex2dec('8031'), hex2dec('0B'), true;
                                 hex2dec('8031'), hex2dec('0C'), true;
                                 hex2dec('8031'), hex2dec('11'), 5;
                                 hex2dec('8031'), hex2dec('12'), 5;
                                 hex2dec('8031'), hex2dec('15'), 1];

                % Current
                io.current.gain = [];
                io.current.sdo = [hex2dec('8060'), hex2dec('05'), 1;
                                  hex2dec('8060'), hex2dec('13'), 4;
                                  hex2dec('8060'), hex2dec('14'), 4;
                                  hex2dec('F800'), hex2dec('08'), 1];

                rv = ep8309(model).configure(io);
                slave.testConfig(rv.SlaveConfig,rv.PortConfig);

                io = [];

                % dual shaft mode
                io.tacho2.gain = [5,6];
                io.tacho2.sdo = [hex2dec('8020'), hex2dec('11'), 4;
                                 hex2dec('8020'), hex2dec('12'), 4;
                                 hex2dec('8020'), hex2dec('15'), 1;
                                 hex2dec('8030'), hex2dec('11'), 5;
                                 hex2dec('8030'), hex2dec('12'), 5;
                                 hex2dec('8030'), hex2dec('15'), 2];
                % PWM
                io.pwm.gain = [];
                io.pwm.sdo = [hex2dec('8050'), hex2dec('03'), true;
                              hex2dec('8050'), hex2dec('04'), true;
                              hex2dec('8050'), hex2dec('05'), 1;
                              hex2dec('8050'), hex2dec('0D'), 8;
                              hex2dec('8050'), hex2dec('0E'), 8;
                              hex2dec('8050'), hex2dec('10'), 100;
                              hex2dec('8050'), hex2dec('12'), 250;
                              hex2dec('8050'), hex2dec('13'), 4;
                              hex2dec('8050'), hex2dec('14'), 50;
                              hex2dec('8050'), hex2dec('1E'), 64;
                              hex2dec('8050'), hex2dec('1F'), 5];

                rv = ep8309(model).configure(io);
                slave.testConfig(rv.SlaveConfig,rv.PortConfig);
            end
        end

        %====================================================================
        function updateAnalogIn
            %% Analog Input selection changed
            sdo = ep8309.sdo.anain;
            extra = strvcat('gain','offset','tau');

            if strcmp(get_param(gcbh,'anain'), 'on')
                ep8309.updateSlaveBlock(sdo, extra, sdo, extra)
            else
                ep8309.updateSlaveBlock(sdo, extra)
            end
        end

        %====================================================================
        function updateAnalogOut
            %% Analog Output selection changed
            sdo = [ep8309.sdo.current; ep8309.sdo.pwm];
            extra = 'gain2';

            switch get_param(gcbh,'anaout');
            case 'Current'
                ep8309.updateSlaveBlock(sdo, extra, ...
                                        ep8309.sdo.current, extra);
            case 'PWM'
                ep8309.updateSlaveBlock(sdo, extra, ...
                                        ep8309.sdo.pwm, extra);
            otherwise
                ep8309.updateSlaveBlock(sdo, extra);
            end
        end

        %====================================================================
        function updateTacho
            %% Tacho selection changed
            sdo = [ep8309.sdo.tacho1; ep8309.sdo.tacho2];

            switch get_param(gcbh,'tacho');
            case 'Single shaft'
                ep8309.updateSlaveBlock(sdo, [], ep8309.sdo.tacho1)
            case 'Dual shaft'
                ep8309.updateSlaveBlock(sdo, [], ep8309.sdo.tacho2)
            otherwise
                ep8309.updateSlaveBlock(sdo, [])
            end
        end

        %====================================================================
        function updateSlaveBlock(sdo, extra, visible, extra_visible)
            %% Helper function for all the update* functions above
            %   sdo     - Nx4 array of affected SDOs (Index, SubIndex)
            %   extra   - string array of other mask parameters
            %   visible - Nx4 array of SDO's that are visible
            %   extra_visible - string array of mask parameters that must
            %                   remain visible
            %
            % Any mask variable that is not in *visible will be hidden
            %
            if nargin < 4
                extra_visible = [];
            end

            if nargin < 3 || isempty(visible)
                visible = double.empty(0,2);
            end

            list = strvcat(extra, strcat('sdo_', dec2hex(sdo(:,1),4),...
                                            '_', dec2hex(sdo(:,2),2)));
            on = [ismember(extra, extra_visible, 'rows');
                  ismember(  sdo,       visible, 'rows')];

            EtherCATSlave.setVisible(list, on);
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods (Static, Access = private)
        function rv = appendSDO(sdo, list, candidates)
            %% Appends candidates to sdo, checks against list
            % whether they are allowed. List also delivers the
            % data type
            %   sdo     - Nx4 array Index, SubIndex, BitLen, Value
            %   list - Nx3 array Index, SubIndex, BitLen
            %   candidates  - Nx3 array Index, SubIndex, Value
            [tf, loc] = ismember(list(:,2), candidates(:,2), 'rows');
            rv = [sdo; [list(tf,:), candidates(loc(tf),3)]];
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties (Constant, Access = private)
        pdo2 = {...
            {hex2dec('1600'), [ % Tacho output ch.1, mandatory
                0,               0,               8;
                hex2dec('7020'), hex2dec('09'),   1;  % Reset Error
                0,               0,               7;
                ]}, ...
            {hex2dec('1601'), [ % DIG Outputs, mandatory
                hex2dec('7040'), hex2dec('01'),   1;
                hex2dec('7040'), hex2dec('02'),   1;
                hex2dec('7040'), hex2dec('03'),   1;
                hex2dec('7040'), hex2dec('04'),   1;
                hex2dec('7040'), hex2dec('05'),   1;
                hex2dec('7040'), hex2dec('06'),   1;
                hex2dec('7040'), hex2dec('07'),   1;
                hex2dec('7040'), hex2dec('08'),   1;
                0,               0,               8;
                ]}, ...
            {hex2dec('1602'), [                       % PWM Control excl 1603
                hex2dec('7050'), hex2dec('01'),   1;  % Enable dithering
                0,               0,               4;
                hex2dec('7050'), hex2dec('06'),   1;  % Enable
                hex2dec('7050'), hex2dec('07'),   1;  % Reset
                0,               0,               9;
                hex2dec('7050'), hex2dec('11'),  16;  % PWM Output
                ]}, ...
            {hex2dec('1603'), [                       % Analog outputs, excl 1602
                hex2dec('7060'), hex2dec('11'),  16;  % Value
                ]}, ...
        };

        pdo3 = {...
            {hex2dec('1a00'), [                       % Analog input ch.1
                hex2dec('6000'), hex2dec('01'),   1;  % Underrange
                hex2dec('6000'), hex2dec('02'),   1;  % Overrange
                hex2dec('6000'), hex2dec('03'),   2;  % Limit 1
                hex2dec('6000'), hex2dec('05'),   2;  % Limit 2
                hex2dec('6000'), hex2dec('07'),   1;  % Error
                0,               0,               6;
                hex2dec('6000'), hex2dec('0e'),   1;  % Sync error
                hex2dec('6000'), hex2dec('0f'),   1;  % TxPDO State
                hex2dec('6000'), hex2dec('10'),   1;  % TxPDO Toggle
                hex2dec('6000'), hex2dec('11'),  16;  % Value
                ]}, ...
            {hex2dec('1a01'), [                       % Analog input ch.2
                hex2dec('6010'), hex2dec('01'),   1;  % Underrange
                hex2dec('6010'), hex2dec('02'),   1;  % Overrange
                hex2dec('6010'), hex2dec('03'),   2;  % Limit 1
                hex2dec('6010'), hex2dec('05'),   2;  % Limit 2
                hex2dec('6010'), hex2dec('07'),   1;  % Error
                0,               0,               6;
                hex2dec('6010'), hex2dec('0e'),   1;  % Sync error
                hex2dec('6010'), hex2dec('0f'),   1;  % TxPDO State
                hex2dec('6010'), hex2dec('10'),   1;  % TxPDO Toggle
                hex2dec('6010'), hex2dec('11'),  16;  % Value
                ]}, ...
            {hex2dec('1a02'), [ % TACHO single shaft In, excl 1a03,1a04
                0,               0,               1;
                hex2dec('6037'), hex2dec('02'),   1;  % Error  IN A
                hex2dec('6037'), hex2dec('03'),   1;  % Status IN A
                hex2dec('6037'), hex2dec('04'),   1;  % Error  IN B
                hex2dec('6037'), hex2dec('05'),   1;  % Status IN B
                0,               0,               3;
                0,               0,               3;
                hex2dec('6037'), hex2dec('0c'),   1;  % Speed < Threshold
                0,               0,               3;
                hex2dec('6037'), hex2dec('10'),   1;  % TxPDO toggle
                hex2dec('6037'), hex2dec('11'),  16;  % Rotational Speed (INT)
                hex2dec('6037'), hex2dec('12'),  16;  % Rotational Direction (UINT)
                ]}, ...
            {hex2dec('1a03'), [ % TACHO dual shaft In Ch.1, excl 1a02
                hex2dec('6020'), hex2dec('01'),   1;  % Digital input status
                0,               0,               7;
                0,               0,               3;
                hex2dec('6020'), hex2dec('0c'),   1;  % Speed < Threshold
                0,               0,               3;
                hex2dec('6020'), hex2dec('10'),   1;  % TxPDO toggle
                hex2dec('6020'), hex2dec('11'),  16;  % Rotational Speed (INT)
                ]}, ...
            {hex2dec('1a04'), [ % TACHO dual shaft In Ch.2, excl 1a02
                hex2dec('6030'), hex2dec('01'),   1;  % Digital input status
                0,               0,               7;
                0,               0,               3;
                hex2dec('6030'), hex2dec('0c'),   1;  % Speed < Threshold
                0,               0,               3;
                hex2dec('6030'), hex2dec('10'),   1;  % TxPDO toggle
                hex2dec('6030'), hex2dec('11'),  16;  % Rotational Speed (INT)
                ]}, ...
            {hex2dec('1a05'), [ % Digital IN, mandatory
                hex2dec('6040'), hex2dec('01'),   1;
                hex2dec('6040'), hex2dec('02'),   1;
                hex2dec('6040'), hex2dec('03'),   1;
                hex2dec('6040'), hex2dec('04'),   1;
                hex2dec('6040'), hex2dec('05'),   1;
                hex2dec('6040'), hex2dec('06'),   1;
                hex2dec('6040'), hex2dec('07'),   1;
                hex2dec('6040'), hex2dec('08'),   1;
                0,               0,               8;
                ]}, ...
            {hex2dec('1a06'), [ % PWM Status
                0,               0,               1;
                0,               0,               3;
                hex2dec('6050'), hex2dec('05'),   1;  % Ready to enable
                hex2dec('6050'), hex2dec('06'),   1;  % Warning
                hex2dec('6050'), hex2dec('07'),   1;  % Error
                0,               0,               1;
                0,               0,               7;
                hex2dec('6050'), hex2dec('10'),   1;  % TxPDO toggle
                ]}, ...
            {hex2dec('1a08'), [ % DEV inputs, mandatory
                hex2dec('f611'), hex2dec('01'),   1;  % Undervoltage Us
                hex2dec('f611'), hex2dec('02'),   1;  % Undervoltage Up
                0,               0,              14;
                ]}, ...
            };

        % Available SDO's for the various modules
        sdo = struct('anain',   [hex2dec('8000'), hex2dec('06'),  8;
                                 hex2dec('8000'), hex2dec('15'), 16;
                                 hex2dec('F800'), hex2dec('01'), 16;
                                 hex2dec('F800'), hex2dec('02'), 16], ...
                     'tacho2',  [hex2dec('8020'), hex2dec('11'), 16;
                                 hex2dec('8020'), hex2dec('12'), 16;
                                 hex2dec('8020'), hex2dec('15'), 16;
                                 hex2dec('8030'), hex2dec('11'), 16;
                                 hex2dec('8030'), hex2dec('12'), 16;
                                 hex2dec('8030'), hex2dec('15'), 16], ...
                     'tacho1',  [hex2dec('8031'), hex2dec('0B'),  8;
                                 hex2dec('8031'), hex2dec('0C'),  8;
                                 hex2dec('8031'), hex2dec('11'), 16;
                                 hex2dec('8031'), hex2dec('12'), 16;
                                 hex2dec('8031'), hex2dec('15'), 16], ...
                     'pwm',     [hex2dec('8050'), hex2dec('03'),  8;
                                 hex2dec('8050'), hex2dec('04'),  8;
                                 hex2dec('8050'), hex2dec('05'),  8;
                                 hex2dec('8050'), hex2dec('0D'), 16;
                                 hex2dec('8050'), hex2dec('0E'), 16;
                                 hex2dec('8050'), hex2dec('10'),  8;
                                 hex2dec('8050'), hex2dec('12'), 16;
                                 hex2dec('8050'), hex2dec('13'), 16;
                                 hex2dec('8050'), hex2dec('14'), 16;
                                 hex2dec('8050'), hex2dec('1E'), 16;
                                 hex2dec('8050'), hex2dec('1F'),  8], ...
                     'current', [hex2dec('8060'), hex2dec('05'),  8;
                                 hex2dec('8060'), hex2dec('13'), 16;
                                 hex2dec('8060'), hex2dec('14'), 16;
                                 hex2dec('F800'), hex2dec('08'), 16]);

    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties (Constant)
        %    Name          ProductCode          RevisionNo
        models = {
            'EP8309-1022', hex2dec('20754052'), hex2dec('001003FE');
        };
    end

end
