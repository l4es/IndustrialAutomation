classdef el51xx < EtherCATSlave

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods
        %====================================================================
        function obj = el51xx(id)
            if nargin > 0
                obj.slave = obj.find(id);
            end
        end

        %====================================================================
        function rv = configure(obj,mapped_pdo,sdo_config,dc_config)

            rv.SlaveConfig.vendor = 2;
            rv.SlaveConfig.description = obj.slave{1};
            rv.SlaveConfig.product  = obj.slave{2};

            % Get a list of pdo's for the selected slave
            selected = ismember(1:size(obj.pdo,1), [obj.slave{4},obj.slave{5}]) ...
                & ismember([obj.pdo{:,1}], mapped_pdo);

            % Reduce this list to the ones selected, making sure that
            % excluded pdo's are not mapped
            exclude = [];
            for i = 1:numel(selected)
                selected(i) = selected(i) & ~ismember(el51xx.pdo{i,1}, exclude);
                if selected(i)
                    exclude = [exclude,el51xx.pdo{i,2}];
                end
            end

            % Configure SM2 and SM3
            selected_idx = find(selected);
            rx = obj.slave{4}(ismember(obj.slave{4}, selected_idx));
            tx = obj.slave{5}(ismember(obj.slave{5}, selected_idx));
            rv.SlaveConfig.sm = ...
                {{2,0, arrayfun(@(x) {el51xx.pdo{x,1}, el51xx.pdo{x,3}},...
                                rx, 'UniformOutput', false)}, ...
                 {3,1, arrayfun(@(x) {el51xx.pdo{x,1}, el51xx.pdo{x,3}},...
                                tx, 'UniformOutput', false)}};

            % Configure input port. The algorithm below will group all boolean
            % signals to one port. All other entries get a separate port
            inputs = arrayfun(@(i) arrayfun(@(j) {i-1, ...
                                                  el51xx.pdo{rx(i),3}(el51xx.pdo{rx(i),4}{j,1}(1)+1,3), ...
                                                  el51xx.pdo{rx(i),4}{j,1}',...
                                                  el51xx.pdo{rx(i),4}{j,2}},...
                                            1:size(el51xx.pdo{rx(i),4},1),...
                                            'UniformOutput', false), ...
                              1:numel(rx), 'UniformOutput', false);
            if ~isempty(inputs)
                inputs = horzcat(inputs{:});
            end

            rv.PortConfig.input = ...
                cellfun(@(i) struct('pdo',horzcat(repmat([0,i{1}], ...
                                                         size(i{3})), ...
                                                  i{3}, zeros(size(i{3}))), ...
                                    'pdo_data_type', uint(i{2}), ...
                                    'portname', i{4}), ...
                         inputs);
            if isempty(rv.PortConfig.input)
                rv.PortConfig.input = struct('portname',{});
            end

            % Configure output port. The algorithm below will group all boolean
            % signals to one port. All other entries get a separate port
            outputs = arrayfun(@(i) arrayfun(@(j) {i-1, ...
                                                   el51xx.pdo{tx(i),3}(el51xx.pdo{tx(i),4}{j,1}(1)+1,3), ...
                                                   el51xx.pdo{tx(i),4}{j,1}',...
                                                   el51xx.pdo{tx(i),4}{j,2}},...
                                            1:size(el51xx.pdo{tx(i),4},1),...
                                            'UniformOutput', false), ...
                              1:numel(tx), 'UniformOutput', false);
            if ~isempty(outputs)
                outputs = horzcat(outputs{:});
            end

            rv.PortConfig.output = ...
                cellfun(@(i) struct('pdo',horzcat(repmat([1,i{1}], ...
                                                         size(i{3})), ...
                                                  i{3}, zeros(size(i{3}))), ...
                                    'pdo_data_type', uint(i{2}), ...
                                    'portname', i{4}), ...
                         outputs);
            if isempty(rv.PortConfig.output)
                rv.PortConfig.output = struct('portname',{});
            end

            % Distributed clocks
            if dc_config(1) == 4
                rv.PortConfig.dc = dc_config(2:11);
            elseif dc_config(1) > 1
                dc = el51xx.dc;
                rv.PortConfig.dc = dc(dc_config(1),:);
                rv.PortConfig.dc(1) = obj.slave{7}; % Set AssignActivate
            end

            % CoE Configuration
            sdo_row = unique([obj.slave{6}, el51xx.pdo{[tx,rx],5}]);
            rv.SlaveConfig.sdo = num2cell(...
                horzcat(obj.sdo(sdo_row,:), sdo_config(sdo_row)'));
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods (Static)
        %====================================================================
        function modelChanged
            obj = el51xx(get_param(gcbh,'model'));

            obj.updateRevision();

            EtherCATSlave.updatePDOVisibility([obj.pdo{[obj.slave{[4,5]}],1}]);
            el51xx.updatePDO()
        end

        %====================================================================
        function updatePDO
            obj = el51xx(get_param(gcbh,'model'));
            names = char(get_param(gcbh,'MaskNames'));
            values = get_param(gcbh,'MaskValues');

            % row list of all pdo's for the slave
            % Columns:
            %   1: PDO
            %   2: Excluded PDO's
            %   3: Enabled additional SDO's
            pdo_list = el51xx.pdo([obj.slave{[4,5]}], [1,2,5]);

            pdo_rows = ismember(names, ...
                          strcat('pdo_x', dec2hex([pdo_list{:,1}],4)),...
                          'rows');
            pdo_num = zeros(size(pdo_rows));
            pdo_num(pdo_rows) = hex2dec(names(pdo_rows,6:end));
            on = strcmp(values,'on') & pdo_rows;

            selected_rows = ismember([pdo_list{:,1}], pdo_num(on));
            exclude = unique([pdo_list{selected_rows, 2}]);

            disable = pdo_rows & ismember(pdo_num, exclude);

            for i = 1:length(pdo_list)
                if disable(i) && strcmp(values(i), 'on')
                    % Upps, there is a confict here. Uncheck the option
                    % in question and retry the update
                    set_param(gcbh,deblank(names(i,:)),'off')
                    el51xx.updatePDO()
                    return
                end
            end

            EtherCATSlave.setEnable(pdo_rows, ~disable);

            % Append additional SDO's to the list defined in slave{6}
            sdo = unique([obj.slave{6}, pdo_list{selected_rows, 3}]);

            EtherCATSlave.updateSDOVisibility(dec2base(sdo,10,2));
        end

        %====================================================================
        function test(p)
            ei = EtherCATInfo(fullfile(p,'Beckhoff EL5xxx.xml'));
            for i = 1:size(el51xx.models,1)
                fprintf('Testing %s\n', el51xx.models{i,1});
                slave = ei.getSlave(el51xx.models{i,2},...
                        'revision', el51xx.models{i,3});
                model = el51xx.models{i,1};

                l = [el51xx.models{1,4},el51xx.models{1,5}];

                pdoIdx = cell2mat(el51xx.pdo(l,1))';
                rv = el51xx(model).configure(pdoIdx, 1:50,2);
                slave.testConfig(rv.SlaveConfig,rv.PortConfig);

                pdoIdx = cellfun(@(x) x(1), el51xx.pdo(l,2))';
                rv = el51xx(model).configure(pdoIdx, 1:50,2);
                slave.testConfig(rv.SlaveConfig,rv.PortConfig);
            end
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties (Constant, Access = private)

        pdo = {
                % EL5101,EL5101-1006 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                % Ignore 1600/1A00 because they are not word aligned
                hex2dec('1601'), hex2dec({'1602','1603'})', ...
                   [ hex2dec('7000'), 1, 8;
                     0              , 0, 8;
                     hex2dec('7000'), 2,16],...
                   { 0, 'Ctrl'; 2, 'Value' }, ...
                   [];
                hex2dec('1602'), hex2dec({'1601','1603'})', ...
                   [ hex2dec('7010'), 1, 1;
                     hex2dec('7010'), 2, 1;
                     hex2dec('7010'), 3, 1;
                     hex2dec('7010'), 4, 1;
                     0              , 0, 4;
                     0              , 0, 8;
                     hex2dec('7010'),17,16],...
                   { 0:3, 'bool[4]'; 6, 'Value' },...
                   [];
                hex2dec('1603'), hex2dec({'1601','1602'})', ...
                   [ hex2dec('7010'), 1, 1;
                     hex2dec('7010'), 2, 1;
                     hex2dec('7010'), 3, 1;
                     hex2dec('7010'), 4, 1;
                     0              , 0, 4;
                     0              , 0, 8;
                     hex2dec('7010'),17,32],...
                   { 0:3, 'bool[4]'; 6, 'Value' },...
                   [];
                hex2dec('1a01'), hex2dec({              '1a03','1a04',...
                                          '1a05','1a06','1a07','1a08'})',...
                   [ hex2dec('6000'), 1, 8;
                     0              , 0, 8;
                     hex2dec('6000'), 2,16;
                     hex2dec('6000'), 3,16],...
                   { 0, 'Status'; 2, 'Counter'; 3, 'Latch' }, ...
                   [1:5,25];
                hex2dec('1a02'), hex2dec({              '1a03','1a04',...
                                          '1a05','1a06','1a07','1a08'})',...
                   [ hex2dec('6000'), 4,32;
                     hex2dec('6000'), 5,16;
                     hex2dec('6000'), 6,16],...
                   { 0, 'Freq'; 1, 'Period'; 2, 'Window' },...
                   [1:5,24,25];

                hex2dec('1a03'), hex2dec({'1a01','1a02','1a04'})',...
                   [ hex2dec('6010'), 1, 1;
                     hex2dec('6010'), 2, 1;
                     hex2dec('6010'), 3, 1;
                     hex2dec('6010'), 4, 1;
                     hex2dec('6010'), 5, 1;
                     hex2dec('6010'), 6, 1;
                     hex2dec('6010'), 7, 1;
                     hex2dec('6010'), 8, 1;
                     hex2dec('6010'), 9, 1;
                     hex2dec('6010'),10, 1;
                     hex2dec('6010'),11, 1;
                     hex2dec('6010'),12, 1;
                     hex2dec('6010'),13, 1;
                     hex2dec('1c32'),32, 1;
                     hex2dec('1803'), 7, 1;
                     hex2dec('1803'), 9, 1;
                     hex2dec('6010'),17,16;
                     hex2dec('6010'),18,16],...
                   { 0:12, 'bool[13]'; 16, 'Counter'; 17, 'Latch' },...
                   [26:35,37:43];
                hex2dec('1a04'), hex2dec({'1a01','1a02','1a03'})',...
                   [ hex2dec('6010'), 1, 1;
                     hex2dec('6010'), 2, 1;
                     hex2dec('6010'), 3, 1;
                     hex2dec('6010'), 4, 1;
                     hex2dec('6010'), 5, 1;
                     hex2dec('6010'), 6, 1;
                     hex2dec('6010'), 7, 1;
                     hex2dec('6010'), 8, 1;
                     hex2dec('6010'), 9, 1;
                     hex2dec('6010'),10, 1;
                     hex2dec('6010'),11, 1;
                     hex2dec('6010'),12, 1;
                     hex2dec('6010'),13, 1;
                     hex2dec('1c32'),32, 1;
                     hex2dec('1804'), 7, 1;
                     hex2dec('1804'), 9, 1;
                     hex2dec('6010'),17,32;
                     hex2dec('6010'),18,32],...
                   { 0:12, 'bool[13]'; 16, 'Counter'; 17, 'Latch' },...
                   [26:35,37:43];
                hex2dec('1a05'), hex2dec({'1a01','1a02','1a06'})',...
                   [ hex2dec('6010'),19,32],...
                   { 0, 'Freq' }, ...
                   [26:35,37:43];
                hex2dec('1a06'), hex2dec({'1a01','1a02','1a05'})',...
                   [ hex2dec('6010'),20,32],...
                   { 0, 'Period' },...
                   [26:35,37:43];
                hex2dec('1a07'), hex2dec({'1a01','1a02','1a08'})',...
                   [ hex2dec('6010'),22,64],...
                   { 0, 'Time' },...
                   [26:35,37:43];
                hex2dec('1a08'), hex2dec({'1a01','1a02','1a07'})',...
                   [ hex2dec('6010'),22,32],...
                   { 0, 'Time' },...
                   [26:35,37:43];

                % EL5101-0010 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                hex2dec('1600'), hex2dec('1601'), ...
                   [ hex2dec('7000'), 1, 1;
                     hex2dec('7000'), 2, 1;
                     hex2dec('7000'), 3, 1;
                     hex2dec('7000'), 4, 1;
                     0              , 0, 4;
                     0              , 0, 8;
                     hex2dec('7000'),17,32], ...
                   {0:3, 'bool[4]'; 6, 'Value'},...
                   [];
                hex2dec('1601'), hex2dec('1600'), ...
                   [ hex2dec('7000'), 1, 1;
                     hex2dec('7000'), 2, 1;
                     hex2dec('7000'), 3, 1;
                     hex2dec('7000'), 4, 1;
                     0              , 0, 4;
                     0              , 0, 8;
                     hex2dec('7000'),17,16], ...
                   {0:3, 'bool[4]'; 6, 'Value'},...
                   [];
                hex2dec('1a00'), hex2dec('1a01'),...
                   [ hex2dec('6000'), 1, 1;
                     hex2dec('6000'), 2, 1;
                     hex2dec('6000'), 3, 1;
                     0              , 0, 2;
                     hex2dec('6000'), 6, 1;
                     hex2dec('6000'), 7, 1;
                     0              , 0, 1;
                     hex2dec('6000'), 9, 1;
                     hex2dec('6000'),10, 1;
                     hex2dec('6000'),11, 1;
                     hex2dec('6000'),12, 1;
                     hex2dec('6000'),13, 1;
                     hex2dec('1c32'),32, 1;
                     hex2dec('1801'), 7, 1;
                     hex2dec('1801'), 9, 1;
                     hex2dec('6000'),17,32;
                     hex2dec('6000'),18,32], ...
                   {[0:2,4,5,7:11], 'bool[10]'; 15, 'Counter'; 16, 'Latch'},...
                   [];
                hex2dec('1a01'), hex2dec('1a00'),...
                   [ hex2dec('6000'), 1, 1;
                     hex2dec('6000'), 2, 1;
                     hex2dec('6000'), 3, 1;
                     0              , 0, 2;
                     hex2dec('6000'), 6, 1;
                     hex2dec('6000'), 7, 1;
                     0              , 0, 1;
                     hex2dec('6000'), 9, 1;
                     hex2dec('6000'),10, 1;
                     hex2dec('6000'),11, 1;
                     hex2dec('6000'),12, 1;
                     hex2dec('6000'),13, 1;
                     hex2dec('1c32'),32, 1;
                     hex2dec('1801'), 7, 1;
                     hex2dec('1801'), 9, 1;
                     hex2dec('6000'),17,16;
                     hex2dec('6000'),18,16], ...
                   {[0:2,4,5,7:11], 'bool[10]'; 15, 'Counter'; 16, 'Latch'},...
                   [];
                hex2dec('1a02'), hex2dec('1a03'),...
                   [ hex2dec('6000'),20,32], ...
                   {0, 'Period'},...
                   [];
                hex2dec('1a03'), hex2dec('1a02'),...
                   [ hex2dec('6000'),19,32], ...
                   {0, 'Freq'},...
                   [];
                hex2dec('1a04'), hex2dec('1a05'),...
                   [ hex2dec('6000'),22,64], ...
                   {0, 'Time'},...
                   [];
                hex2dec('1a05'), hex2dec('1a04'),...
                   [ hex2dec('6000'),22,32], ...
                   {0, 'Time'},...
                   [];

                % EL5151 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                % Same as EL5101-0010, except for
                hex2dec('1a00'), hex2dec('1a01'),...
                   [ hex2dec('6000'), 1, 1;
                     hex2dec('6000'), 2, 1;
                     hex2dec('6000'), 3, 1;
                     0              , 0, 4;
                     hex2dec('6000'), 8, 1;
                     hex2dec('6000'), 9, 1;
                     hex2dec('6000'),10, 1;
                     hex2dec('6000'),11, 1;
                     0              , 0, 1;
                     hex2dec('6000'),13, 1;
                     hex2dec('6000'),14, 1;
                     0              , 0, 1;
                     hex2dec('6000'),16, 1;
                     hex2dec('6000'),17,32;
                     hex2dec('6000'),18,32], ...
                   {[0:2,4:7,9], 'bool[8]'; 13, 'Counter'; 14, 'Latch'},...
                   [];
                hex2dec('1a01'), hex2dec('1a00'),...
                   [ hex2dec('6000'), 1, 1;
                     hex2dec('6000'), 2, 1;
                     hex2dec('6000'), 3, 1;
                     0              , 0, 4;
                     hex2dec('6000'), 8, 1;
                     hex2dec('6000'), 9, 1;
                     hex2dec('6000'),10, 1;
                     hex2dec('6000'),11, 1;
                     0              , 0, 1;
                     hex2dec('6000'),13, 1;
                     hex2dec('6000'),14, 1;
                     0              , 0, 1;
                     hex2dec('6000'),16, 1;
                     hex2dec('6000'),17,16;
                     hex2dec('6000'),18,16], ...
                   {[0:2,4:7,9], 'bool[8]'; 13, 'Counter'; 14, 'Latch'},...
                   [];

                % EL5152 RxPdo for Rev 00100000 %%%%%%%%%%%%%%%%%%%%%%%%%%%
                hex2dec('1600'), hex2dec('1601'),...
                   [ 0              , 0, 1;
                     0              , 0, 1;
                     hex2dec('7000'), 3, 1;
                     0              , 0, 1;
                     0              , 0, 4;
                     0              , 0, 8;
                     hex2dec('7000'),17,32], ...
                   {2, 'Set'; 6, 'Value'},...
                   [];
                hex2dec('1601'), hex2dec('1600'),...
                   [ 0              , 0, 1;
                     0              , 0, 1;
                     hex2dec('7000'), 3, 1;
                     0              , 0, 1;
                     0              , 0, 4;
                     0              , 0, 8;
                     hex2dec('7000'),17,16], ...
                   {2, 'Set'; 6, 'Value'},...
                   [];
                hex2dec('1602'), hex2dec('1603'),...
                   [ 0              , 0, 1;
                     0              , 0, 1;
                     hex2dec('7010'), 3, 1;
                     0              , 0, 1;
                     0              , 0, 4;
                     0              , 0, 8;
                     hex2dec('7010'),17,32], ...
                   {2, 'Set'; 6, 'Value'},...
                   [];
                hex2dec('1603'), hex2dec('1602'),...
                   [ 0              , 0, 1;
                     0              , 0, 1;
                     hex2dec('7010'), 3, 1;
                     0              , 0, 1;
                     0              , 0, 4;
                     0              , 0, 8;
                     hex2dec('7010'),17,16], ...
                   {2, 'Set'; 6, 'Value'},...
                   [];

                % EL5152 TxPdo %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                hex2dec('1a00'), hex2dec('1a01'),...
                   [ 0              , 0, 2;
                     hex2dec('6000'), 3, 1;
                     0              , 0, 4;
                     hex2dec('6000'), 8, 1;
                     hex2dec('6000'), 9, 1;
                     hex2dec('6000'),10, 1;
                     0              , 0, 3;
                     hex2dec('1c32'),32, 1;
                     0              , 0, 1;
                     hex2dec('1800'), 9, 1;
                     hex2dec('6000'),17,32], ...
                   {[1,3,4,5], 'bool[4]'; 10, 'Counter'},...
                   [];
                hex2dec('1a01'), hex2dec('1a00'),...
                   [ 0              , 0, 2;
                     hex2dec('6000'), 3, 1;
                     0              , 0, 4;
                     hex2dec('6000'), 8, 1;
                     hex2dec('6000'), 9, 1;
                     hex2dec('6000'),10, 1;
                     0              , 0, 3;
                     hex2dec('1c32'),32, 1;
                     0              , 0, 1;
                     hex2dec('1801'), 9, 1;
                     hex2dec('6000'),17,16], ...
                   {[1,3,4,5], 'bool[4]'; 10, 'Counter'},...
                   [];
                hex2dec('1a02'), hex2dec('1a03'),...
                   [ hex2dec('6000'),20,32], ...
                   {0, 'Period'},...
                   [];
                hex2dec('1a03'), hex2dec('1a02'),...
                   [ hex2dec('6000'),19,32], ...
                   {0, 'Freq'},...
                   [];
                hex2dec('1a04'), hex2dec('1a05'),...
                   [ 0              , 0, 2;
                     hex2dec('6010'), 3, 1;
                     0              , 0, 4;
                     hex2dec('6010'), 8, 1;
                     hex2dec('6010'), 9, 1;
                     hex2dec('6010'),10, 1;
                     0              , 0, 3;
                     hex2dec('1c32'),32, 1;
                     0              , 0, 1;
                     hex2dec('1804'), 9, 1;
                     hex2dec('6010'),17,32], ...
                   {[1,3,4,5], 'bool2[4]'; 10, 'Counter2'},...
                   [];
                hex2dec('1a05'), hex2dec('1a04'),...
                   [ 0              , 0, 2;
                     hex2dec('6010'), 3, 1;
                     0              , 0, 4;
                     hex2dec('6010'), 8, 1;
                     hex2dec('6010'), 9, 1;
                     hex2dec('6010'),10, 1;
                     0              , 0, 3;
                     hex2dec('1c32'),32, 1;
                     0              , 0, 1;
                     hex2dec('1805'), 9, 1;
                     hex2dec('6010'),17,16], ...
                   {[1,3,4,5], 'bool2[4]'; 10, 'Counter2'},...
                   [];
                hex2dec('1a06'), hex2dec('1a07'),...
                   [ hex2dec('6010'),20,32], ...
                   {0, 'Period'},...
                   [];
                hex2dec('1a07'), hex2dec('1a06'),...
                   [ hex2dec('6010'),19,32], ...
                   {0, 'Freq'},...
                   [];

                % EL5152 RxPdo for Rev 00120000 %%%%%%%%%%%%%%%%%%%%%%%%%%%
                hex2dec('1600'), hex2dec('1601'),...
                   [ 0              , 0, 2;
                     hex2dec('7000'), 3, 1;
                     0              , 0, 5;
                     0              , 0, 8;
                     hex2dec('7000'),17,32], ...
                   {1, 'Set'; 4, 'Value'},...
                   [];
                hex2dec('1601'), hex2dec('1600'),...
                   [ 0              , 0, 2;
                     hex2dec('7000'), 3, 1;
                     0              , 0, 5;
                     0              , 0, 8;
                     hex2dec('7000'),17,16], ...
                   {1, 'Set'; 4, 'Value'},...
                   [];
                hex2dec('1602'), hex2dec('1603'),...
                   [ 0              , 0, 2;
                     hex2dec('7010'), 3, 1;
                     0              , 0, 5;
                     0              , 0, 8;
                     hex2dec('7010'),17,32], ...
                   {1, 'Set'; 4, 'Value'},...
                   [];
                hex2dec('1603'), hex2dec('1602'),...
                   [ 0              , 0, 2;
                     hex2dec('7010'), 3, 1;
                     0              , 0, 5;
                     0              , 0, 8;
                     hex2dec('7010'),17,16], ...
                   {1, 'Set'; 4, 'Value'},...
                   [];
        };

        dc = [0,0,0,0,0,0,0,0,0,0;      % FreeRun
              0,0,1,0,0,0,0,1,0,0;      % DC-Synchron
              0,0,1,0,0,1,0,1,0,0];     % DC-Synchron (input based)

        % CoE Definition
        sdo = [ 
                %                               +EL5101 Normal
                %                               |+EL5101 Enhanced
                %                               ||+EL5101-0010
                %                               |||+EL5151
                %                               ||||+EL5152
                %                               |||||
                hex2dec('8000'),  1,  8; % 1    10000  ' 1  1 Enable register reload'
                hex2dec('8000'),  2,  8; % 2    10000  ' 2  1 Enable index reset'
                hex2dec('8000'),  3,  8; % 3    10000  ' 3  1 Enable FWD count'
                hex2dec('8000'),  4,  8; % 4    10000  ' 4  1 Enable pos. gate'
                hex2dec('8000'),  5,  8; % 5    10000  ' 5  1 Enable neg. gate'
                hex2dec('8000'),  1,  8; % 6    00110  ' 1  1 Enable C reset'
                hex2dec('8000'),  2,  8; % 7    00110  ' 2  1 Enable extern reset'
                hex2dec('8000'),  3,  8; % 8    00011  ' 3  1 Enable up/down counter'
                hex2dec('8000'),  4,  8; % 9    00110  ' 4  2 Gate polarity'
                hex2dec('8000'),  8,  8; %10    00011  ' 8  1 Disable filter'
                hex2dec('8000'), 10,  8; %11    00011  ' A  1 Enable micro increments'
                hex2dec('8000'), 11,  8; %12    00100  ' B  1 Open circuit detection A'
                hex2dec('8000'), 12,  8; %13    00100  ' C  1 Open circuit detection B'
                hex2dec('8000'), 13,  8; %14    00100  ' D  1 Open circuit detection C'
                hex2dec('8000'), 14,  8; %15    00111  ' E  2 Reversion of rotation'
                hex2dec('8000'), 15,  8; %16    00011  ' F  1 Frequency window base'
                hex2dec('8000'), 16,  8; %17    00110  '10  1 Extern reset polarity'
                hex2dec('8000'), 17, 16; %18    00111  '11 16 Frequency window'
                hex2dec('8000'), 19, 16; %19    00111  '13 16 Frequency scaling'
                hex2dec('8000'), 20, 16; %20    00111  '14 16 Period scaling'
                hex2dec('8000'), 21, 16; %21    00111  '15 16 Frequency resolution'
                hex2dec('8000'), 22, 16; %22    00111  '16 16 Period resolution'
                hex2dec('8000'), 23, 16; %23    00111  '17 16 Frequency wait time'
                hex2dec('8001'),  1, 16; %24    10000  ' 1 16 Frequency window'
                hex2dec('8001'),  2, 16; %25    10000  ' 2 16 Counter reload value'
                hex2dec('8010'),  1,  8; %26    01000  ' 1  1 Enable C reset'
                hex2dec('8010'),  2,  8; %27    01000  ' 2  1 Enable extern reset'
                hex2dec('8010'),  3,  8; %28    01001  ' 3  1 Enable up/down counter'
                hex2dec('8010'),  4,  8; %29    01000  ' 4  2 Gate polarity'
                hex2dec('8010'),  8,  8; %30    01001  ' 8  1 Disable filter'
                hex2dec('8010'), 10,  8; %31    01001  ' A  1 Enable micro increments'
                hex2dec('8010'), 11,  8; %32    01000  ' B  1 Open circuit detection A'
                hex2dec('8010'), 12,  8; %33    01000  ' C  1 Open circuit detection B'
                hex2dec('8010'), 13,  8; %34    01000  ' D  1 Open circuit detection C'
                hex2dec('8010'), 14,  8; %35    01001  ' E  1 Reversion of rotation'
                hex2dec('8010'), 15,  8; %36    00001  ' F  1 Frequency window base'
                hex2dec('8010'), 16,  8; %37    01000  '10  1 Extern reset polarity'
                hex2dec('8010'), 17, 16; %38    01001  '11 16 Frequency window'
                hex2dec('8010'), 19, 16; %39    01001  '13 16 Frequency scaling'
                hex2dec('8010'), 20, 16; %40    01001  '14 16 Period scaling'
                hex2dec('8010'), 21, 16; %41    01001  '15 16 Frequency resolution'
                hex2dec('8010'), 22, 16; %42    01001  '16 16 Period resolution'
                hex2dec('8010'), 23, 16; %43    01001  '17 16 Frequency wait time'
        ];

    end

    properties (Constant)
        %      Model,                   ProductCode, RevNo
        %         Rx,              Tx,                   CoE, AssignActivate
        models = {...
            'EL5101',           hex2dec('13ed3052'), hex2dec('00010000'), ...
                 1:3,            4:11,                    [], hex2dec('320');
            'EL5101-0010',      hex2dec('13ed3052'), hex2dec('0010000a'), ...
                 12:13,         14:19,   [6,7,9,12:15,17:23], hex2dec('320');
            'EL5151',           hex2dec('141f3052'), hex2dec('00190000'), ...
                 12:13, [20,21,16:19],          [6:11,15:23], hex2dec('320');
            'EL5152',           hex2dec('14203052'), hex2dec('00120000'), ...
                 34:37,         26:33, [8,10,11,15,16,18:23,...
                                       28,30,31,35,36,38:43], hex2dec('720');
            'EL5152-0000-0016', hex2dec('14203052'), hex2dec('00100000'), ...
                 22:25,         26:33, [8,10,11,15,16,18:23,...
                                       28,30,31,35,36,38:43], hex2dec('720');
        };
    end
end
