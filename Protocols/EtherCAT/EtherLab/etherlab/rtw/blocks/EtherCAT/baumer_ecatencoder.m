classdef baumer_ecatencoder < EtherCATSlave

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods
        %====================================================================
        function obj = baumer_ecatencoder(id)
            if nargin > 0
                obj.slave = obj.find(id);
            end
        end

        %====================================================================
        function rv = configure(obj, direction, dc_config)

            rv.SlaveConfig.vendor = hex2dec('516');
            rv.SlaveConfig.description = obj.slave{1};
            rv.SlaveConfig.product = obj.slave{2};
            rv.SlaveConfig.revision = obj.slave{3};

            rv.SlaveConfig.sm = obj.sm(1);

            rv.PortConfig.output = struct('pdo', [0,0,0,0], ...
                                          'pdo_data_type', uint(32));

            rv.SlaveConfig.sdo = {hex2dec('6000'),0,16,direction + 4};

            if obj.slave{4} && dc_config(1) > 1
                if dc_config(1) > 2
                    rv.SlaveConfig.dc = dc_config(2:end);
                else
                    rv.SlaveConfig.dc = obj.dc(dc_config(1),:);
                end
            end
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods (Static)

        %====================================================================
        function modelChanged(obj)
            obj = baumer_ecatencoder(get_param(gcbh,'model'));
            obj.updateDC()
            obj.updateRevision()
        end

        %====================================================================
        function updateDC()
            obj = baumer_ecatencoder(get_param(gcbh, 'model'));
            obj.updateDCVisibility(obj.slave{4});
        end

        %====================================================================
        function test(f)
            ei = EtherCATInfo(f);
            for i = 1:size(baumer_ecatencoder.models,1)
                fprintf('Testing %s\n', baumer_ecatencoder.models{i,1});
                slave = baumer_ecatencoder(baumer_ecatencoder.models{i,1});
                rv = slave.configure(i&1,2:12);
                slave = ei.getSlave(baumer_ecatencoder.models{i,2},...
                        'revision', baumer_ecatencoder.models{i,3});
                slave.testConfig(rv.SlaveConfig);
            end
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties (Constant, Access = private)
        sm = {{3, 1, {{hex2dec('1A00') [hex2dec('6004') 0 32]}}}}
        dc = [             0,  0,0,0,0,0,0,0,0,0;
                hex2dec('300'),0,0,0,0,0,0,0,0,0]
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties (Constant)

        %   Model       ProductCode Revision DC
        models = {...
            'BT ATD4',     1,          2, false;
            'BT ATD2',     2,          2,  true;
            'BT ATD2_PoE', 3,          2,  true;
            };
    end  % properties
end
