%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Encapsulation for Power Supply slaves EL9xxx
%
% Copyright (C) 2013 Richard Hacker
% License: LGPL
%
classdef ekxxxx < EtherCATSlave

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
methods
    %====================================================================
    function obj = ekxxxx(id)
        if nargin > 0
            obj.slave = obj.find(id);
        end
    end

    %========================================================================
    function rv = configure(obj,vector)

        % General information
        rv.SlaveConfig.vendor = 2;
        rv.SlaveConfig.product = obj.slave{2};
        rv.SlaveConfig.description = obj.slave{1};

        % Get the model's SM
        sm = ekxxxx.sm(obj.slave{4});
        rv.SlaveConfig.sm = sm;

        % Go through the rv.SlaveConfig.sm and reformat the PDO Entries
        % rv.SlaveConfig.sm{:}{3}{:}{2} to be an array (it is still
        % a cell at the moment)
        %
        % At the same time Create a cell array with 5 columns for
        % output_port and input_port:
        %       1: Sm Idx
        %       2: PDO Idx
        %       3: Entry Idx
        %       4: Element Idx
        %       5: PDO Entry name
        output_port = {};
        input_port  = {};
        for i = 1:numel(sm)
            pdo = rv.SlaveConfig.sm{i}{3};
            for j = 1:numel(pdo)
                entries = vertcat(pdo{j}{2}{:,1});
                rv.SlaveConfig.sm{i}{3}{j}{2} = entries;

                entry_idx = find(entries(:,1));
                n = numel(entry_idx);
                sm_idx      = num2cell((i-1)*ones(n,1));
                pdo_idx     = num2cell((j-1)*ones(n,1));
                element_idx = num2cell(zeros(n,1));
                ports = horzcat(sm_idx, ...
                                pdo_idx, ...
                                num2cell(entry_idx-1), ...
                                element_idx, ...
                                pdo{j}{2}(entry_idx,2));
                if sm{i}{2}
                    output_port = vertcat(output_port, ports);
                else
                    input_port  = vertcat(input_port,  ports);
                end
            end
        end

        % Only the output port may be boolean or uint16; select the bitlen
        % from the SyncManager
        % input ports are always boolean
        rv.PortConfig.output = struct('portname',{});
        rv.PortConfig.input  = struct('portname',{});
        if vector
            if ~isempty(output_port)
                bitlen = sm{output_port{1,1}+1}{3}{1}{2}{1,1}(3);
                rv.PortConfig.output = struct(...
                    'pdo', cell2mat(output_port(:,1:4)), ...
                    'pdo_data_type', uint(bitlen), ...
                    'portname','Out' ...
                );
            end
            if ~isempty(input_port)
                rv.PortConfig.input = struct(...
                    'pdo', cell2mat(input_port(:,1:4)), ...
                    'pdo_data_type', uint(1), ...
                    'portname','In' ...
                );
            end
        else    % vector
            if ~isempty(output_port)
                bitlen = sm{output_port{1,1}+1}{3}{1}{2}{1,1}(3);
                rv.PortConfig.output = struct(...
                    'pdo', arrayfun(@(x) [output_port{x,1:4}], ...
                                    1:size(output_port,1), ...
                                    'UniformOutput', false), ...
                    'pdo_data_type', uint(bitlen), ...
                    'portname', output_port(:,5)' ...
                );
            end
            if ~isempty(input_port)
                rv.PortConfig.input = struct(...
                    'pdo', arrayfun(@(x) [input_port{x,1:4}], ...
                                    1:size(input_port,1), ...
                                    'UniformOutput', false), ...
                    'pdo_data_type', uint(1), ...
                    'portname', input_port(:,5)' ...
                );
            end
        end     % vector
    end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
methods (Static)
    %====================================================================
    function test(p)
        ei = EtherCATInfo(fullfile(p,'Beckhoff EKxxxx.xml'));
        for i = 1:size(ekxxxx.models,1)-1
            fprintf('Testing %s\n', ekxxxx.models{i,1});
            slave = ei.getSlave(ekxxxx.models{i,2},...
                    'revision', ekxxxx.models{i,3});
            model = ekxxxx.models{i,1};

            rv = ekxxxx(model).configure(i&1);
            slave.testConfig(rv.SlaveConfig,rv.PortConfig);
        end

        ei = EtherCATInfo(fullfile(p,'Beckhoff CXxxxx.xml'));
        i = size(ekxxxx.models,1);
        fprintf('Testing %s\n', ekxxxx.models{i,1});
        slave = ei.getSlave(ekxxxx.models{i,2},...
                'revision', ekxxxx.models{i,3});
        model = ekxxxx.models{i,1};

        rv = ekxxxx(model).configure(i&1);
        slave.testConfig(rv.SlaveConfig,rv.PortConfig);
    end
end     % methods

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
properties (Constant, Access=private)

    % SyncManager class definitions
    % Column 4 of models selects the correct SyncManager
    sm = {
        % Input Sm (TxPdo) for EK1101 EK1501 EK1541
        {0, 1, {{hex2dec('1a00'), {[hex2dec('6000'), 1, 16], 'Id'}}}},

        % Output Sm (RxPdo) for EK1814
        {0, 0, {{hex2dec('1608'), {[hex2dec('7080'), 1, 1], 'Ch. 5'}},
                {hex2dec('1609'), {[hex2dec('7090'), 1, 1], 'Ch. 6'}},
                {hex2dec('160a'), {[hex2dec('70a0'), 1, 1], 'Ch. 7'}},
                {hex2dec('160b'), {[hex2dec('70b0'), 1, 1], 'Ch. 8'}}}},

        % Input Sm (TxPdo) for EK1814
        {1, 1, {{hex2dec('1a00'), {[hex2dec('6000'), 1, 1], 'Ch. 1'}},
                {hex2dec('1a01'), {[hex2dec('6010'), 1, 1], 'Ch. 2'}},
                {hex2dec('1a02'), {[hex2dec('6020'), 1, 1], 'Ch. 3'}},
                {hex2dec('1a03'), {[hex2dec('6030'), 1, 1], 'Ch. 4'}}}},

        % Output Sm (RxPdo) for EK1818
        {0, 0, {{hex2dec('1600'), {[hex2dec('7000'), 1, 1], 'Ch. 9'}},
                {hex2dec('1601'), {[hex2dec('7010'), 1, 1], 'Ch. 10'}},
                {hex2dec('1602'), {[hex2dec('7020'), 1, 1], 'Ch. 11'}},
                {hex2dec('1603'), {[hex2dec('7030'), 1, 1], 'Ch. 12'}}}},

        % Input Sm (TxPdo) for EK1818
        {1, 1, {{hex2dec('1a00'), {[hex2dec('6000'), 1, 1], 'Ch. 1'}},
                {hex2dec('1a01'), {[hex2dec('6010'), 1, 1], 'Ch. 2'}},
                {hex2dec('1a02'), {[hex2dec('6020'), 1, 1], 'Ch. 3'}},
                {hex2dec('1a03'), {[hex2dec('6030'), 1, 1], 'Ch. 4'}},
                {hex2dec('1a04'), {[hex2dec('6040'), 1, 1], 'Ch. 5'}},
                {hex2dec('1a05'), {[hex2dec('6050'), 1, 1], 'Ch. 6'}},
                {hex2dec('1a06'), {[hex2dec('6060'), 1, 1], 'Ch. 7'}},
                {hex2dec('1a07'), {[hex2dec('6070'), 1, 1], 'Ch. 8'}}}},

        % 1st Output SM (RxPdo) for EK1828 EK1828-0010
        {0, 0, {{hex2dec('1600'), {[              0, 0, 4], 'Pad';
                                   [hex2dec('7000'), 1, 1], 'Ch. 5'}},
                {hex2dec('1601'), {[hex2dec('7010'), 1, 1], 'Ch. 6'}},
                {hex2dec('1602'), {[hex2dec('7020'), 1, 1], 'Ch. 7'}},
                {hex2dec('1603'), {[hex2dec('7030'), 1, 1], 'Ch. 8'}}}},

        % 2st Output SM (RxPdo) for EK1828 EK1828-0010
        {1, 0, {{hex2dec('1604'), {[hex2dec('7040'), 1, 1], 'Ch. 9'}},
                {hex2dec('1605'), {[hex2dec('7050'), 1, 1], 'Ch. 10'}},
                {hex2dec('1606'), {[hex2dec('7060'), 1, 1], 'Ch. 11'}},
                {hex2dec('1607'), {[hex2dec('7070'), 1, 1], 'Ch. 12';
                                   [              0, 0, 4], 'Pad'   }}}},

        % Input SM (TxPdo) for EK1828
        {2, 1, {{hex2dec('1a00'), {[hex2dec('6000'), 1, 1], 'Ch. 1'}},
                {hex2dec('1a01'), {[hex2dec('6010'), 1, 1], 'Ch. 2'}},
                {hex2dec('1a02'), {[hex2dec('6020'), 1, 1], 'Ch. 3'}},
                {hex2dec('1a03'), {[hex2dec('6030'), 1, 1], 'Ch. 4';
                                   [              0, 0, 4], ''    }}}},
    };

end     % properties

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
properties (Constant)
    %  name          product code         revision   {PDO classes}
    % Note: to date 'Beckhoff EKxxx.xml Version 1.2', there are
    % no differences between the revisions, so column 3 only has the
    % version numbers
    models = {
      'EK1100',      hex2dec('044c2c52'), hex2dec('00000000'), [];
      'EK1100-0030', hex2dec('044c2c52'), hex2dec('0010001e'), [];
      'EK1101',      hex2dec('044d2c52'), hex2dec('00100000'), [1];
%      'EK1101-0080', hex2dec('044d2c52'), hex2dec('00000000'), [1];
      'EK1110',      hex2dec('04562c52'), hex2dec('00000000'), [];
      'EK1122',      hex2dec('04622c52'), hex2dec('00100000'), [];
%      'EK1122-0080', hex2dec('04622c52'), hex2dec('00000000'), [];
      'EK1200',      hex2dec('04b02c52'), hex2dec('00001388'), [];
      'EK1501',      hex2dec('05dd2c52'), hex2dec('00100000'), [1];
      'EK1501-0010', hex2dec('05dd2c52'), hex2dec('0010000a'), [1];
      'EK1521',      hex2dec('05f12c52'), hex2dec('00100000'), [];
      'EK1521-0010', hex2dec('05f12c52'), hex2dec('0010000a'), [];
      'EK1541',      hex2dec('06052c52'), hex2dec('00100000'), [1];
      'EK1561',      hex2dec('06192c52'), hex2dec('00100000'), [];
      'EK1814',      hex2dec('07162c52'), hex2dec('00100000'), [2,3];
      'EK1818',      hex2dec('071a2c52'), hex2dec('00100000'), [4,5];
      'EK1828',      hex2dec('07242c52'), hex2dec('00100000'), [6,7,8];
      'EK1828-0010', hex2dec('07242c52'), hex2dec('0010000a'), [6,7];
      'CX1100-0004', hex2dec('044c6032'), hex2dec('00000004'), [];
    };
end     % properties

end     % classdef
