classdef el2xxx < EtherCATSlave

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods
        %====================================================================
        function obj = el2xxx(id)
            if nargin > 0
                obj.slave = obj.find(id);
            end
        end

        %====================================================================
        function rv = configure(obj,vector,status)

            rv.SlaveConfig.vendor = 2;
            rv.SlaveConfig.description = obj.slave{1};
            rv.SlaveConfig.product  = obj.slave{2};

            pdo_count = obj.slave{4};

            pdo_idx = [obj.slave{5} + (0:pdo_count-1)];

            status = status && obj.slave{6};
            if status
                pdo_idx = [pdo_idx, obj.slave{6} + (0:pdo_count-1)];
            end

            pdo = obj.pdo(pdo_idx,:);

            sm_id = unique(pdo(:,1))';
            rv.SlaveConfig.sm = repmat({{0,0,{}}}, size(sm_id));

            for i = 1:numel(sm_id)
                p = arrayfun(@(i) {pdo(i,2), [pdo(i,[3,4]),1]},...
                        find(pdo(:,1) == sm_id(i))', ...
                        'UniformOutput', false);
                dir = status && (obj.pdo(obj.slave{6},1) == sm_id(i));
                dir = double(dir);
                rv.SlaveConfig.sm{i} = {sm_id(i), dir, p};
            end

            rv.PortConfig.input  = obj.configurePorts('O',...
                                obj.findPdoEntries(rv.SlaveConfig.sm,0),...
                                uint(1),vector);

            if status
                rv.PortConfig.output = obj.configurePorts('I',...
                        obj.findPdoEntries(rv.SlaveConfig.sm,1),...
                        uint(1),vector);
            else
                rv.PortConfig.output = [];
            end
        end
    end


    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods (Static)
        %====================================================================
        function modelChanged()
            obj = el2xxx(get_param(gcbh,'model'));
            obj.setEnable('diag', obj.slave{6});
            obj.updateRevision();
        end

        %====================================================================
        function test(p)
            ei = EtherCATInfo(fullfile(p,'Beckhoff EL2xxx.xml'));
            for i = 1:size(el2xxx.models,1)
                fprintf('Testing %s\n', el2xxx.models{i,1});
                slave = ei.getSlave(el2xxx.models{i,2},...
                        'revision', el2xxx.models{i,3});

                rv = el2xxx(el2xxx.models{i,1}).configure(i&1,0);
                slave.testConfig(rv.SlaveConfig,rv.PortConfig);

                rv = el2xxx(el2xxx.models{i,1}).configure(i&1,1);
                slave.testConfig(rv.SlaveConfig,rv.PortConfig);
            end
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties (Access = private, Constant)

        %      Sm   PdoIdx          EntryIdx      SubIdx
        pdo = [ 0, hex2dec('1600'), hex2dec('7000'), 1;    %   1
                0, hex2dec('1601'), hex2dec('7010'), 1;    %   2
                0, hex2dec('1602'), hex2dec('7020'), 1;    %   3
                0, hex2dec('1603'), hex2dec('7030'), 1;    %   4
                0, hex2dec('1604'), hex2dec('7040'), 1;    %   5
                0, hex2dec('1605'), hex2dec('7050'), 1;    %   6
                0, hex2dec('1606'), hex2dec('7060'), 1;    %   7
                0, hex2dec('1607'), hex2dec('7070'), 1;    %   8
                1, hex2dec('1608'), hex2dec('7080'), 1;    %   9
                1, hex2dec('1609'), hex2dec('7090'), 1;    %  10
                1, hex2dec('160a'), hex2dec('70a0'), 1;    %  11
                1, hex2dec('160b'), hex2dec('70b0'), 1;    %  12
                1, hex2dec('160c'), hex2dec('70c0'), 1;    %  13
                1, hex2dec('160d'), hex2dec('70d0'), 1;    %  14
                1, hex2dec('160e'), hex2dec('70e0'), 1;    %  15
                1, hex2dec('160f'), hex2dec('70f0'), 1;    %  16

                1, hex2dec('1a00'), hex2dec('6000'), 1;    %  17
                1, hex2dec('1a01'), hex2dec('6010'), 1;    %  18
                1, hex2dec('1a02'), hex2dec('6020'), 1;    %  19
                1, hex2dec('1a03'), hex2dec('6030'), 1;    %  20

                0, hex2dec('1600'), hex2dec('3001'), 1;    %  21
                0, hex2dec('1601'), hex2dec('3001'), 2;    %  22
                0, hex2dec('1602'), hex2dec('3001'), 3;    %  23
                0, hex2dec('1603'), hex2dec('3001'), 4;    %  24

                1, hex2dec('1a00'), hex2dec('3101'), 1;    %  25
                1, hex2dec('1a01'), hex2dec('3101'), 2];   %  26
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties (Constant)

        %   Model       ProductCode          Revision          N, Rx, Tx
        models = {...
            'EL2002', hex2dec('07d23052'), hex2dec('00100000'), 2, 1,  0;
            'EL2004', hex2dec('07d43052'), hex2dec('00100000'), 4, 1,  0;
            'EL2008', hex2dec('07d83052'), hex2dec('00100000'), 8, 1,  0;
            'EL2022', hex2dec('07e63052'), hex2dec('00100000'), 2, 1,  0;
            'EL2024', hex2dec('07e83052'), hex2dec('00100000'), 4, 1,  0;
            'EL2024-0010', ...
                      hex2dec('07e83052'), hex2dec('0010000A'), 4, 1,  0;
            'EL2032', hex2dec('07f03052'), hex2dec('00100000'), 2, 1, 17;
            'EL2034', hex2dec('07f23052'), hex2dec('00100000'), 4, 1, 17;
            'EL2042', hex2dec('07fa3052'), hex2dec('00100000'), 2, 1,  0;
            'EL2084', hex2dec('08243052'), hex2dec('00100000'), 4, 1,  0;
            'EL2088', hex2dec('08283052'), hex2dec('00100000'), 8, 1,  0;
            'EL2124', hex2dec('084c3052'), hex2dec('00100000'), 4, 1,  0;
            'EL2202', hex2dec('089a3052'), hex2dec('00100000'), 2, 1,  0;
            'EL2202-0100', ...
                      hex2dec('089a3052'), hex2dec('00100064'), 2, 1,  0;
            'EL2602', hex2dec('0a2a3052'), hex2dec('00100000'), 2, 1,  0;
            'EL2612', hex2dec('0a343052'), hex2dec('00100000'), 2, 1,  0;
            'EL2622', hex2dec('0a3e3052'), hex2dec('00100000'), 2, 1,  0;
            'EL2624', hex2dec('0a403052'), hex2dec('00100000'), 4, 1,  0;
            'EL2652', hex2dec('0a5c3052'), hex2dec('00100000'), 2, 1,  0;
            'EL2712', hex2dec('0a983052'), hex2dec('00100000'), 2, 1,  0;
            'EL2722', hex2dec('0aa23052'), hex2dec('00100000'), 2, 1,  0;
            'EL2722-0010', ...
                      hex2dec('0aa23052'), hex2dec('0010000A'), 2, 1,  0;
            'EL2732', hex2dec('0aac3052'), hex2dec('00100000'), 2, 1,  0;
            'EL2798', hex2dec('0aee3052'), hex2dec('00100000'), 8, 1,  0;
            'EL2808', hex2dec('0af83052'), hex2dec('00100000'), 8, 1,  0;
            'EL2809', hex2dec('0af93052'), hex2dec('00100000'),16, 1,  0;
            'EL2828', hex2dec('0b0c3052'), hex2dec('00100000'), 8, 1,  0;
            'EL2872', hex2dec('0b383052'), hex2dec('00100000'), 8, 1,  0;
            'EL2889', hex2dec('0b493052'), hex2dec('00100000'),16, 1,  0;
            'EL2872-0010', ...
                      hex2dec('0B383052'), hex2dec('0010000A'), 8, 1,  0;
            'EL2004-0000-0000', ...
                      hex2dec('07d43052'), hex2dec('00000000'), 4, 21, 0;
            'EL2032-0000-0000', ...
                      hex2dec('07f03052'), hex2dec('00000000'), 2, 21,25;
        };

    end
end
