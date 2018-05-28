classdef XmlNode
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods
        %------------------------------------------------------------------
        function obj = XmlNode(n)
            if isa(n,'XmlNode')
                obj.node = n.node;
            else
                obj.node = n;
            end
        end

        %------------------------------------------------------------------
        function node = getFirstNode(xmlNode,name)
            n = xmlNode.node.getChildNodes;
            for i = 0:n.getLength-1
                x = n.item(i);
                if x.getNodeType == x.ELEMENT_NODE ...
                        && x.getNodeName.equals(name)
                    node = XmlNode(x);
                    return
                end
            end
            node = [];
        end

        %------------------------------------------------------------------
        function nodes = getNodes(xmlNode,name)
            n = xmlNode.node.getChildNodes;
            nodes = {};
            for i = 0:n.getLength-1
                x = n.item(i);
                if x.getNodeType == x.ELEMENT_NODE ...
                        && x.getNodeName.equals(name)
                    nodes{end+1} = XmlNode(x);
                end
            end
        end

        %------------------------------------------------------------------
        function attr = getAttribute(xmlNode,name)
            attr = char(xmlNode.node.getAttribute(name));
        end

        %------------------------------------------------------------------
        function t = getTextContent(xmlNode)
            t = char(xmlNode.node.getTextContent);
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties (SetAccess = private)
        node
    end
end
