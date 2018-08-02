define(["dojo/_base/declare",
    "dojo/request",
    "dojo/json",
    "dijit/Tree",
    "dijit/Menu",
    "dijit/MenuItem",
    "dijit/PopupMenuItem",
    "dijit/ConfirmDialog",
    "dijit/form/TextBox",
    "dojo/keys",
    "dijit/TooltipDialog",
    "dijit/popup",
    "dojo/i18n!scadabr/desktop/nls/messages"
], function (declare, request, json, Tree, Menu, MenuItem, PopupMenuItem, ConfirmDialog, TextBox, keys, TooltipDialog, popup, messages) {
    var MyTreeNode = declare([Tree._TreeNode], {
        _setLabelAttr: {node: "labelNode", type: "innerHTML"}
    });
    return declare("scadabr/desktop/NavigationTree", [Tree], {
        baseHref: null,
        nodeMenues: null,
        initialSelectedNode: 'ROOT',
        initialSelectedNodeInProgress: false,
        onLoad: function () {
            this._selectNode();
        },
        selectNode: function (id) {
            this.initialSelectedNode = id;
            this._selectNode();
        },
        _selectNode: function () {
            if (this.initialSelectedNodeInProgress) {
                return;
            }
            this.initialSelectedNodeInProgress = true;
            var nodes = this.getNodesByItem(this.initialSelectedNode);
            if (nodes) {
                if (nodes.length === 1) {
                    var node = nodes[0];
                    if (!node) {
                        //fetch path and retry
                        var self = this;
                        this.model.fetchTreePathOfId(this.initialSelectedNode).then(function (treePath) {
                            var path = ["ROOT"];
                            treePath.forEach(function (id) {
                                path.push("" + id);
                            });
                            self.set('path', path);
                            this.initialSelectedNodeInProgress = false;
                        });
                        return;
                    }
                    var selectedId = null;
                    if (this.selectedNode) {
                        selectedId = this.selectedNode.id;
                    }
                    if (node.item.id !== selectedId) {
                        //vom server holen???
                        var path = node.getTreePath();
                        for (var i = 0; i < path.length; i++) {
                            path[i] = "" + path[i].id;
                        }
                        this.set('path', path);
                        this.initialSelectedNodeInProgress = false;
                        return;
                    }
                } else {
                    alert("Can't set selected node: " + nodes.length);
                    this.initialSelectedNodeInProgress = false;
                    return;
                }
            } else {
                alert("Ups no nodes found");
                this.initialSelectedNodeInProgress = false;
                return;
            }

        },
        postCreate: function () {
            this.nodeMenues = {};
            this.inherited(arguments);
        },
        destroy: function () {
            this.destroyNodeMenues();
        },
        _createTreeNode: function (args) {
            var result = new MyTreeNode(args);
            var nodeMenu = this.getNodeMenuFor(args.item.nodeType);
            nodeMenu.bindDomNode(result.domNode);
            return result;
        },
        getNodeMenuFor: function (nodeType) {
            var nodeMenu = this.nodeMenues[nodeType];
            if (nodeMenu === undefined) {
                switch (nodeType) {
                    case "ROOT":
                        nodeMenu = new Menu();
                        nodeMenu.addChild(this.createAddFolderMenuItem());
                        nodeMenu.addChild(this.createAddDataPointNodeMenuItem());
                        break;
                    case "POINT_FOLDER":
                        nodeMenu = new Menu();
                        nodeMenu.addChild(this.createAddFolderMenuItem());
                        nodeMenu.addChild(this.createAddDataPointNodeMenuItem());
                        nodeMenu.addChild(this.createRenameNodeMenuItem());
                        nodeMenu.addChild(this.createDeleteNodeMenuItem());
                        break;
                    case "DATA_POINT":
                        nodeMenu = new Menu();
                        nodeMenu.addChild(this.createRenameNodeMenuItem());
                        nodeMenu.addChild(this.createDeleteNodeMenuItem());
                        break;
                    default:
                        alert("Unknown node for NodeMenue: " + nodeType);
                }
                this.nodeMenues[nodeType] = nodeMenu;
            }
            return nodeMenu;
        },
        createAddFolderMenuItem: function () {
            var tree = this.tree;
            return new MenuItem({
                iconClass: "dijitIconAdd",
                label: messages['common.add'],
                onClick: function () {
                    var addFolderDialog = new ConfirmDialog({
                        title: "New Folder name localize ME!",
                        content: new TextBox({
                            value: "New Folder localize ME!",
                            name: "folderName",
                            onKeyUp: function (event) {
                                switch (event.keyCode) {
                                    case keys.ESCAPE:
                                        addFolderDialog.onCancel();
                                        break;
                                    case keys.ENTER:
                                        addFolderDialog._onSubmit();
                                        break;
                                }
                            }
                        }),
                        execute: function (formContents) {
                            tree.model.add({name: formContents.folderName, nodeType: "POINT_FOLDER", scadaBrType: "POINT_FOLDER"}, {parent: tree.selectedItem});
                        }
                    });
                    addFolderDialog.show();
                }

            });
        },
        createAddDataPointNodeMenuItem: function () {
            var tree = this.tree;
            var addDataPointNodeMenu = new Menu({});
            var addDataPointNodeMenuItem = new PopupMenuItem({iconClass: "dijitIconAdd", label: "Add DataPoint", popup: addDataPointNodeMenu});
            request("RPC/DataPoints/DataTypes", tree.model.prepareRequestParams.GET).then(function (dataTypes) {
                dataTypes = json.parse(dataTypes);
                var dataTypeKeys = Object.keys(dataTypes);
                for (var i = 0; i < dataTypeKeys.length; i++) {
                    addDataPointNodeMenu.addChild(new MenuItem({
                        iconClass: "dijitIconAdd",
                        label: "ADD DP: " + dataTypes[dataTypeKeys[i]],
                        _dataTypeKey: dataTypeKeys[i],
                        _dataTypeLabel: dataTypes[dataTypeKeys[i]],
                        onClick: function () {
                            var dataTypeKey = this._dataTypeKey;
                            var dataTypeLabel = this._dataTypeLabel;
                            var addDataPointDialog = new ConfirmDialog({
                                title: "New Data Point Name localize ME!",
                                content: new TextBox({
                                    value: dataTypeLabel,
                                    name: "dataPointName",
                                    onKeyUp: function (event) {
                                        switch (event.keyCode) {
                                            case keys.ESCAPE:
                                                addDataPointDialog.onCancel();
                                                break;
                                            case keys.ENTER:
                                                addDataPointDialog._onSubmit();
                                                break;
                                        }
                                    }
                                }),
                                execute: function (formContents) {
                                    tree.model.add({name: formContents.dataPointName, nodeType: "DATA_POINT", dataType: dataTypeKey, scadaBrType: ["DATA_POINT", dataTypeKey].join(".")}, {parent: tree.selectedItem});
                                }
                            });
                            addDataPointDialog.show();
                        }
                    }));
                }
            }, function (error) {
                alert(error);
            });
            return addDataPointNodeMenuItem;
        },
        createDeleteNodeMenuItem: function () {
            var tree = this.tree;
            return new MenuItem({
                iconClass: "dijitIconDelete",
                label: messages['common.delete'],
                onClick: function () {

                    switch (tree.selectedItem.nodeType) {
                        case "ROOT":
                            alert("Wrong node " + tree.selectedItem);
                            break;
                        default :
                            tree.model.delete(tree.selectedItem);
                    }
                }
            });
        },
        createRenameNodeMenuItem: function () {
            var tree = this.tree;
            return new MenuItem({
                iconClass: "dijitIconEdit",
                label: messages['common.rename'],
                onClick: function () {
                    if (tree.selectedNode === null) {
                        return;
                    }
                    var nodeNameDialog = tree.createNodeNameDialog();
                    popup.open({
                        popup: nodeNameDialog,
                        around: tree.selectedNode.contentNode
                    });
                    nodeNameDialog.focus();
                }

            });
        },
        createNodeNameDialog: function () {
            var tree = this.tree;
            var nodeNameDialog = new TooltipDialog({
                content: new TextBox({
                    value: tree.selectedItem.name,
                    onKeyUp: function (event) {
                        if (event.keyCode === keys.ESCAPE) {
                            popup.close(nodeNameDialog);
//TODO ?? check if nessecary                            nodeNameDialog.destroy();
                        } else if (event.keyCode === keys.ENTER) {
                            popup.close(nodeNameDialog);
                            var treeNode = tree.selectedNode;
                            var node = tree.selectedItem;
                            node.name = this.get('value');
                            tree.model.put(node);
                            treeNode.focus();
                        }
                    }
                })
            });
            return nodeNameDialog;
        },
        destroyNodeMenues: function () {

        },
        getIconClass: function (item, opened) {
            switch (item.nodeType) {
                case "ROOT":
                    return "dsIcon";
                case "PointFolder":
                    return (!item || this.model.mayHaveChildren(item)) ? (opened ? "dijitFolderOpened" : "dijitFolderClosed") : "dijitLeaf";
                case "DataPoint":
                    return item.enabled ? "plRunningIcon" : "plStoppedIcon";
                default:
                    return (!item || this.model.mayHaveChildren(item)) ? (opened ? "dijitFolderOpened" : "dijitFolderClosed") : "dijitLeaf";
            }
        },
        onClick: function (node) {
            window.location = this.baseHref + "?id=" + node.id;
        }
    });
});
