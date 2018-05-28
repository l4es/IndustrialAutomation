classdef EtherCATInfoSlave < XmlNode
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods
        %------------------------------------------------------------------
        function slave = EtherCATInfoSlave(s)
            slave = slave@XmlNode(s);
        end

        %------------------------------------------------------------------
        function rv = Name(s)
            type = s.getFirstNode('Type');
            rv = char(type.getTextContent);
        end

        %------------------------------------------------------------------
        function rv = ProductCode(s)
            type = s.getFirstNode('Type');
            pc = type.getAttribute('ProductCode');
            rv = EtherCATInfo.hexDecValue(char(pc));
        end

        %------------------------------------------------------------------
        function rv = hideTypes(s)
            rv = cellfun(@(x) EtherCATInfo.hexDecValue(...
                                x.getAttribute('RevisionNo')), ...
                         s.getNodes('HideType'));
        end

        %------------------------------------------------------------------
        function rv = getObject(s)
            dict = s.getFirstNode('Profile').getFirstNode('Dictionary');
            rv = cellfun(@(o) struct('node', o, ...
                                     'index', o.getFirstNode('Index').getTextContent, ...
                                     'type', o.getFirstNode('Type').getTextContent, ...
                                     'name', o.getFirstNode('Name').getTextContent), ...
                        dict.getFirstNode('Objects').getNodes('Object'));
        end

        %------------------------------------------------------------------
        function rv = getTypes(s)
            dict = s.getFirstNode('Profile').getFirstNode('Dictionary');
            rv = cellfun(@(t) struct('node', t, ...
                                     'name', t.getFirstNode('Name').getTextContent), ...
                        dict.getFirstNode('DataTypes').getNodes('DataType'));
        end

        %------------------------------------------------------------------
        function rv = RevisionNumber(s)
            type = s.getFirstNode('Type');
            pc = type.getAttribute('RevisionNo');
            rv = EtherCATInfo.hexDecValue(char(pc));
        end

        %------------------------------------------------------------------
        function rv = testConfig(obj,SlaveConfig,PortConfig)

            % Compare description
            if isfield(SlaveConfig,'description')
                len = min(length(obj.Name), length(SlaveConfig.description));
                if ~strncmp(obj.Name, SlaveConfig.description, len)
                    fprintf('Slave Description for #x%x do not match\n',...
                        SlaveConfig.product);
                end
            end

            % Finish here if there is no .sm
            if ~isfield(SlaveConfig,'sm') || ~numel(SlaveConfig.sm)
                return
            end

            dc = obj.getFirstNode('Dc');
            if isfield(SlaveConfig,'dc') && ~isempty(SlaveConfig.dc) && ...
                     SlaveConfig.dc(1) ~= 0

                if isempty(dc)
                    disp('Slave does not have Dc, but DC is configured in SlaveConfig')
                else
                    if numel(SlaveConfig.dc) ~= 10
                        fprintf('DC has %i elements instead of 10\n', ...
                            numel(SlaveConfig.dc));
                    end

                    assignActivate = cellfun(@(x) EtherCATInfo.hexDecValue(x.getFirstNode('AssignActivate').getTextContent), ...
                            dc.getNodes('OpMode'));
                    if ~any(ismember(SlaveConfig.dc(1), assignActivate))
                        fprintf('DC AssignActivate=#x%x is not found\n', ...
                            SlaveConfig.dc(1));
                    end
                end
            end

            sm = obj.getNodes('Sm');
            for i = 1:numel(SlaveConfig.sm)
                % .sm must have 3 elements
                if numel(SlaveConfig.sm{i}) < 3
                    fprintf('SlaveConfig.sm{%i} does not have 3 elements\n',i);
                    return
                end
                smIdx = SlaveConfig.sm{i}{1}+1;

                % Test whether there are enough <Sm>'s
                if smIdx > numel(sm)
                    fprintf('SmIdx SlaveConfig.sm{%i}{1}=%i does not exist\n',...
                        i, SlaveConfig.sm{i}{1});
                    return
                end

                dirText = sm{smIdx}.getTextContent;
                control_byte = EtherCATInfo.hexDecValue(sm{smIdx}.getAttribute('ControlByte'));

                % Check Sm direction
                % {2} == 0 is for RxPdo's
                % {2} == 1 is for TxPdo's
                if (SlaveConfig.sm{i}{2} ...
                        && (~(isempty(dirText) || strcmp(dirText,'Inputs')) ...
                             || bitand(control_byte,4)) ...
                   || (~SlaveConfig.sm{i}{2} ...
                        && (~(isempty(dirText) || strcmp(dirText,'Outputs')) ...
                             || ~bitand(control_byte,4))))
                    fprintf('Sm direction of SlaveConfig.sm{i}{2}=%i is incorrect\n', ...
                        i, SlaveConfig.sm{i}{2});
                end

                if SlaveConfig.sm{i}{2}
                    pdo_dir = 'TxPdo';
                else
                    pdo_dir = 'RxPdo';
                end
                pdo = obj.getNodes(pdo_dir);
                pdoIndex = cellfun(...
                        @(x) EtherCATInfo.hexDecValue(...
                            x.getFirstNode('Index').getTextContent), ...
                        pdo);
                osIndexInc = cellfun(...
                        @(x) str2double(x.getAttribute('OSIndexInc')), ...
                        pdo);

                for j = 1:numel(SlaveConfig.sm{i}{3})
                    n = find(pdoIndex == SlaveConfig.sm{i}{3}{j}{1},1);
                    obj.testPdoEntry(i,j,SlaveConfig.sm{i}{3}{j}{2}, ...
                                 pdo{n},osIndexInc(n));
                end

                txt = cellfun(@(x) x.getAttribute('Mandatory'), pdo, ...
                              'UniformOutput', false);
                mandatory = str2double(txt);
                x = isnan(mandatory);
                mandatory(x) = 0;
                mandatory = (mandatory | strcmp(txt,'true')) ...
                        & cellfun(@(x) str2double(x.getAttribute('Sm')) == SlaveConfig.sm{i}{1}, ...
                                   pdo);

                mandatory = cellfun(@(x) EtherCATInfo.hexDecValue(x.getFirstNode('Index').getTextContent), ...
                                    pdo(mandatory));

                missing = setdiff(mandatory,...
                                  cellfun(@(x) x{1}, SlaveConfig.sm{i}{3}));

                if ~isempty(missing)
                    x = cell2mat(strcat('#x',cellstr(dec2hex(missing)),',')');
                    x(end) = [];
                    fprintf('Mandatory pdo''s %s have not been mapped\n',x)
                end
            end
        end

        %------------------------------------------------------------------
        function testPdoEntry(obj,i,j,sc,pdo,OSIndexInc)
            if isempty(pdo)
                fprintf('SlaveConfig.sm{%i}{3}{%i}: Pdo does not exist\n', ...
                        i,j)
                return
            end

            entry = pdo.getNodes('Entry');

            if isnan(OSIndexInc)
                os_count = 1;
                OSIndexInc = 0;
            else
                os_count = floor(size(sc,1) / numel(entry));
            end

            for k = 1:size(sc,1)
                os_idx = floor((k-1) / numel(entry));
                n = k - os_idx * numel(entry);

                if numel(entry) < n
                    fprintf('SlaveConfig.sm{%i}{3}{%i}{2}(%i,:): Pdo does not have Entry[%i]\n', ...
                        i,j,k,k);
                    continue
                end

                index = EtherCATInfo.hexDecValue(entry{n}.getFirstNode('Index').getTextContent) + os_idx*OSIndexInc;
                bitLen = str2double(entry{n}.getFirstNode('BitLen').getTextContent);
                if sc(k,1) ~= index
                    fprintf('Index SlaveConfig.sm{%i}{3}{%i}{2}(%i,1)=#x%x is incorrect, xml=#x%x\n', ...
                        i,j,k, sc(k,1), index);
                end

                if sc(k,3) ~= bitLen
                    fprintf('BitLen SlaveConfig.sm{%i}{3}{%i}{2}(%i,1)=%i is incorrect, xml=%i\n', ...
                        i,j,k, sc(k,3), bitLen);
                end

                if ~index
                    continue
                end

                subIndex = EtherCATInfo.hexDecValue(entry{n}.getFirstNode('SubIndex').getTextContent);
                if sc(k,2) ~= subIndex
                    fprintf('SubIndex SlaveConfig.sm{%i}{3}{%i}{2}(%i,2)=%i is incorrect, xml=%i\n', ...
                        i,j,k, sc(k,2), subIndex);
                end
            end
        end
    end
end
