classdef EtherCATSlave

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods
        %====================================================================
        function checkModelName(obj)
            %% Check whether the slave name changed based on revision information
            %
            % This function requires a (hidden) mask variable called 'revision'.
            % The value is a vector with the fields [ProductCode Revision]
            % When a product is outdated, its name may change from
            % say EL3054 to EL3054-000-0011 for instance and a new slave with
            % named EL3054 is created.
            %
            % This function uses the revision information to check whether the
            % name has changed and sets the new model accordingly.

            % Ignore if block is in a library or block does not have
            % a revision field
            if strcmp(bdroot, 'etherlab_lib') || ...
                        ~any(strcmp(get_param(gcbh, 'MaskNames'), 'revision'))
                return
            end

            revision = get_param(gcbh, 'revision');
            if ~isempty(revision)
                revision = eval(revision);
            end

            if isempty(revision) || ~revision(1)
                obj.updateRevision()
                return
            end

            slave = obj.find(eval(get_param(gcbh, 'revision')));
            model = slave{1};
            if ~isempty(model) && ~strcmp(model, get_param(gcbh,'model'))
                disp([gcb ': Setting model from ' get_param(gcbh,'model') ' to ' model])
                set_param(gcbh, 'model', model)
            end
        end

        %====================================================================
        function updateRevision(obj)
            %% Set revision mask variable to values based on model mask variable
            %if isempty(get_param(gcbh, 'LibraryVersion'))
            if strcmp(bdroot, 'etherlab_lib')
                revision = '';
            else
                revision = mat2str(obj.findRevision(get_param(gcbh, 'model')));
            end

            if ~strcmp(get_param(gcbh, 'revision'), revision)
                set_param(gcbh, 'revision', revision)
            end
        end

        %====================================================================
        function revision = findRevision(obj, id)
            slave = obj.find(id);
            revision = [slave{[2,3]}];
        end

        %====================================================================
        % Return the row where the name in the first column matches
        % if id is a string, or columns 2 and 3 match if id is numeric
        function slave = find(obj, id)
            row = [];
            if isnumeric(id)
                row = ismember(cell2mat(obj.models(:,[2 3])), id, 'rows');
            elseif isstr(id)
                row = strcmp(obj.models(:,1), id);
            end

            slave = obj.models(row,:);
        end

        %====================================================================
        function updateModels(obj)
            %% Keeps the list model list up to date
            % Arguments
            %       list: cell array where first column is the list of models
            %
            % Requires the following mask variables:
            %       model

            if verLessThan('Simulink','8.0')
                style = get_param(gcbh,'MaskStyles');
                names = get_param(gcbh,'MaskNames');

                pos = find(strcmp(names,'model'));

                modelstr = ['popup(', EtherCATSlave.strJoin(obj.models(:,1),'|'), ')'];
                if ~strcmp(style{pos}, modelstr)
                    style{pos} = modelstr;
                    set_param(gcbh,'MaskStyles', style);
                    display(['Updated ', gcb])
                end
            else
                p = Simulink.Mask.get(gcbh);
                pos = find(strcmp({p.Parameters.Name}, 'model'), 1);
                if ~isequal(p.Parameters(pos).TypeOptions, obj.models)
                    p.Parameters(pos).set('TypeOptions', obj.models(:,1));
                    display(['Updated ', gcb])
                end
            end

            set_param(gcbh, 'revision', '')
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods (Static)
        %====================================================================
        function rv = formatAddress(master,index,tsample)
            rv.master = master(1);

            if numel(tsample) > 1
                rv.domain = tsample(2);
            else
                rv.domain = 0;
            end

            if numel(index) > 1
                rv.alias = index(1);
                rv.position = index(2);
            else
                rv.alias = 0;
                rv.position = index(1);
            end
        end
 
        %====================================================================
        function scale = configureScale(full_scale,gain,offset,filter,pfx)
            %% Return a structure with fields
            %   scale.full_scale = full_scale
            %   scale.gain       = evalin('base',gain)
            %   scale.offset     = evalin('base',offset)
            %   scale.filter      = evalin('base',filter)
            %   scale.prefix      = evalin('base',filter)
            % if gain, offset and filter are not all ''
            % otherwise, return false

            % Make sure all options are set
            if nargin < 3
                offset = [];
            end
            if nargin < 4
                filter = [];
            end

            % If everything is empty, no scaling and return
            if isempty([gain,offset,filter])
                scale = false;
                return
            end

            % Make sure every parameter is set
            scale.gain = gain;
            scale.offset = offset;
            scale.filter = filter;
            if nargin >= 5
                scale.prefix = pfx;
            end

            if isempty(scale.gain) && isempty(scale.offset)
                scale.full_scale = 1;
            else
                scale.full_scale = full_scale;
            end
        end

        %====================================================================
        function checkFilter
            omega = get_param(gcbh,'omega');
            if ~(isempty(omega) || strcmp(omega, '[]'))
                disp(sprintf(['%s: Deprecated use of LPF Frequency dialog parameter. ' ...
                     'See <matlab:web(etherlab_help_path(''general.html#filter''), ''-helpbrowser'')>'], ...
                    gcb))
            end
        end

        %====================================================================
        function setPortNames(input,output,deflt)
            fmt = 'port_label(''%s'', %i, ''%s'')\n';

            if nargin > 3
                str = {sprintf('disp(''%s'')\n',deflt)};
            else
                str = {''};
            end

            str = horzcat(str, ...
                    arrayfun(@(x) sprintf(fmt, 'input', x, input{x}), ...
                             1:numel(input), 'UniformOutput', false), ...
                    arrayfun(@(x) sprintf(fmt, 'output', x, output{x}), ...
                             1:numel(output), 'UniformOutput', false));

            str = cell2mat(str);

            set_param(gcbh, 'MaskDisplay', str);
        end

        %====================================================================
        function updateCustomDCEnable()
            names = get_param(gcbh,'MaskNames');

            % This function sets the enable state of all mask variables
            % where custom values for distributed clock can be entered.
            % if dc_mode == 'Custom' they are enabled
            %
            % Requires the following mask variables:
            %       dc_mode, dc_*

            dc = strncmp(names,'dc_',3);

            % Mask out dc_mode itself
            dc(strcmp(names,'dc_mode')) = false;

            EtherCATSlave.setEnable(dc, ...
                    strcmp(get_param(gcbh,'dc_mode'),'Custom'));
        end

        %====================================================================
        function updateDCVisibility(state)
            %% Change the visibility (on/off) of all mask variables
            % starting with 'dc_' depending on <state>
            %
            % Requires the following mask varialbes:
            %       dc_mode, dc_*

            dc = strncmp(get_param(gcbh,'MaskNames'),'dc_',3);

            EtherCATSlave.setVisible(dc, state);
        end

        %====================================================================
        function updatePDOVisibility(list)
            %% Make mask variables of type pdo_xXXXX visible
            %    list       - double vector of indices
            [idx,enable] = EtherCATSlave.getVariableList('pdo_x',...
                                                              dec2hex(list,4));
            EtherCATSlave.setVisible(idx,enable)
        end

        %====================================================================
        function updateSDOVisibility(sdoList)
            %% Change visibility (on/off) of SDO Mask Variables
            % SDO Variables start with 'sdo_'
            % Argument:
            %       sdoList: (cellarray of strings|string array)
            %                eg: cellstr(dec2hex(1:20,2));
            [idx,enable] = EtherCATSlave.getVariableList('sdo_', sdoList);
            EtherCATSlave.setVisible(idx,enable)
        end

        %====================================================================
        function updateSDOEnable(sdoList)
            %% Change enable state (gray/black) of SDO Mask Variables
            % SDO Variables start with 'sdo_'
            % Argument:
            %       sdoList: (cellarray of strings|string array)
            %                eg: cellstr(dec2hex(1:20,2));
            [idx,enable] = EtherCATSlave.getVariableList('sdo_', sdoList);
            EtherCATSlave.setEnable(idx, enable);
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods (Static, Access = protected)
        %====================================================================
        % This function returns an array that will address every
        % Pdo Entry that is non-zero and have the correct direction
        % given the SyncManager list
        % The return value is a Nx7 array, where columns
        %       1:4   = [SmIdx,PdoIdx,PdoEntryIdx,0] address
        %       5:6   = [PdoEntryIndex,PdoEntrySyubIndex,BitLen]
        function entry = findPdoEntries(Sm, dir, limit)
            if isempty(Sm)
                entry = [];
                return
            end

            % Find the syncmanagers with the correct directions
            % The result is a cell array where each cell consists of:
            %   SmIdx, -- Syncmanager Index
            %   Pdo,   -- Cell array of pdo's
            sm = arrayfun(@(smIdx) {smIdx-1,Sm{smIdx}{3}}, ...
                           find(cellfun(@(x) x{2} == dir, Sm)), ...
                           'UniformOutput', false);

            if isempty(sm)
                entry = [];
                return
            end

            % Split up the individual syncmanagers into pdo's
            % The result is a vertical cell array where each cell has
            %   SmIdx, -- Syncmanager Index
            %   PdoIdx, -- Index of pdo
            %   PdoEntry, -- Nx4 Array of Pdo Entries
            pdo = cellfun(...
                @(pdo) mat2cell(horzcat(repmat({pdo{1}},...
                                               numel(pdo{2}),1),...
                                        num2cell(0:numel(pdo{2})-1)',...
                                        reshape(pdo{2},[],1)),...
                                ones(1,numel(pdo{2}))),...
                sm, 'UniformOutput', false);
            pdo = vertcat(pdo{:});

            % Split up the individual pdo's into pdo entries
            % The result is an array where each row consists of
            %   SmIdx, -- Syncmanager Index
            %   PdoIdx, -- Index of pdo
            %   ElementIdx, -- (=0), Index of the element
            %   PdoEntryIdx, -- Index of Pdo Entry
            %   EtherCAT PdoEntryIndex
            %   EtherCAT PdoEntrySubIndex
            %   EtherCAT BitLen
            entry = cellfun(...
                @(e) horzcat(repmat([e{[1,2]},0],size(e{3}{2},1),1),...
                             (0:size(e{3}{2},1)-1)',...
                             e{3}{2}),...
                pdo, 'UniformOutput', false);
            entry = vertcat(entry{:});

            % - Remove rows where PdoEntryIndex == 0
            % - Swap columns 4 and 3
            if ~isempty(entry)
                entry = entry(entry(:,5) ~= 0,[1,2,4,3,5:end]);
            end

            % Limit count
            if nargin > 2
                entry(limit+1:end,:) = [];
            end
        end
        
        %====================================================================
        function port = configurePorts(name,pdo,dtype,vector,scale)

            if nargin < 5
                scale = false;
            end

            pdo_count = size(pdo,1);

            if ~pdo_count
                port = [];
                return
            end

            if isfield(scale, 'prefix')
                pfx = scale.prefix;
            else
                pfx = [];
            end

            if vector
                port.pdo = pdo(:,1:4);
                port.pdo_data_type = dtype;
                port.portname = name;

                if isa(scale,'struct')
                    port.full_scale = scale.full_scale;
                    port.gain   = {[pfx, 'Gain'],  scale.gain};
                    port.offset = {[pfx, 'Offset'],scale.offset};
                    port.filter = {[pfx, 'Filter'],scale.filter};
                elseif scale
                    port.full_scale = [];
                    port.gain   = [];
                    port.offset = [];
                    port.filter = [];
                end
            else
                port = arrayfun(...
                        @(i) struct('pdo',pdo(i,1:4),...
                                    'pdo_data_type',dtype,...
                                    'portname', [name,int2str(i)]), ...
                        1:pdo_count);

                if isa(scale,'struct') || scale
                    port(1).full_scale = [];
                    port(1).gain   = [];
                    port(1).offset = [];
                    port(1).filter = [];
                end

                if ~isa(scale,'struct')
                    return
                end

                % Replicate gain, offset and filter if there is only one element
                if numel(scale.gain) == 1
                    scale.gain = repmat(scale.gain,1,pdo_count);
                end
                if numel(scale.offset) == 1
                    scale.offset = repmat(scale.offset,1,pdo_count);
                end
                if numel(scale.filter) == 1
                    scale.filter = repmat(scale.filter,1,pdo_count);
                end

                for i = 1:pdo_count
                    idxstr = int2str(i);

                    port(i).full_scale = scale.full_scale;

                    if i <= numel(scale.gain)
                        port(i).gain   = {[pfx, 'Gain', idxstr], scale.gain(i)};
                    end

                    if i <= numel(scale.offset)
                        port(i).offset = {[pfx, 'Offset', idxstr], scale.offset(i)};
                    end

                    if i <= numel(scale.filter)
                        port(i).filter = {[pfx, 'Filter', idxstr], scale.filter(i)};
                    end
                end
            end
        end

        %====================================================================
        function setEnable(list, state)
            %% Change enabled state of a mask variable
            % Arguments:
            %   state/off: cellarray of strings or logical array
            %           to enable/disable a mask variable

            EtherCATSlave.setMaskState('MaskEnables', list, state);
        end

        %====================================================================
        function setVisible(list, state)
            %% Change visibility of a mask variable
            % Arguments:
            %   state/off: cellarray of strings or logical array
            %           to make visible/invisible

            EtherCATSlave.setMaskState('MaskVisibilities', list, state);
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods (Static, Access = private)

        %====================================================================
        function [idx,enable] = getVariableList(prefix, enableList)
            %% Return a list of indices for all mask names that
            % start with prefix. enable is also set when a variable is
            % in the enableList

            % Get a list of names that begin with prefix
            names = get_param(gcbh,'MaskNames');
            idx = strncmp(names,prefix,length(prefix));

            enable = ismember(names, strcat(prefix, enableList));
        end

        %====================================================================
        function setMaskState(variable, list, on)
            if numel(on) == 1
                on = repmat(on, size(list));
            end

            if ~islogical(list)
                names = get_param(gcbh, 'MaskNames');
                [list, pos] = ismember(names, list);
                i = on;
                on = list;
                on(list) = i(pos(list));
            end

            state = get_param(gcbh,variable);

            state(list &  on) = {'on'};
            state(list & ~on) = {'off'};

            if ~isequal(get_param(gcbh,variable),state)
                set_param(gcbh,variable,state);
            end
        end

        %====================================================================
        function rv = strJoin(s,delim)
            rv = cell2mat(strcat(reshape(s,1,[]),delim));
            rv(end) = [];
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties (Access = protected)
        slave = []
    end
end
