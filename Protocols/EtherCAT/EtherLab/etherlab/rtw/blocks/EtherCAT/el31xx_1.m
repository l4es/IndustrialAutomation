classdef el31xx_1 < EtherCATSlave

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods
        %====================================================================
        function obj = el31xx_1(id)
            if nargin > 0
                obj.slave = obj.find(id);
            end
        end

        %====================================================================
        function rv = configure(obj,status,vector,scale,dc,filter)

            rv.SlaveConfig.vendor = 2;
            rv.SlaveConfig.description = obj.slave{1};
            rv.SlaveConfig.product  = obj.slave{2};
            rv.function = obj.slave{4};

            pdo_count = str2double(obj.slave{1}(6));

            if status
                pdo_idx = obj.slave{6};
                value_idx = 10;
            else
                pdo_idx = obj.slave{5};
                value_idx = 0;
            end

            rv.SlaveConfig.sm = {{3,1, arrayfun(@(i) obj.pdo(i,:), ...
                                         pdo_idx:(pdo_idx+pdo_count-1), ...
                                         'UniformOutput', false)}};
            if dc(1) > 3
                rv.SlaveConfig.dc = dc(2:11);
            else
                rv.SlaveConfig.dc = obj.dc{dc(1),2};
            end

            pdo = repmat([0,0,value_idx,0],pdo_count,1);
            pdo(:,2) = 0:pdo_count-1;

            rv.PortConfig.output = ...
                obj.configurePorts('Ch.',pdo,sint(16),vector,scale);

            if status
                pdo(:,3) = 4;

                if vector
                    n = 1;
                else
                    n = pdo_count;
                end
                
                rv.PortConfig.output(end+(1:n)) = obj.configurePorts(...
                        'St.',pdo,uint(1),vector,isa(scale,'struct'));
            end
            
            rv.SlaveConfig.sdo = {
                hex2dec('8000'),hex2dec(' 6'), 8,double(filter > 1);
                hex2dec('8000'),hex2dec('15'),16,max(0,filter-2);
            };

        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods (Static)
        %====================================================================
        function test(p)
            ei = EtherCATInfo(fullfile(p,'Beckhoff EL31xx.xml'));
            for i = 1:size(el31xx_1.models,1)
                fprintf('Testing %s\n', el31xx_1.models{i,1});
                slave = ei.getSlave(el31xx_1.models{i,2},...
                        'revision', el31xx_1.models{i,3});

                rv = el31xx_1(el31xx_1.models{i,1}).configure(0,i&1,...
                        EtherCATSlave.configureScale(2^15,''),2,1);
                slave.testConfig(rv.SlaveConfig,rv.PortConfig);

                rv = el31xx_1(el31xx_1.models{i,1}).configure(1,i&1,...
                        EtherCATSlave.configureScale(2^15,'6'),rem(i,3)+1,2);
                slave.testConfig(rv.SlaveConfig,rv.PortConfig);
            end
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties (Constant)

        %              PdoEntry       EntryIdx, SubIdx, Bitlen
        pdo = { % For all slaves except EL31.2 without status
                hex2dec('1a01'), [hex2dec('6000') , 17, 16];     %1
                hex2dec('1a03'), [hex2dec('6010') , 17, 16];     %2
                hex2dec('1a05'), [hex2dec('6020') , 17, 16];     %3
                hex2dec('1a07'), [hex2dec('6030') , 17, 16];     %4

                % For EL31.2 without status
                hex2dec('1a03'), [hex2dec('6000') , 17, 16];     %5
                hex2dec('1a05'), [hex2dec('6010') , 17, 16];     %6

                % With status
                hex2dec('1a00'), [hex2dec('6000') ,  1,  1;      %7
                                  hex2dec('6000') ,  2,  1;
                                  hex2dec('6000') ,  3,  2;
                                  hex2dec('6000') ,  5,  2;
                                  hex2dec('6000') ,  7,  1;
                                                0 ,  0,  1;
                                                0 ,  0,  5;
                                  hex2dec('1c32') , 32,  1;
                                  hex2dec('1800') ,  7,  1;
                                  hex2dec('1800') ,  9,  1;
                                  hex2dec('6000') , 17, 16];
                hex2dec('1a02'), [hex2dec('6010') ,  1,  1;      %8
                                  hex2dec('6010') ,  2,  1;
                                  hex2dec('6010') ,  3,  2;
                                  hex2dec('6010') ,  5,  2;
                                  hex2dec('6010') ,  7,  1;
                                                0 ,  0,  1;
                                                0 ,  0,  5;
                                  hex2dec('1c32') , 32,  1;
                                  hex2dec('1802') ,  7,  1;
                                  hex2dec('1802') ,  9,  1;
                                  hex2dec('6010') , 17, 16];
                hex2dec('1a04'), [hex2dec('6020') ,  1,  1;      %9
                                  hex2dec('6020') ,  2,  1;
                                  hex2dec('6020') ,  3,  2;
                                  hex2dec('6020') ,  5,  2;
                                  hex2dec('6020') ,  7,  1;
                                                0 ,  0,  1;
                                                0 ,  0,  5;
                                  hex2dec('1c32') , 32,  1;
                                  hex2dec('1804') ,  7,  1;
                                  hex2dec('1804') ,  9,  1;
                                  hex2dec('6020') , 17, 16];
                hex2dec('1a06'), [hex2dec('6030') ,  1,  1;      %10
                                  hex2dec('6030') ,  2,  1;
                                  hex2dec('6030') ,  3,  2;
                                  hex2dec('6030') ,  5,  2;
                                  hex2dec('6030') ,  7,  1;
                                                0 ,  0,  1;
                                                0 ,  0,  5;
                                  hex2dec('1c32') , 32,  1;
                                  hex2dec('1806') ,  7,  1;
                                  hex2dec('1806') ,  9,  1;
                                  hex2dec('6030') , 17, 16];

                hex2dec('1a02'), [hex2dec('6000') ,  1,  1;      %11
                                  hex2dec('6000') ,  2,  1;
                                  hex2dec('6000') ,  3,  2;
                                  hex2dec('6000') ,  5,  2;
                                  hex2dec('6000') ,  7,  1;
                                                0 ,  0,  1;
                                                0 ,  0,  5;
                                  hex2dec('1c33') , 32,  1;
                                  hex2dec('1802') ,  7,  1;
                                  hex2dec('1802') ,  9,  1;
                                  hex2dec('6000') , 17, 16];
                hex2dec('1a04'), [hex2dec('6010') ,  1,  1;      %12
                                  hex2dec('6010') ,  2,  1;
                                  hex2dec('6010') ,  3,  2;
                                  hex2dec('6010') ,  5,  2;
                                  hex2dec('6010') ,  7,  1;
                                                0 ,  0,  1;
                                                0 ,  0,  5;
                                  hex2dec('1c33') , 32,  1;
                                  hex2dec('1804') ,  7,  1;
                                  hex2dec('1804') ,  9,  1;
                                  hex2dec('6010') , 17, 16];

                hex2dec('1a00'), [hex2dec('6000') ,  1,  1;      %13
                                  hex2dec('6000') ,  2,  1;
                                  hex2dec('6000') ,  3,  2;
                                  hex2dec('6000') ,  5,  2;
                                  hex2dec('6000') ,  7,  1;
                                                0 ,  0,  1;
                                                0 ,  0,  5;
                                  hex2dec('1c33') , 32,  1;
                                  hex2dec('1800') ,  7,  1;
                                  hex2dec('1800') ,  9,  1;
                                  hex2dec('6000') , 17, 16];

                hex2dec('1a02'), [hex2dec('6000') ,  1,  1;      %14
                                  hex2dec('6000') ,  2,  1;
                                  hex2dec('6000') ,  3,  2;
                                  hex2dec('6000') ,  5,  2;
                                  hex2dec('6000') ,  7,  1;
                                                0 ,  0,  1;
                                                0 ,  0,  5;
                                  hex2dec('6000') , 14,  1;
                                  hex2dec('6000') , 15,  1;
                                  hex2dec('6000') , 16,  1;
                                  hex2dec('6000') , 17, 16];
                hex2dec('1a04'), [hex2dec('6010') ,  1,  1;      %15
                                  hex2dec('6010') ,  2,  1;
                                  hex2dec('6010') ,  3,  2;
                                  hex2dec('6010') ,  5,  2;
                                  hex2dec('6010') ,  7,  1;
                                                0 ,  0,  1;
                                                0 ,  0,  5;
                                  hex2dec('6010') , 14,  1;
                                  hex2dec('6010') , 15,  1;
                                  hex2dec('6010') , 16,  1;
                                  hex2dec('6010') , 17, 16];

                hex2dec('1a00'), [hex2dec('6000') ,  1,  1;      %16
                                  hex2dec('6000') ,  2,  1;
                                  hex2dec('6000') ,  3,  2;
                                  hex2dec('6000') ,  5,  2;
                                  hex2dec('6000') ,  7,  1;
                                                0 ,  0,  1;
                                                0 ,  0,  5;
                                  hex2dec('6000') , 14,  1;
                                  hex2dec('6000') , 15,  1;
                                  hex2dec('6000') , 16,  1;
                                  hex2dec('6000') , 17, 16];
                hex2dec('1a02'), [hex2dec('6010') ,  1,  1;      %17
                                  hex2dec('6010') ,  2,  1;
                                  hex2dec('6010') ,  3,  2;
                                  hex2dec('6010') ,  5,  2;
                                  hex2dec('6010') ,  7,  1;
                                                0 ,  0,  1;
                                                0 ,  0,  5;
                                  hex2dec('6010') , 14,  1;
                                  hex2dec('6010') , 15,  1;
                                  hex2dec('6010') , 16,  1;
                                  hex2dec('6010') , 17, 16];
                hex2dec('1a04'), [hex2dec('6020') ,  1,  1;      %18
                                  hex2dec('6020') ,  2,  1;
                                  hex2dec('6020') ,  3,  2;
                                  hex2dec('6020') ,  5,  2;
                                  hex2dec('6020') ,  7,  1;
                                                0 ,  0,  1;
                                                0 ,  0,  5;
                                  hex2dec('6020') , 14,  1;
                                  hex2dec('6020') , 15,  1;
                                  hex2dec('6020') , 16,  1;
                                  hex2dec('6020') , 17, 16];
                hex2dec('1a06'), [hex2dec('6030') ,  1,  1;      %19
                                  hex2dec('6030') ,  2,  1;
                                  hex2dec('6030') ,  3,  2;
                                  hex2dec('6030') ,  5,  2;
                                  hex2dec('6030') ,  7,  1;
                                                0 ,  0,  1;
                                                0 ,  0,  5;
                                  hex2dec('6030') , 14,  1;
                                  hex2dec('6030') , 15,  1;
                                  hex2dec('6030') , 16,  1;
                                  hex2dec('6030') , 17, 16];
        };


        %   Model   ProductCode  Revision         Function NoStatus WithStatus;
        models = {
          'EL3101',      hex2dec('0c1d3052'), hex2dec('00120000'), '+-10V' , 1, 16;
          'EL3102',      hex2dec('0c1e3052'), hex2dec('00130000'), '+-10V' , 5, 14;
          'EL3104',      hex2dec('0c203052'), hex2dec('00120000'), '+-10V' , 1, 16;
          'EL3111',      hex2dec('0c273052'), hex2dec('00120000'), '0-20mA', 1, 16;
          'EL3112',      hex2dec('0c283052'), hex2dec('00130000'), '0-20mA', 5, 14;
          'EL3114',      hex2dec('0c2a3052'), hex2dec('00130000'), '0-20mA', 1, 16;
          'EL3121',      hex2dec('0c313052'), hex2dec('00120000'), '4-20mA', 1, 16;
          'EL3122',      hex2dec('0c323052'), hex2dec('00130000'), '4-20mA', 5, 14;
          'EL3124',      hex2dec('0c343052'), hex2dec('00120000'), '4-20mA', 1, 16;
          'EL3141',      hex2dec('0c453052'), hex2dec('00120000'), '0-20mA', 1, 16;
          'EL3142',      hex2dec('0c463052'), hex2dec('00130000'), '0-20mA', 5, 14;
          'EL3142-0010', hex2dec('0c463052'), hex2dec('0014000a'), '+-10mA', 5, 14;
          'EL3144',      hex2dec('0c483052'), hex2dec('00120000'), '0-20mA', 1, 16;
          'EL3151',      hex2dec('0c4f3052'), hex2dec('00120000'), '4-20mA', 1, 16;
          'EL3152',      hex2dec('0c503052'), hex2dec('00130000'), '4-20mA', 5, 14;
          'EL3154',      hex2dec('0c523052'), hex2dec('00120000'), '4-20mA', 1, 16;
          'EL3161',      hex2dec('0c593052'), hex2dec('00120000'), '0-10V' , 1, 16;
          'EL3162',      hex2dec('0c5a3052'), hex2dec('00130000'), '0-10V' , 5, 14;
          'EL3164',      hex2dec('0c5c3052'), hex2dec('00120000'), '0-10V' , 1, 16;
          'EL3101-0000-0016', hex2dec('0c1d3052'), hex2dec('00100000'), '+-10V' , 1,  7;
          'EL3102-0000-0017', hex2dec('0c1e3052'), hex2dec('00110000'), '+-10V' , 5, 11;
          'EL3104-0000-0016', hex2dec('0c203052'), hex2dec('00100000'), '+-10V' , 1,  7;
          'EL3111-0000-0016', hex2dec('0c273052'), hex2dec('00100000'), '0-20mA', 1, 13;
          'EL3112-0000-0017', hex2dec('0c283052'), hex2dec('00110000'), '0-20mA', 5, 11;
          'EL3114-0000-0017', hex2dec('0c2a3052'), hex2dec('00110000'), '0-20mA', 1,  7;
          'EL3121-0000-0016', hex2dec('0c313052'), hex2dec('00100000'), '4-20mA', 1,  7;
          'EL3122-0000-0017', hex2dec('0c323052'), hex2dec('00110000'), '4-20mA', 5, 11;
          'EL3124-0000-0016', hex2dec('0c343052'), hex2dec('00100000'), '4-20mA', 1,  7;
          'EL3141-0000-0016', hex2dec('0c453052'), hex2dec('00100000'), '0-20mA', 1,  7;
          'EL3142-0000-0017', hex2dec('0c463052'), hex2dec('00110000'), '0-20mA', 5, 11;
          'EL3142-0010-0017', hex2dec('0c463052'), hex2dec('0011000a'), '+-10mA', 5, 11;
          'EL3144-0000-0016', hex2dec('0c483052'), hex2dec('00100000'), '0-20mA', 1,  7;
          'EL3151-0000-0016', hex2dec('0c4f3052'), hex2dec('00100000'), '4-20mA', 1,  7;
          'EL3152-0000-0017', hex2dec('0c503052'), hex2dec('00110000'), '4-20mA', 5, 11;
          'EL3154-0000-0016', hex2dec('0c523052'), hex2dec('00100000'), '4-20mA', 1,  7;
          'EL3161-0000-0016', hex2dec('0c593052'), hex2dec('00100000'), '0-10V' , 1,  7;
          'EL3162-0000-0017', hex2dec('0c5a3052'), hex2dec('00110000'), '0-10V' , 5, 11;
          'EL3164-0000-0016', hex2dec('0c5c3052'), hex2dec('00100000'), '0-10V' , 1,  7;
        };

        dc = {'Free Run/SM-Synchron', repmat(0,10,1);
              'DC-Synchron',  [hex2dec('700'), 0, 1, 0, 0, 0, 0, 1, 5000, 0];
              'DC-Synchron (input based)', ...
                              [hex2dec('700'), 0, 1, 0, 0, 1, 0, 1, 5000, 0];
        };
    end
end
