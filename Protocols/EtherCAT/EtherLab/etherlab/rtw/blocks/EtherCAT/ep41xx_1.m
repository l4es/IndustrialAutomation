classdef ep41xx_1 < EtherCATSlave

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods
        %====================================================================
        function obj = ep41xx_1(id)
            if nargin > 0
                obj.slave = obj.find(id);
            end
        end

        %====================================================================
        function rv = configure(obj,vector,scale,dc,type)

            rv.SlaveConfig.vendor = 2;
            rv.SlaveConfig.description = obj.slave{1};
            rv.SlaveConfig.product = obj.slave{2};

            % PdoCount is needed to configure SDO
            pdo_count = str2double(obj.slave{1}(6));

            rv.SlaveConfig.sm = obj.sm;

            % Value output
            pdo = zeros(pdo_count,4);
            pdo(:,2) = 0:pdo_count-1;
            rv.PortConfig.input = obj.configurePorts('Ch.',...
                           pdo,sint(16),vector,scale);
                      
            % Distributed clock
            if dc(1) > 2
                rv.SlaveConfig.dc = dc(2:11);
            else
                rv.SlaveConfig.dc = obj.dc{dc(1),2};
            end

            basetype = [0,1,2,6];
            types = basetype(type);
            types = types(1:pdo_count)';

            rv.SlaveConfig.sdo = num2cell(horzcat(...
                hex2dec('F800')*ones(pdo_count,1),...
                (1:pdo_count)',...
                16*ones(pdo_count,1),...
                types));

        end
    end
 

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods (Static)
        %====================================================================
        function test(p)
            ei = EtherCATInfo(fullfile(p,'Beckhoff EP4xxx.xml'));
            for i = 1:size(ep41xx_1.models,1)
                fprintf('Testing %s\n', ep41xx_1.models{i,1});
                slave = ei.getSlave(ep41xx_1.models{i,2},...
                        'revision', ep41xx_1.models{i,3});

                rv = ep41xx_1(ep41xx_1.models{i,1}).configure(i&1,...
                        EtherCATSlave.configureScale(2^15,''),1,1:4);
                slave.testConfig(rv.SlaveConfig,rv.PortConfig);

                rv = ep41xx_1(ep41xx_1.models{i,1}).configure(~(i&1),...
                        EtherCATSlave.configureScale(2^15,'6'),1,1:4);
                slave.testConfig(rv.SlaveConfig,rv.PortConfig);
            end
        end
   end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties (Constant, Access = private)

        sm = {...
                {2, 0, { {hex2dec('1600') [hex2dec('7000'), 17, 16]},...
                         {hex2dec('1601') [hex2dec('7010'), 17, 16]},...
                         {hex2dec('1602') [hex2dec('7020'), 17, 16]},...
                         {hex2dec('1603') [hex2dec('7030'), 17, 16]}}}...
        };

        dc = {'Free Run', [         0,   0, 0,     0, 0, 0, 0, 1,      0, 0];
              'DC-Synchron', ...
                        [hex2dec('730'), 0, 1,     0, 0, 0, 0, 1, 140000, 0];
        };

    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties (Constant)
        %   Model       ProductCode          Revision          HasOutput
        models = {
          'EP4174-0002', hex2dec('104e4052'), hex2dec('00100002'); ...
        };
    end
end
