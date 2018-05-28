classdef EtherCATInfo < XmlNode
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods (Static)
        function rv = hexDecValue(s)
            if numel(s) > 2 && strcmp(s(1:2),'#x')
                rv = hex2dec(s(3:end));
            else
                rv = str2double(s);
            end
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods
        %------------------------------------------------------------------
        function ei = EtherCATInfo(f)
            doc = xmlread(f);
            ei = ei@XmlNode(doc.getDocumentElement);
            ei.doc = doc;
        end

        %------------------------------------------------------------------
        function list = getSlaves(obj)
            descriptions = obj.getFirstNode('Descriptions');
            devices = descriptions.getFirstNode('Devices');
            slaves = devices.getNodes('Device');
            list = cellfun(@(x) EtherCATInfoSlave(x), slaves, ...
                                'UniformOutput', false);
        end

        %------------------------------------------------------------------
        function slaves = getSlave(obj,code,class,revision)
            %% Return a list of slaves
            % arguments:
            %   code: string:name | numeric:ProductCode
            %   class: 'class'|'revision'
            %   revision: number for class
            descriptions = obj.getFirstNode('Descriptions');
            devices = descriptions.getFirstNode('Devices');
            slaves = cellfun(@(x) EtherCATInfoSlave(x), ...
                             devices.getNodes('Device'), ...
                             'UniformOutput', false);

            if nargin == 1
                return
            elseif isstr(code)
                slaves = slaves(cellfun(@(s) strcmp(s.Name,code), slaves));
            elseif isnumeric(code)
                slaves = slaves(cellfun(@(s) s.ProductCode == code, slaves));
            else
                return
            end

            if nargin > 3
                switch class
                case 'class'
                    slaves = slaves(cellfun(@(s) (s.RevisionNumber & 65535) == revision, ...
                                            slaves));
                    hidden = unique(cell2mat(horzcat(...
                        cellfun(@(s) s.hideTypes,slaves,'UniformOutput', false))));
                    rev = cellfun(@(x) x.RevisionNumber, slaves);

                    slaves = slaves(~ismember(rev,hidden));
                case 'revision'
                    slaves = slaves{cellfun(@(s) s.RevisionNumber == revision, ...
                                            slaves)};
                end
            end
        end

        %------------------------------------------------------------------
        function testConfiguration(obj, SlaveConfig, PortConfig)
            if ~isempty(SlaveConfig)
                obj.testSlaveConfig(SlaveConfig,PortConfig)
            end
        end

        %------------------------------------------------------------------
        function testSlaveConfig(obj,SlaveConfig,PortConfig)
            % Make sure .vendor exists and is correct
            if ~isfield(SlaveConfig,'vendor')
                disp('Vendor missing');
            elseif EtherCATInfo.hexDecValue(obj.getFirstNode('Vendor').getFirstNode('Id').getTextContent) ~= SlaveConfig.vendor
                disp('Vendor incorrect');
            end

            % Make sure .product exists and is correct
            if ~isfield(SlaveConfig,'product')
                disp('ProductCode missing');
            else
                slave = obj.getSlave(SlaveConfig.product);
                if isempty(slave)
                    fprintf('Slave with ProductCode=#x%x does not exist\n', ...
                            SlaveConfig.product);
                    return
                end

                if numel(slave) > 1 && isfield(SlaveConfig,'description')
                    names = cellfun(@(x) x.Name, slave, 'UniformOutput', false);
                    slave = slave(strcmp(names,SlaveConfig.description));

                    if isempty(slave)
                        fprintf('Slave %s does not exist\n', ...
                                SlaveConfig.description);
                        return
                    end
                end

                slave{1}.testConfig(SlaveConfig,PortConfig);
                return
            end
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties (Access = private)
        doc
    end
end
