%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Encapsulation for Murr IMPACT67 digital input and output slaves
% ESI: Murrelektronik_IMPACT67.xml
%
% Copyright (C) 2014 Richard Hacker
% License: LGPL
%
classdef murr_dio < EtherCATSlave

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods
        %====================================================================
        function obj = murr_dio(id)
            if nargin > 0
                obj.slave = obj.find(id);
            end
        end

        %====================================================================
        function rv = configure(obj,vector)

            rv.SlaveConfig.vendor = 79;
            rv.SlaveConfig.description = obj.slave{1};
            rv.SlaveConfig.product  = obj.slave{2};

            pdo_list = murr_dio.pdo;

            % Find out how many entries there are in PDO #x1a01
            if isempty(obj.slave{8})
                status_end = 8;
            else
                status_end = obj.slave{8}(end) + 1;
            end

            % Create an array of TxPdo,length
            if isempty(obj.slave{5})
                pdo_idx = [3,status_end];
                pdo = {{'Stat', zeros(numel(obj.slave{7}),4)}};
                pdo{1}{2}(:,3) = obj.slave{7};

                if ~isempty(obj.slave{8})
                    pdo{end+1} = {'Short', zeros(numel(obj.slave{8}),4)};
                    pdo{end}{2}(:,3) = obj.slave{8};
                end
            else
                pdo_idx = [2,obj.slave{5}(end)+1;
                           3,status_end];

                % Set BitLen of PDO Entry #x5999
                pdo_list{2,2}(1,end) = obj.slave{6};

                pdo = {{'DI', zeros(numel(obj.slave{5}),4)}, ...
                        {'Stat', zeros(numel(obj.slave{7}),4)}};
                pdo{1}{2}(:,3) = obj.slave{5};
                pdo{2}{2}(:,2) = 1;
                pdo{2}{2}(:,3) = obj.slave{7};

                if ~isempty(obj.slave{8})
                    pdo{end+1} = {'Short', zeros(numel(obj.slave{8}),4)};
                    pdo{end}{2}(:,2) = 1;
                    pdo{end}{2}(:,3) = obj.slave{8};
                end
            end

            rv.SlaveConfig.sm = ...
                {{3,1,arrayfun(@(i) {pdo_list{pdo_idx(i,1),1}, ...
                                     pdo_list{pdo_idx(i,1),2}(1:pdo_idx(i,2),:)}, ...
                               1:size(pdo_idx,1), 'UniformOutput', false)}};

             output = cellfun(@(x) EtherCATSlave.configurePorts(x{1}, x{2}, ...
                                                        uint(1), vector), ...
                              pdo, 'UniformOutput', false);
             rv.PortConfig.output = [output{:}];

             if ~isempty(obj.slave{4})
                 rv.SlaveConfig.sm{end+1} = ...
                        {2,0,{{pdo_list{1,1}, ...
                               pdo_list{1,2}(1:obj.slave{4}(end)+1,:)}}};
                 pdo = repmat([1, 0, 0, 0], numel(obj.slave{4}), 1);
                 pdo(:,3) = obj.slave{4};
                 rv.PortConfig.input = ...
                     EtherCATSlave.configurePorts('DO', pdo, uint(1), vector);
             end
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods (Static)
        %====================================================================
        function test(p)
            ei = EtherCATInfo(fullfile(p,'Murrelektronik_IMPACT67.xml'));
            for i = 1:size(murr_dio.models,1)
                fprintf('Testing %s\n', murr_dio.models{i,1});
                slave = ei.getSlave(murr_dio.models{i,2},...
                        'revision', murr_dio.models{i,3});
                model = murr_dio.models{i,1};

                rv = murr_dio(model).configure(i&1);
                slave.testConfig(rv.SlaveConfig,rv.PortConfig);
            end
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties (Constant, Access = private)

        pdo = {hex2dec('1600'), [hex2dec('6199'), 1,  8;
                                 hex2dec('6200'), 1,  1; 
                                 hex2dec('6200'), 2,  1; 
                                 hex2dec('6200'), 3,  1; 
                                 hex2dec('6200'), 4,  1; 
                                 hex2dec('6200'), 5,  1; 
                                 hex2dec('6200'), 6,  1; 
                                 hex2dec('6200'), 7,  1; 
                                 hex2dec('6200'), 8,  1;
                                 hex2dec('6200'), 9,  1; 
                                 hex2dec('6200'), 10, 1; 
                                 hex2dec('6200'), 11, 1; 
                                 hex2dec('6200'), 12, 1; 
                                 hex2dec('6200'), 13, 1; 
                                 hex2dec('6200'), 14, 1; 
                                 hex2dec('6200'), 15, 1; 
                                 hex2dec('6200'), 16, 1];

               hex2dec('1a00'), [hex2dec('5999'), 1,  1;
                                 hex2dec('6000'), 1,  1;
                                 hex2dec('6000'), 2,  1;
                                 hex2dec('6000'), 3,  1;
                                 hex2dec('6000'), 4,  1;
                                 hex2dec('6000'), 5,  1;
                                 hex2dec('6000'), 6,  1;
                                 hex2dec('6000'), 7,  1;
                                 hex2dec('6000'), 8,  1;
                                 hex2dec('6000'), 9,  1;
                                 hex2dec('6000'), 10, 1;
                                 hex2dec('6000'), 11, 1;
                                 hex2dec('6000'), 12, 1;
                                 hex2dec('6000'), 13, 1;
                                 hex2dec('6000'), 14, 1;
                                 hex2dec('6000'), 15, 1;
                                 hex2dec('6000'), 16, 1];

               hex2dec('1a01'), [hex2dec('1001'), 1,  1;
                                 hex2dec('1001'), 2,  1;
                                 hex2dec('1001'), 3,  1;
                                 hex2dec('1001'), 4,  1;
                                 hex2dec('1001'), 5,  1;
                                 hex2dec('1001'), 6,  1;
                                 hex2dec('1001'), 7,  1;
                                 hex2dec('1001'), 8,  1;
                                 hex2dec('1002'), 1,  1;
                                 hex2dec('1002'), 2,  1;
                                 hex2dec('1002'), 3,  1;
                                 hex2dec('1002'), 4,  1;
                                 hex2dec('1002'), 5,  1;
                                 hex2dec('1002'), 6,  1;
                                 hex2dec('1002'), 7,  1;
                                 hex2dec('1002'), 8,  1;
                                 hex2dec('1002'), 9,  1;
                                 hex2dec('1002'), 10, 1;
                                 hex2dec('1002'), 11, 1;
                                 hex2dec('1002'), 12, 1;
                                 hex2dec('1002'), 13, 1;
                                 hex2dec('1002'), 14, 1;
                                 hex2dec('1002'), 15, 1;
                                 hex2dec('1002'), 16, 1];
        };

    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties (Constant)
        %   Model,ProductCode,Revision,
        %   RxPdo(#x1600),TxPdo(#x1a00), BitLen(Pdo #x5999), Status(#x1a01)
        models = {
                'DI8DO8', hex2dec('0000d72a'), 1, 1:8,  1:8, 8,     0:3, 8:15;
                'DO16',   hex2dec('0000d72c'), 1, 1:16,  [], 0, [0,1,3], 8:23;
                'DO8',    hex2dec('0000d72b'), 1, 1:8,   [], 0, [0,1,3], 8:15;
                'DI16',   hex2dec('0000d729'), 1, [],  1:16, 1, [0,2,3],   [];
            };
    end
end

