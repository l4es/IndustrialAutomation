classdef ep31xx_1 < EtherCATSlave


    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods
        %====================================================================
        function obj = ep31xx_1(id)
            if nargin > 0
                obj.slave = obj.find(id);
            end
        end

        %====================================================================
        function rv = configure(obj,status,vector,scale,dc,filter,type)

            rv.SlaveConfig.vendor = 2;
            rv.SlaveConfig.description = obj.slave{1};
            rv.SlaveConfig.product = obj.slave{2};

            % PdoCount is needed to configure SDO
            pdo_count = str2double(obj.slave{1}(6));

            % Configure SyncManager
            if status
                pdo_idx = obj.slave{5};
                value_idx = 10;
            else
                pdo_idx = 1;
                value_idx = 0;
            end
            rv.SlaveConfig.sm = {{3,1,obj.sm(pdo_idx + (0:pdo_count-1))}};

            % Value output
            pdo = repmat([0,0,value_idx,0], pdo_count, 1);
            pdo(:,2) = 0:pdo_count-1;
            rv.PortConfig.output = obj.configurePorts('Ch.',...
                           pdo,sint(16),vector,scale);

            % Status output
            if status
                pdo(:,3) = 4;

                if vector
                    n = 1;
                else
                    n = pdo_count;
                end
                
                rv.PortConfig.output(end+(1:n)) = el31xx_1.configurePorts(...
                        'St.',pdo,uint(1),vector,isa(scale,'struct'));
            end

            % Digital output for EP3182
            if obj.slave{4}
                pdo(:,1) = 1;
                pdo(:,2) = 0;
                pdo(:,3) = 0:pdo_count-1;
                rv.SlaveConfig.sm{2} = {2,0,obj.sm(obj.slave{4})};
                rv.PortConfig.input = obj.configurePorts('DO',...
                               pdo, uint(1),vector);
            end
                      
            % Distributed clock
            if dc(1) > 3
                rv.SlaveConfig.dc = dc(2:11);
            else
                rv.SlaveConfig.dc = obj.dc{dc(1),2};
            end

            rv.SlaveConfig.sdo = {};

            % Configure filter
            if filter > 1
                rv.SlaveConfig.sdo = {
                    hex2dec('8000'),hex2dec( '6'), 8, 1;
                    hex2dec('8000'),hex2dec('15'),16, filter-2;
                };
            end

            % Configure input type (newer models only, using F800)
            type_code = [0,1,2,6];
            for i = 1:pdo_count
                rv.SlaveConfig.sdo(end+1,:) = ...
                    {hex2dec('F800'),i,16,type_code(type(i))};
            end
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods (Static)
        %====================================================================
        function modelChanged()
            obj = ep31xx_1(get_param(gcbh,'model'));
            obj.setEnable({'type_3','type_4'}, obj.slave{1}(6) == '4');
            obj.updateRevision()
        end

        %====================================================================
        function updateFilter
            filter = ~strcmp(get_param(gcbh,'filter'),'Off');
            if filter
                set_param(gcbh,'dc_mode','FreeRun/SM-Synchron')
                EtherCATSlave.updateCustomDCEnable
                EtherCATSlave.setEnable('dc_mode',false)
            else
                EtherCATSlave.setEnable('dc_mode',true)
            end
        end

        %====================================================================
        function test(p)
            ei = EtherCATInfo(fullfile(p,'Beckhoff EP3xxx.xml'));
            for i = 1:size(ep31xx_1.models,1)
                fprintf('Testing %s\n', ep31xx_1.models{i,1});
                slave = ei.getSlave(ep31xx_1.models{i,2},...
                        'revision', ep31xx_1.models{i,3});

                rv = ep31xx_1(ep31xx_1.models{i,1}).configure(0,0,...
                        EtherCATSlave.configureScale(2^15,'3'),1,1,1:4);
                slave.testConfig(rv.SlaveConfig,rv.PortConfig);

                % Test with status and vector true and various DC
                rv = ep31xx_1(ep31xx_1.models{i,1}).configure(1,1,...
                        EtherCATSlave.configureScale(2^15,''),1+rem(i,3),1,1:4);
                slave.testConfig(rv.SlaveConfig,rv.PortConfig);
            end
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties (Access = private, Constant)

        sm = {{hex2dec('1a01'), [hex2dec('6000'), 17, 16]}, ...
              {hex2dec('1a03'), [hex2dec('6010'), 17, 16]}, ...
              {hex2dec('1a05'), [hex2dec('6020'), 17, 16]}, ...
              {hex2dec('1a07'), [hex2dec('6030'), 17, 16]}, ...
              {hex2dec('1600'), [hex2dec('7020'),  1,  1; % Digital output
                                 hex2dec('7020'),  2,  1;
                                             0  ,  0, 14]}...
              {hex2dec('1a00'), [hex2dec('6000') ,  1,  1;      %6  With status
                                 hex2dec('6000') ,  2,  1;
                                 hex2dec('6000') ,  3,  2;
                                 hex2dec('6000') ,  5,  2;
                                 hex2dec('6000') ,  7,  1;
                                               0 ,  0,  1;
                                               0 ,  0,  5;
                                 hex2dec('1c33') , 32,  1;
                                 hex2dec('1800') ,  7,  1;
                                 hex2dec('1800') ,  9,  1;
                                 hex2dec('6000') , 17, 16]},...
              {hex2dec('1a02'), [hex2dec('6010') ,  1,  1;      %7
                                 hex2dec('6010') ,  2,  1;
                                 hex2dec('6010') ,  3,  2;
                                 hex2dec('6010') ,  5,  2;
                                 hex2dec('6010') ,  7,  1;
                                               0 ,  0,  1;
                                               0 ,  0,  5;
                                 hex2dec('1c33') , 32,  1;
                                 hex2dec('1802') ,  7,  1;
                                 hex2dec('1802') ,  9,  1;
                                 hex2dec('6010') , 17, 16]},...
              {hex2dec('1a04'), [hex2dec('6020') ,  1,  1;      %8
                                 hex2dec('6020') ,  2,  1;
                                 hex2dec('6020') ,  3,  2;
                                 hex2dec('6020') ,  5,  2;
                                 hex2dec('6020') ,  7,  1;
                                               0 ,  0,  1;
                                               0 ,  0,  5;
                                 hex2dec('1c33') , 32,  1;
                                 hex2dec('1804') ,  7,  1;
                                 hex2dec('1804') ,  9,  1;
                                 hex2dec('6020') , 17, 16]},...
              {hex2dec('1a06'), [hex2dec('6030') ,  1,  1;      %9
                                 hex2dec('6030') ,  2,  1;
                                 hex2dec('6030') ,  3,  2;
                                 hex2dec('6030') ,  5,  2;
                                 hex2dec('6030') ,  7,  1;
                                               0 ,  0,  1;
                                               0 ,  0,  5;
                                 hex2dec('1c33') , 32,  1;
                                 hex2dec('1806') ,  7,  1;
                                 hex2dec('1806') ,  9,  1;
                                 hex2dec('6030') , 17, 16]},...
              {hex2dec('1a00'), [hex2dec('6000') ,  1,  1;      %10
                                 hex2dec('6000') ,  2,  1;
                                 hex2dec('6000') ,  3,  2;
                                 hex2dec('6000') ,  5,  2;
                                 hex2dec('6000') ,  7,  1;
                                               0 ,  0,  1;
                                               0 ,  0,  5;
                                 hex2dec('6000') , 14,  1;
                                 hex2dec('6000') , 15,  1;
                                 hex2dec('6000') , 16,  1;
                                 hex2dec('6000') , 17, 16]},...
              {hex2dec('1a02'), [hex2dec('6010') ,  1,  1;      %11
                                 hex2dec('6010') ,  2,  1;
                                 hex2dec('6010') ,  3,  2;
                                 hex2dec('6010') ,  5,  2;
                                 hex2dec('6010') ,  7,  1;
                                               0 ,  0,  1;
                                               0 ,  0,  5;
                                 hex2dec('6010') , 14,  1;
                                 hex2dec('6010') , 15,  1;
                                 hex2dec('6010') , 16,  1;
                                 hex2dec('6010') , 17, 16]},...
              {hex2dec('1a04'), [hex2dec('6020') ,  1,  1;      %12
                                 hex2dec('6020') ,  2,  1;
                                 hex2dec('6020') ,  3,  2;
                                 hex2dec('6020') ,  5,  2;
                                 hex2dec('6020') ,  7,  1;
                                               0 ,  0,  1;
                                               0 ,  0,  5;
                                 hex2dec('6020') , 14,  1;
                                 hex2dec('6020') , 15,  1;
                                 hex2dec('6020') , 16,  1;
                                 hex2dec('6020') , 17, 16]},...
              {hex2dec('1a06'), [hex2dec('6030') ,  1,  1;      %13
                                 hex2dec('6030') ,  2,  1;
                                 hex2dec('6030') ,  3,  2;
                                 hex2dec('6030') ,  5,  2;
                                 hex2dec('6030') ,  7,  1;
                                               0 ,  0,  1;
                                               0 ,  0,  5;
                                 hex2dec('6030') , 14,  1;
                                 hex2dec('6030') , 15,  1;
                                 hex2dec('6030') , 16,  1;
                                 hex2dec('6030') , 17, 16]},...
        };

        dc = {'Free Run', [         0,   0, 0,     0, 0, 0, 0, 1,     0, 0];
              'DC-Synchron', ...
                        [hex2dec('700'), 0, 1,     0, 0, 0, 0, 1, 20000, 0];
              'DC-Synchron (input based)',...
                        [hex2dec('700'), 0, 1, -5000, 0, 1, 0, 1, 20000, 0];
        };

    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties (Constant)

        %   Model       ProductCode          Revision          HasOutput
        models = {
          'EP3174-0002', hex2dec('0c664052'), hex2dec('00160002'), 0, 10;
          'EP3182-1002', hex2dec('0c6e4052'), hex2dec('001203ea'), 5, 10;
          'EP3184-0002', hex2dec('0c704052'), hex2dec('00110002'), 0,  6;
          'EP3184-1002', hex2dec('0c704052'), hex2dec('001203ea'), 0,  6;
        };
    end
end
