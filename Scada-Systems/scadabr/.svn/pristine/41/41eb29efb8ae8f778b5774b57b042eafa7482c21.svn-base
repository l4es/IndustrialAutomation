define(["dojo/_base/declare",
    "dijit/registry",
    "dijit/Tree",
    "dojo/store/JsonRest",
    "dojo/store/Observable",
    "dijit/Menu",
    "dijit/MenuItem",
    "dijit/PopupMenuItem",
    "dojo/rpc/JsonService",
    "dojo/ready",
    "dojo/i18n!scadabr/nls/messages"
], function (declare, registry, Tree, JsonRest, Observable, Menu, MenuItem, PopupMenuItem, JsonService, ready, messages) {
   
    return declare(null, {
        store: null,
        tree: null,
        rootNodeMenu: new Menu(),
        dsNodeMenu: new Menu(),
        plfNodeMenu: new Menu(),
        plNodeMenu: new Menu(),
        svc: null,
        treeNodeDetailsWidget: null,
        dataSourceTypes: null,
        constructor: function (dataSourcesTreeNode, treeNodeDetailsWidgetId, dataSourceTypes) {
            this.dataSourceTypes = dataSourceTypes;
            this._initSvc();
            this._initTreeStore();
            this._initDataSourceTree(dataSourcesTreeNode);
            this._initMenu();
            var _this = this;
            ready(function () {
                _this.treeNodeDetailsWidget = registry.byId(treeNodeDetailsWidgetId);
            });
        },
        _initTreeStore: function () {
            this.store = new Observable(new JsonRest({
                target: "configTree/",
                getChildren: function (object, onComplete, onError) {
                    switch (object.nodeType) {
                        case "ROOT" :
                            this.get("dataSources").then(onComplete, onError);
                            break;
                        case "DataSource":
                            onComplete([{id: "dataSources/" + object.id + "/pointLocators", name: "pointLocators", nodeType: "RootPointLocatorFolder"}]);
                            break;
                        case "RootPointLocatorFolder":
                            this.get(object.id).then(onComplete, onError);
                            break;
                        case "PointLocatorFolder":
                            this.get("pointLocatorFolders/" + object.folderId + "/pointlocators").then(onComplete, onError);
                            break;
                        default :
                            alert("Unknown Type: " + object.nodeType);
                    }
                },
                mayHaveChildren: function (object) {
                    switch (object.nodeType) {
                        case "ROOT":
                            return true;
                        case "DataSource":
                            return true;
                        case "RootPointLocatorFolder":
                            return true;
                        case "PointLocatorFolder":
                            return true;
                        default:
                            return false;
                    }
                },
                getRoot: function (onItem, onError) {
                    onItem({id: "root",
                        name: "DataSources",
                        nodeType: "ROOT"});
                },
                getLabel: function (object) {
                    switch (object.nodeType) {
                        case "DataSource":
                            var label = "<b>" + object.name + "</b>";
                            return label;
                        default:
                            return object.name;
                    }
                },
                //inserted manually to catch the aspect of Tree to get this working -- Observable looks wired to me ... at least JSONRest does not woirk out of the box ...
                onChange: function (/*dojo/data/Item*/ /*===== item =====*/) {
                },
                onChildrenChange: function (/*===== parent, newChildrenList =====*/) {
                },
                onDelete: function (/*dojo/data/Item*/ /*===== item =====*/) {
                }

            }));

        },
        _initDataSourceTree: function (dataSourcesTreeNode) {
            var MyTreeNode = declare(Tree._TreeNode, {
                _setLabelAttr: {node: "labelNode", type: "innerHTML"}
            });

            this.tree = new Tree({
                model: this.store,
                rootNodeMenu: this.rootNodeMenu,
                dsNodeMenu: this.dsNodeMenu,
                plfNodeMenu: this.plfNodeMenu,
                plNodeMenu: this.plNodeMenu,
                _createTreeNode: function (args) {
                    var result = new MyTreeNode(args);
                    switch (args.item.nodeType) {
                        case "ROOT":
                            this.rootNodeMenu.bindDomNode(result.domNode);
                            break;
                        case "DataSource":
                            this.dsNodeMenu.bindDomNode(result.domNode);
                            break;
                        case "PointLocatorFolder":
                            this.plfNodeMenu.bindDomNode(result.domNode);
                            break;
                        case "PointLocator":
                            this.plNodeMenu.bindDomNode(result.domNode);
                            break;
                        default:
                    }
                    return result;
                },
                getIconClass: function (item, opened) {
                    switch (item.nodeType) {
                        case "ROOT":
                            return "dsIcon";
                        case "DataSource":
                            return item.enabled ? "dsRunningIcon" : "dsStoppedIcon";
                        case "PointLocatorFolder":
                            return "plIcon";
                        case "PointLocator":
                            return item.enabled ? "plRunningIcon" : "plStoppedIcon";
                        default:
                            return (!item || this.model.mayHaveChildren(item)) ? (opened ? "dijitFolderOpened" : "dijitFolderClosed") : "dijitLeaf"
                    }
                },
                detailController: this,
                onClick: function (node) {
                    switch (node.nodeType) {
                        case "DataSource":
                            this.detailController.treeNodeDetailsWidget.set("href", "dataSources/dataSource?id=" + node.id);
                            break;
                        case "PointLocator":
                            this.detailController.treeNodeDetailsWidget.set("href", "dataSources/pointLocator?id=" + node.plId);
                            break;
                        default:
                            this.detailController.treeNodeDetailsWidget.set("content", null);
                    }
                }
            }, dataSourcesTreeNode);
        },
        _initMenu: function () {
            this.treeMenu = new Menu({
                targetNodeIds: []
            });
            var _store = this.store;
            var _tree = this.tree;
            var _svc = this.svc;
            this.dsNodeMenu.addChild(new MenuItem({
                iconClass: "dsStartIcon",
                label: messages["dataSource.action.start"],
                onClick: function () {
                    _svc.startDataSource(_tree.lastFocused.item.id).then(function (result) {
                        _store.onChange(result);
                    }, function (error) {
                        alert(error);
                    });
                }
            }));
            this.dsNodeMenu.addChild(new MenuItem({
                iconClass: "dsStopIcon",
                label: messages["dataSource.action.stop"],
                onClick: function () {
                    _svc.stopDataSource(_tree.lastFocused.item.id).then(function (result) {
                        _store.onChange(result);
                    }, function (error) {
                        alert(error);
                    });
                }
            }));
            this.dsNodeMenu.addChild(new MenuItem({
                iconClass: "dsDeleteIcon",
                label: messages["dataSource.action.delete"],
                onClick: function () {
                    _svc.deleteDataSource(_tree.lastFocused.item.id).then(function (result) {
                        if (result) {
                            _store.onDelete(_tree.lastFocused.item);
                        } else {
                            alert("Could not delete Datasource", _tree.lastFocused.item);
                        }
                    }, function (error) {
                        alert(error);
                    });
                }
            }));
            var dsAddMenu = new Menu({});
            for (var i = 0; i < this.dataSourceTypes.length; i++) {
                dsAddMenu.addChild(new MenuItem({
                    iconClass: "dsAddIcon",
                    label: this.dataSourceTypes[i].label,
                    _dsKey: this.dataSourceTypes[i].key,
                    onClick: function () {
                        _svc.addDataSource(this._dsKey).then(function (result) {
                            _store.getChildren(_tree.lastFocused.item, function (children) {
                                _store.onChildrenChange(_tree.lastFocused.item, children);
                            }, function (error) {
                                alert(error);
                            });
                        }, function (error) {
                            alert(error);
                        });
                    }
                })
                        );
            }
            this.rootNodeMenu.addChild(new PopupMenuItem({
                iconClass: "dsAddIcon",
                label: messages["dataSource.action.add"],
                popup: dsAddMenu
            }));
            this.plfNodeMenu.addChild(new MenuItem({
                iconClass: "dijitIconAdd",
                label: messages["pointLocator.action.add"],
                onClick: function () {
                    _svc.addPointLocator(_tree.lastFocused.item.dsId, _tree.lastFocused.folderId).then(function (result) {
                        _store.getChildren(_tree.lastFocused.item, function (children) {
                            _store.onChildrenChange(_tree.lastFocused.item, children);
                        }, function (error) {
                            alert(error);
                        });
                    }, function (error) {
                        alert(error);
                    });
                }
            }));
            this.treeMenu.addChild(new MenuItem({
                label: messages["dataSource.action.delete"],
                disabled: true
            }));
            this.plNodeMenu.addChild(new MenuItem({
                iconClass: "plStartIcon",
                label: messages["pointLocator.action.start"],
                onClick: function () {
                    _svc.startPointLocator(_tree.lastFocused.item.plId).then(function (result) {
                        _store.onChange(result);
                    }, function (error) {
                        alert(error);
                    });
                }
            }));
            this.plNodeMenu.addChild(new MenuItem({
                iconClass: "plStopIcon",
                label: messages["pointLocator.action.stop"],
                onClick: function () {
                    _svc.stopPointLocator(_tree.lastFocused.item.plId).then(function (result) {
                        _store.onChange(result);
                    }, function (error) {
                        alert(error);
                    });
                }
            }));
            this.rootNodeMenu.startup();
            this.dsNodeMenu.startup();
            this.plfNodeMenu.startup();
            this.plNodeMenu.startup();
        },
        _initSvc: function () {
            this.svc = new JsonService({
                serviceUrl: 'dataSources/rpc/', // Adress of the RPC service end point
                timeout: 1000,
                strictArgChecks: true,
                methods: [{
                        name: 'addDataSource',
                        parameters: [
                            {
                                name: 'type',
                                type: 'STRING'
                            }
                        ]
                    },
                    {
                        name: 'deleteDataSource',
                        parameters: [{
                                name: 'id',
                                type: 'INT'
                            }]
                    },
                    {
                        name: 'addPointLocator',
                        parameters: [
                            {
                                name: 'dataSourceId',
                                type: 'INT'
                            },
                            {
                                name: 'parentFolderId',
                                type: 'INT'
                            }
                        ]
                    },
                    {
                        name: 'startDataSource',
                        parameters: [{
                                name: 'id',
                                type: 'INT'
                            }]
                    },
                    {
                        name: 'stopDataSource',
                        parameters: [{
                                name: 'id',
                                type: 'INT'
                            }]
                    },
                    {
                        name: 'startPointLocator',
                        parameters: [{
                                name: 'id',
                                type: 'INT'
                            }]
                    },
                    {
                        name: 'stopPointLocator',
                        parameters: [{
                                name: 'id',
                                type: 'INT'
                            }]
                    }

                ]
            });
        }
    });
});