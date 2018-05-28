classdef ep1xxx < EtherCATSlave

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods
        %====================================================================
        function obj = ep1xxx(id)
            if nargin > 0
                obj.slave = obj.find(id);
            end
        end

        %====================================================================
        function rv = configure(obj,vector)

            rv.SlaveConfig.vendor = 2;
            rv.SlaveConfig.description = obj.slave{1};
            rv.SlaveConfig.product = obj.slave{2};
            rv.function = obj.slave{7};

            pdo = obj.pdo((0:obj.slave{6}-1) + obj.slave{4},:);

            if obj.slave{4}
                rv.SlaveConfig.sm = {{obj.slave{5},1, ...
                    arrayfun(@(i) {pdo(i,1) [pdo(i,2),1,pdo(i,3)]}, ...
                             1:size(pdo,1), 'UniformOutput', false)}};

                entries = obj.findPdoEntries(rv.SlaveConfig.sm,1);
            else
                entries = [];
            end

            if isempty(entries)
                rv.PortConfig = [];
            else
                rv.PortConfig.output = obj.configurePorts('I',...
                                    entries,uint(entries(1,7)),vector);
            end
        end
    end


    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods (Static)
        %====================================================================
        function test(p)
            ei = EtherCATInfo(fullfile(p,'Beckhoff EP1xxx.xml'));
            for i = 1:size(ep1xxx.models,1)
                fprintf('Testing %s\n', ep1xxx.models{i,1});
                slave = ei.getSlave(ep1xxx.models{i,2},...
                        'revision', ep1xxx.models{i,3});

                rv = ep1xxx(ep1xxx.models{i,1}).configure(i&1);
                slave.testConfig(rv.SlaveConfig,rv.PortConfig);
            end
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties (Access = private, Constant)

                % PdoIdx        PdoSubIdx       BitLen
        pdo = [ hex2dec('1a00'),  hex2dec('6000'),  1;
                hex2dec('1a01'),  hex2dec('6010'),  1;
                hex2dec('1a02'),  hex2dec('6020'),  1;
                hex2dec('1a03'),  hex2dec('6030'),  1;
                hex2dec('1a04'),  hex2dec('6040'),  1;
                hex2dec('1a05'),  hex2dec('6050'),  1;
                hex2dec('1a06'),  hex2dec('6060'),  1;
                hex2dec('1a07'),  hex2dec('6070'),  1;
                hex2dec('1a08'),  hex2dec('6080'),  1;
                hex2dec('1a09'),  hex2dec('6090'),  1;
                hex2dec('1a0a'),  hex2dec('60a0'),  1;
                hex2dec('1a0b'),  hex2dec('60b0'),  1;
                hex2dec('1a0c'),  hex2dec('60c0'),  1;
                hex2dec('1a0d'),  hex2dec('60d0'),  1;
                hex2dec('1a0e'),  hex2dec('60e0'),  1;
                hex2dec('1a0f'),  hex2dec('60f0'),  1;
                hex2dec('1a00'),  hex2dec('f600'), 16];
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties (Constant)

        %   Model, ProductCode, Revision, PdoStartRow, Count, Description
        models = {
            'EP1008-0001', ...
                hex2dec('03f04052'),hex2dec('00100001'), 1, 0, 8, '8 Ch DigIn';
            'EP1008-0002', ...
                hex2dec('03f04052'),hex2dec('00100002'), 1, 0, 8, '8 Ch DigIn';
            'EP1008-0022', ...
                hex2dec('03f04052'),hex2dec('00100016'), 1, 0, 8, '8 Ch DigIn';
            'EP1018-0001', ...
                hex2dec('03fa4052'),hex2dec('00100001'), 1, 0, 8, '8 Ch DigIn';
            'EP1018-0002', ...
                hex2dec('03fa4052'),hex2dec('00100002'), 1, 0, 8, '8 Ch DigIn';
            'EP1098-0001', ...
                hex2dec('044a4052'),hex2dec('00100001'), 1, 0, 8, '8 Ch DigIn';
            'EP1111'     , ...
                hex2dec('04574052'),hex2dec('00100000'),17, 3, 1, 'ID-Switch';
            'EP1122-0001', ...
                hex2dec('04624052'),hex2dec('00110001'), 0, 0, 0, 'Junction';
            'EP1809-0021', ...
                hex2dec('07114052'),hex2dec('00100015'), 1, 0,16, '16 Ch DigIn';
            'EP1809-0022', ...
                hex2dec('07114052'),hex2dec('00110016'), 1, 0,16, '16 Ch DigIn';
            'EP1819-0021', ...
                hex2dec('071b4052'),hex2dec('00100015'), 1, 0,16, '16 Ch DigIn';
            'EP1819-0022', ...
                hex2dec('071b4052'),hex2dec('00100016'), 1, 0,16, '16 Ch DigIn';
        };
            %'EP1258-0001', ...
            %    hex2dec('04ea4052'),hex2dec('00110001'), 1,'8 Ch DigIn';
            %'EP1258-0002', ...
            %    hex2dec('04ea4052'),hex2dec('00110002'), 1,'8 Ch DigIn';
            %'EP1816-0008', ...
            %    hex2dec('07184052'),hex2dec('00100016'), 1,'16 Ch DigIn';

    end
end
