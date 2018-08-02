define(["dojo/_base/declare",
    "dijit/Tree",
    "dojo/request",
    "dojo/json",
    "dijit/registry",
    "dijit/Menu",
    "dijit/MenuItem",
    "dijit/TooltipDialog",
    "dijit/ConfirmDialog",
    "dijit/form/TextBox",
    "dojo/keys",
    "dijit/popup",
    "dijit/PopupMenuItem",
    "dojo/i18n!scadabr/nls/messages"
], function (declare, Tree, request, json, registry, Menu, MenuItem, TooltipDialog, ConfirmDialog, TextBox, keys, popup, PopupMenuItem, messages) {

    return declare(null, {
        tree: null,
        model: null,
        treeMenu: null,
        nodeNameDialog: null,
        dataTypes: null,
        restBaseUrl: "REST/",
        constructor: function (treeNodeId, tabWidgetId, dataTypes) {
            this.dataTypes = dataTypes;
            this.model = new Object({
                ROOT: {id: "ROOT", name: "ROOT", nodeType: "ROOT"},
                dataPoints: this,
                getIdentity: function (object) {
                    return object.id;
                },
                getChildren: function (object, onComplete, onError) {
                    switch (object.nodeType) {
                        case "ROOT":
                            request(this.dataPoints.restBaseUrl + "pointFolders", {
                                handleAs: "json",
                                method: "GET"
                            }).then(onComplete, onError);
                            break;
                        case "POINT_FOLDER":
                            request(this.dataPoints.restBaseUrl + object.id + "/children", {
                                handleAs: "json",
                                method: "GET"
                            }).then(onComplete, onError);
                            break;
                        default:
                            alert("No children for: " + object);
                    }
                },
                mayHaveChildren: function (object) {
                    return object.nodeType === "POINT_FOLDER" || object.id === "ROOT";
                },
                getRoot: function (onItem, onError) {
                    onItem(this.ROOT);
                },
                getLabel: function (object) {
                    return object.name;
                },
                //inserted manually to catch the aspect of Tree to get this working -- Observable looks wired to me ... at least JSONRest does not woirk out of the box ...
                onChange: function (/*dojo/data/Item*/ /*===== item =====*/) {
                },
                onChildrenChange: function (/*===== parent, newChildrenList =====*/) {
                },
                onDelete: function (/*dojo/data/Item*/ /*===== item =====*/) {
                }

            });
            // Create the Tree.
            this.tree = new Tree({
                model: this.model,
                detailController: this,
                showRoot: false,
                onClick: function (node) {
                    switch (node.nodeType) {
                        case "DATA_POINT":
                            this.detailController.setPointId(node.id);
                            break;
                        case "POINT_FOLDER":
                            this.detailController.setFolderId(node.id);
                            break;
                        default:
                            this.detailController.clearDetailViewId();
                    }
                }
            }, treeNodeId);
            this.tree.set('path', ['ROOT']);
            
            this.selectedTab = null;
            this.tabViewWidget = null;
            this.dpId = -1;
            this.pfId = 0;
            this.cleanUpBeforeChanage = function () {
            };
            this.setUpAfterChange = function () {
                if (((this.dpId === -1) && (this.pfId <= 0)) || (this.selectedTab === null)) {
                    return;
                }
                this.selectedTab.set("href", this.selectedTab.contentUrl + "?id=" + (this.dpId !== -1 ? this.dpId : this.pfId)).then(function(succ){alert("Succ" + succ)}, function(err){alert("ERR:" + err);});
            };
            this.setSelectedTab = function (tab) {
                this.cleanUpBeforeChanage();
                this.selectedTab = tab;
                if (this.pfId === -1) {
                    //curently filter out folders
                    this.setUpAfterChange();
                }
            };
            this.setPointId = function (id) {
                this.cleanUpBeforeChanage();
                this.dpId = id;
                this.pfId = -1;
                this.setUpAfterChange();
            };
            this.setFolderId = function (id) {
                this.cleanUpBeforeChanage();
                this.pfId = id;
                this.dpId = -1;
                //curently filter out folders
                this.selectedTab.set(null);
                //  this.setUpAfterChange();
            };
            this.clearDetailViewId = function () {
                this.cleanUpBeforeChanage();
                this.dpId = -1;
                this.pfId = -1;
                this.setUpAfterChange();
            }

            var detailController = this;
            require(["dojo/ready"],
                    function (ready) {
                        ready(function () {
                            detailController.tabViewWidget = registry.byId(tabWidgetId);
                            detailController.setSelectedTab(detailController.tabViewWidget.selectedChildWidget);
                            detailController.tabViewWidget.watch("selectedChildWidget", function (name, oval, nval) {
                                detailController.setSelectedTab(nval);
                            });
                        });
                    });
            this.nodeNameDialog = new TooltipDialog({
                content: new TextBox({
                    dataPoints: this,
                    treeNode: null,
                    setTreeNode: function (treeNode) {
                        this.treeNode = treeNode;
                        this.set('value', this.treeNode.item.name);
                    },
                    onKeyUp: function (event) {
                        if (event.keyCode === keys.ESCAPE) {
                            popup.close(this.nodeNameDialog);
                        } else if (event.keyCode === keys.ENTER) {
                            popup.close(this.nodeNameDialog);
                            var treeNode = this.treeNode;
                            var node = treeNode.item;
                            node.name = this.get('value');
                            var model = this.dataPoints.model;
                            switch (this.treeNode.item.nodeType) {
                                case "POINT_FOLDER" :
                                case "DATA_POINT" :
                                    request(this.dataPoints.restBaseUrl, {
                                        handleAs: "json",
                                        method: "PUT",
                                        headers: {
                                            Accept: "application/json",
                                            "Content-Type": "application/json"
                                        },
                                        data: json.stringify(node)
                                    }).then(function (object) {
                                        model.onChange(object);
                                        treeNode.focus();
                                    }, function (error) {
                                        alert(error);
                                    });
                                    break;
                                default: alert("Can't rename node id: " + node.id);
                            }
                        }
                    }
                }),
                setTreeNode: function (treeNode) {
                    this.content.setTreeNode(treeNode);
                },
                focusInput: function () {
                    this.content.focus();
                },
            });
            this.treeMenu = new Menu({
                tree: this.tree,
                targetNodeIds: [treeNodeId],
                editMenuItem: new MenuItem({
                    iconClass: "dijitIconEdit",
                    label: messages['common.rename'],
                    dataPoints: this,
                    onClick: function () {
                        if (this.dataPoints.tree.lastFocused === null) {
                            return;
                        }
                        this.dataPoints.nodeNameDialog.setTreeNode(this.dataPoints.tree.lastFocused);
                        popup.open({
                            popup: this.dataPoints.nodeNameDialog,
                            around: this.dataPoints.tree.lastFocused.contentNode
                        });
                        this.dataPoints.nodeNameDialog.focusInput();
                    }

                }),
                addFolderMenuItem: new MenuItem({
                    iconClass: "dijitIconAdd",
                    label: messages['common.add'],
                    dataPoints: this,
                    onClick: function () {
                        var selectedItem = this.dataPoints.tree.selectedItem;
                        var url = this.dataPoints.restBaseUrl;
                        var model = this.dataPoints.model;
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
                                switch (selectedItem.nodeType) {
                                    case "ROOT":
                                        break;
                                    case "POINT_FOLDER":
                                        url = url + selectedItem.id + "/children";
                                        break;
                                    default :
                                        alert("Wrong node " + selectedItem);
                                        return;
                                }
                                request(url, {
                                    handleAs: "json",
                                    method: "POST",
                                    headers: {
                                        Accept: "application/json",
                                        "Content-Type": "application/json"
                                    },
                                    data: json.stringify({name: formContents.folderName, nodeType: "POINT_FOLDER", scadaBrType: "POINT_FOLDER"})
                                }).then(function (object) {
                                    model.getChildren(selectedItem, function (children) {
                                        model.onChildrenChange(selectedItem, children);
                                    }, function (error) {
                                        alert(error);
                                    });
                                }, function (error) {
                                    alert(error);
                                });
                            }
                        });
                        addFolderDialog.show();
                    }

                }),
                deleteNodeMenuItem: new MenuItem({
                    iconClass: "dijitIconDelete",
                    label: messages['common.delete'],
                    dataPoints: this,
                    onClick: function () {
                        var selectedItem = this.dataPoints.tree.selectedItem;
                        var url = this.dataPoints.restBaseUrl;
                        var model = this.dataPoints.model;

                        switch (selectedItem.nodeType) {
                            case "POINT_FOLDER":
                            case "DATA_POINT":
                                url = url + selectedItem.id;
                                break;
                            default :
                                alert("Wrong node " + selectedItem);
                                return;
                        }

                        request(url, {
                            handleAs: "json",
                            method: "DELETE",
                            headers: {
                                Accept: "application/json",
                                "Content-Type": "application/json"
                            },
                            data: null
                        }).then(function (object) {
                            model.onDelete(selectedItem);
                        }, function (error) {
                            alert(error);
                        });

                    }
                })
                /*
                onOpen: function () {
                    alert("Inherited");
                    this.inherited.onOpen(arguments);
                    alert("Inherited Done");
                    var parentItems = this.tree.selectedItems;
                    this.editMenuItem.set("disabled", parentItems.length !== 1);
                    this.addFolderMenuItem.set("disabled", parentItems.length !== 1);
                    this.deleteNodeMenuItem.set("disabled", parentItems.length !== 1);
                }
*/
            });
            this.treeMenu.addChild(this.treeMenu.editMenuItem);
            this.treeMenu.addChild(this.treeMenu.addFolderMenuItem);
            this.treeMenu.addChild(this.treeMenu.deleteNodeMenuItem);
            var dpAddMenu = new Menu({});
            for (var i = 0; i < this.dataTypes.length; i++) {
                dpAddMenu.addChild(new MenuItem({
                    dataPoints: this,
                    iconClass: "dsAddIcon",
                    label: this.dataTypes[i].label,
                    _dataType: this.dataTypes[i],
                    onClick: function () {
                        var selectedItem = this.dataPoints.tree.selectedItem;
                        var url = this.dataPoints.restBaseUrl;
                        var model = this.dataPoints.model;
                        var dataType = this._dataType;
                        var addDataPointDialog = new ConfirmDialog({
                            title: "New Data Point Name localize ME!",
                            content: new TextBox({
                                value: this.label,
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
                                switch (selectedItem.nodeType) {
                                    case "ROOT":
                                        break;
                                    case "POINT_FOLDER":
                                        url = url + selectedItem.id + "/children";
                                        break;
                                    default :
                                        alert("Wrong node " + selectedItem);
                                        return;
                                }
                                request(url, {
                                    handleAs: "json",
                                    method: "POST",
                                    headers: {
                                        Accept: "application/json",
                                        "Content-Type": "application/json"
                                    },
                                    data: json.stringify({name: formContents.dataPointName, nodeType: "DATA_POINT", dataType: dataType.key, scadaBrType: ["DATA_POINT", dataType.key].join(".")})
                                }).then(function (object) {
                                    model.getChildren(selectedItem, function (children) {
                                        model.onChildrenChange(selectedItem, children);
                                    }, function (error) {
                                        alert(error);
                                    });
                                }, function (error) {
                                    alert(error);
                                });
                            }
                        });
                        addDataPointDialog.show();

                        /*                        _svc.addDataPoint(_tree.lastFocused.item.id, this._dataType, this.label).then(function (result) {
                         _store.getChildren(_tree.lastFocused.item, function (children) {
                         _store.onChildrenChange(_tree.lastFocused.item, children);
                         }, function (error) {
                         alert(error);
                         });
                         }, function (error) {
                         alert(error);
                         });
                         */
                    }
                }));
            }

            this.treeMenu.addChild(new PopupMenuItem({
                iconClass: "dsAddIcon",
                label: "Add DataPoint",
                popup: dpAddMenu

            }));
            this.treeMenu.addChild(new MenuItem({
                label: "Rename Folder",
                disabled: true

            }));
            this.treeMenu.addChild(new MenuItem({
                label: "Rename DataPoint",
                disabled: true

            }));
            this.treeMenu.startup();
        }

    });
});