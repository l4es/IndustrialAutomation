classdef el4xxx < EtherCATSlave

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods
        %====================================================================
        function obj = el4xxx(id)
            if nargin > 0
                obj.slave = obj.find(id);
            end
        end

        %====================================================================
        function rv = configure(obj,vector,scale,dc)

            rv.SlaveConfig.vendor = 2;
            rv.SlaveConfig.description = obj.slave{1};
            rv.SlaveConfig.product = obj.slave{2};
            rv.function = obj.slave{5};

            pdo_count = str2double(obj.slave{1}(6));

            rv.SlaveConfig.sm = ...
                {{2,0, arrayfun(@(i) {obj.pdo(i,1) [obj.pdo(i,[2,3]), 16]}, ...
                                (0:pdo_count-1) + obj.slave{4},...
                                'UniformOutput', false)}};

            if dc(1) > 2
                rv.SlaveConfig.dc = dc(2:end);
            else
                rv.SlaveConfig.dc = obj.dc{dc(1),2};
            end

            rv.PortConfig.input  = obj.configurePorts('O',...
                                obj.findPdoEntries(rv.SlaveConfig.sm,0),...
                                sint(16),vector,scale);
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods (Static)
        %====================================================================
        function test(p)
            ei = EtherCATInfo(fullfile(p,'Beckhoff EL4xxx.xml'));
            for i = 1:size(el4xxx.models,1)
                fprintf('Testing %s\n', el4xxx.models{i,1});
                slave = ei.getSlave(el4xxx.models{i,2},...
                        'revision', el4xxx.models{i,3});

                rv = el4xxx(el4xxx.models{i,1}).configure(i&1,...
                        EtherCATSlave.configureScale(2^15,''),2);
                slave.testConfig(rv.SlaveConfig,rv.PortConfig);

                rv = el4xxx(el4xxx.models{i,1}).configure(i&1,...
                        EtherCATSlave.configureScale(2^15,'8'),2);
                slave.testConfig(rv.SlaveConfig,rv.PortConfig);
            end
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties (Constant)

                % PdoIdx         PdoEntryIdx    SubIdx  BitLen
        pdo = [hex2dec('1600'), hex2dec('7000'), 17;
               hex2dec('1601'), hex2dec('7010'), 17;
               hex2dec('1602'), hex2dec('7020'), 17;
               hex2dec('1603'), hex2dec('7030'), 17;
               hex2dec('1604'), hex2dec('7040'), 17;
               hex2dec('1605'), hex2dec('7050'), 17;
               hex2dec('1606'), hex2dec('7060'), 17;
               hex2dec('1607'), hex2dec('7070'), 17;
               hex2dec('1600'), hex2dec('3001'), 17;
               hex2dec('1601'), hex2dec('3002'), 17;
               hex2dec('1600'), hex2dec('6411'),  1;
               hex2dec('1601'), hex2dec('6411'),  2;
               hex2dec('1600'), hex2dec('7000'),  1;
               hex2dec('1601'), hex2dec('7010'),  1;
               hex2dec('1602'), hex2dec('7020'),  1;
               hex2dec('1603'), hex2dec('7030'),  1;
               hex2dec('1604'), hex2dec('7040'),  1;
               hex2dec('1605'), hex2dec('7050'),  1;
               hex2dec('1606'), hex2dec('7060'),  1;
               hex2dec('1607'), hex2dec('7070'),  1];

        %   Model     ProductCode          Revision     IndexOffset, function
        models = {
            'EL4001', hex2dec('0fa13052'), hex2dec('00120000'),13,  '0-10V';
            'EL4002', hex2dec('0fa23052'), hex2dec('00130000'), 1,  '0-10V';
            'EL4004', hex2dec('0fa43052'), hex2dec('00130000'), 1,  '0-10V';
            'EL4008', hex2dec('0fa83052'), hex2dec('00130000'), 1,  '0-10V';
            'EL4011', hex2dec('0fab3052'), hex2dec('00130000'), 1, '0-20mA';
            'EL4012', hex2dec('0fac3052'), hex2dec('00130000'), 1, '0-20mA';
            'EL4014', hex2dec('0fae3052'), hex2dec('00130000'), 1, '0-20mA';
            'EL4018', hex2dec('0fb23052'), hex2dec('00130000'), 1, '0-20mA';
            'EL4021', hex2dec('0fb53052'), hex2dec('00130000'), 1, '4-20mA';
            'EL4022', hex2dec('0fb63052'), hex2dec('00130000'), 1, '4-20mA';
            'EL4024', hex2dec('0fb83052'), hex2dec('00130000'), 1, '4-20mA';
            'EL4028', hex2dec('0fbc3052'), hex2dec('00130000'), 1, '4-20mA';
            'EL4031', hex2dec('0fbf3052'), hex2dec('00130000'), 1, '+/-10V';
            'EL4032', hex2dec('0fc03052'), hex2dec('00130000'), 1, '+/-10V';
            'EL4034', hex2dec('0fc23052'), hex2dec('00130000'), 1, '+/-10V';
            'EL4038', hex2dec('0fc63052'), hex2dec('00130000'), 1, '+/-10V';
            'EL4102', hex2dec('10063052'), hex2dec('03fa0000'),11,  '0-10V';
            'EL4104', hex2dec('10083052'), hex2dec('03f90000'), 1,  '0-10V';
            'EL4112', hex2dec('10103052'), hex2dec('03fb0000'),11, '0-20mA';
            'EL4112-0010', ...
                      hex2dec('10103052'), hex2dec('03fb000a'),11, '+-10mA';
            'EL4114', hex2dec('10123052'), hex2dec('03f90000'), 1, '0-20mA';
            'EL4122', hex2dec('101a3052'), hex2dec('03fa0000'),11, '4-20mA';
            'EL4124', hex2dec('101c3052'), hex2dec('03f90000'), 1, '4-20mA';
            'EL4132', hex2dec('10243052'), hex2dec('03fa0000'),11, '+/-10V';
            'EL4134', hex2dec('10263052'), hex2dec('03f90000'), 1, '+/-10V';
            'EL4001-0000-0016', ...
                      hex2dec('0fa13052'), hex2dec('00100000'), 1,  '0-10V';
            'EL4002-0000-0018', ...
                      hex2dec('0fa23052'), hex2dec('00120000'),13,  '0-10V';
            'EL4002-0000-0016', ...
                      hex2dec('0fa23052'), hex2dec('00100000'), 1,  '0-10V';
            'EL4004-0000-0018', ...
                      hex2dec('0fa43052'), hex2dec('00120000'),13,  '0-10V';
            'EL4004-0000-0016', ...
                      hex2dec('0fa43052'), hex2dec('00100000'), 1,  '0-10V';
            'EL4008-0000-0018', ...
                      hex2dec('0fa83052'), hex2dec('00120000'),13,  '0-10V';
            'EL4008-0000-0016', ...
                      hex2dec('0fa83052'), hex2dec('00100000'), 1,  '0-10V';
            'EL4011-0000-0018', ...
                      hex2dec('0fab3052'), hex2dec('00120000'),13, '0-20mA';
            'EL4011-0000-0016', ...
                      hex2dec('0fab3052'), hex2dec('00100000'), 1, '0-20mA';
            'EL4012-0000-0018', ...
                      hex2dec('0fac3052'), hex2dec('00120000'),13, '0-20mA';
            'EL4012-0000-0016', ...
                      hex2dec('0fac3052'), hex2dec('00100000'), 1, '0-20mA';
            'EL4014-0000-0018', ...
                      hex2dec('0fae3052'), hex2dec('00120000'),13, '0-20mA';
            'EL4014-0000-0016', ...
                      hex2dec('0fae3052'), hex2dec('00100000'), 1, '0-20mA';
            'EL4018-0000-0018', ...
                      hex2dec('0fb23052'), hex2dec('00120000'),13, '0-20mA';
            'EL4018-0000-0016', ...
                      hex2dec('0fb23052'), hex2dec('00100000'), 1, '0-20mA';
            'EL4021-0000-0018', ...
                      hex2dec('0fb53052'), hex2dec('00120000'),13, '4-20mA';
            'EL4021-0000-0016', ...
                      hex2dec('0fb53052'), hex2dec('00100000'), 1, '4-20mA';
            'EL4022-0000-0018', ...
                      hex2dec('0fb63052'), hex2dec('00120000'),13, '4-20mA';
            'EL4022-0000-0016', ...
                      hex2dec('0fb63052'), hex2dec('00100000'), 1, '4-20mA';
            'EL4024-0000-0018', ...
                      hex2dec('0fb83052'), hex2dec('00120000'),13, '4-20mA';
            'EL4024-0000-0016', ...
                      hex2dec('0fb83052'), hex2dec('00100000'), 1, '4-20mA';
            'EL4028-0000-0018', ...
                      hex2dec('0fbc3052'), hex2dec('00120000'),13, '4-20mA';
            'EL4028-0000-0016', ...
                      hex2dec('0fbc3052'), hex2dec('00100000'), 1, '4-20mA';
            'EL4031-0000-0018', ...
                      hex2dec('0fbf3052'), hex2dec('00120000'),13, '+/-10V';
            'EL4031-0000-0016', ...
                      hex2dec('0fbf3052'), hex2dec('00100000'), 1, '+/-10V';
            'EL4032-0000-0018', ...
                      hex2dec('0fc03052'), hex2dec('00120000'),13, '+/-10V';
            'EL4032-0000-0016', ...
                      hex2dec('0fc03052'), hex2dec('00100000'), 1, '+/-10V';
            'EL4034-0000-0018', ...
                      hex2dec('0fc23052'), hex2dec('00120000'),13, '+/-10V';
            'EL4034-0000-0016', ...
                      hex2dec('0fc23052'), hex2dec('00100000'), 1, '+/-10V';
            'EL4038-0000-0018', ...
                      hex2dec('0fc63052'), hex2dec('00120000'),13, '+/-10V';
            'EL4038-0000-0016', ...
                      hex2dec('0fc63052'), hex2dec('00100000'), 1, '+/-10V';
        };

        dc = {'Sm-Synchron', [           0,   0, 0, 0, 0, 0, 0, 1,     0, 0];
              'DC-Synchron', [hex2dec('700'), 0, 1, 0, 0, 0, 0, 1, 10000, 0]};
    end
end
