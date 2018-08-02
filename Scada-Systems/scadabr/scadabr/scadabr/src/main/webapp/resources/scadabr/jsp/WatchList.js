define(["dojo/_base/declare",
    "dojo/_base/lang",
    "dijit/Tree",
    "dijit/form/Button",
    "dojo/store/JsonRest",
    "dojo/dnd/Source",
    "dgrid/OnDemandGrid",
    "dgrid/Keyboard",
    "dgrid/Selection",
    "dojo/request",
    "dojo/store/Memory",
    "dojo/store/Observable",
    "dijit/tree/dndSource",
    "dgrid/extensions/DnD",
    "dojo/rpc/JsonService",
    "dojo/_base/fx",
    "dojox/timing",
    "scadabr/DataPointChart",
    "dijit/Dialog",
    "dojo/date",
    "dojo/date/locale"

], function (declare, lang, Tree, Button, JsonRest, DnDSource, OnDemandGrid, Keyboard, Selection, request, Memory, Observable, dndSource, DnD, JsonService, fx, timing, DataPointChart, Dialog, date, dateLocale) {

    return declare(null, {
        pointsTreeStore: null,
        pointsTree: null,
        watchlistGrid: null,
        watchlistId: null,
        constructor: function (pointsTreeNode, watchListNode, watchlistId) {
            this.watchlistId = watchlistId;
            this._initSvc();
            this._initPointsTree(pointsTreeNode);
            this._initWatchListTable(watchListNode, watchlistId);
            //,    "dojox/timing/Timer"
            //       dojo.require('dojox.timing');
            t = new timing.Timer(1000);

            t.onTick = lang.hitch(this, function () {
                this.fetchWatchList(this.watchlistId);
            })
            t.start();
        },
        _initSvc: function () {
            this.svc = new JsonService({
                serviceUrl: 'rpc/watchlists.json', // Adress of the RPC service end point
                timeout: 1000,
                strictArgChecks: true,
                methods: [{
                        name: 'addPointToWatchlist',
                        parameters: [
                            {
                                name: 'watchListId',
                                type: 'INTEGER'
                            },
                            {
                                name: 'index',
                                type: 'INTEGER'
                            },
                            {
                                name: 'dataPointId',
                                type: 'INTEGER'
                            }
                        ]
                    },
                    {
                        name: 'deletePointFromWatchlist',
                        parameters: [
                            {
                                name: 'watchListId',
                                type: 'INTEGER'
                            },
                            {
                                name: 'dataPointId',
                                type: 'INTEGER'
                            }
                        ]
                    },
                    {
                        name: 'getChartDataSet',
                        parameters: [
                            {
                                name: 'dataPointId',
                                type: 'INTEGER'
                            }
                        ]
                    }

                ]
            });
        },
        _initPointsTree: function (pointsTreeNode) {
            this.pointsTreeStore = new JsonRest({
                target: "rest/pointHierarchy/",
                getChildren: function (object, onComplete, onError) {
                    this.query({parentId: object.id}).then(onComplete, onError);
                },
                mayHaveChildren: function (object) {
                    return object.nodeType === "PF";
                },
                getRoot: function (onItem, onError) {
                    this.get("root").then(onItem, onError);
                },
                getLabel: function (object) {
                    return object.name;
                }
            });
            // Create the Tree.
            this.pointsTree = new Tree({
                model: this.pointsTreeStore,
                dndController: dndSource
            }, pointsTreeNode);
        },
        _initWatchListTable: function (watchListNode, watchlistId) {
            this.watchlistGrid = new (declare([OnDemandGrid, Keyboard, Selection, DnD]))({
                sort: "order",
                showHeader: false,
                store: new Observable(new Memory({data: null})),
                columns: {
                    chartType: {
                        label: "ChartType",
                        renderCell: lang.hitch(this, function (point, chartType, default_node, options) {
                            var myIconClass;
                            var myLabel;
                            myIconClass = 'scadaBrDeleteIcon';
                            myLabel = "Show Chart";

                            var btnAck = new Button({
                                myObj: this,
                                pointId: point.id,
                                showLabel: false,
                                iconClass: myIconClass,
                                label: myLabel,
                                onClick: function () {
                                    this.myObj.svc.getChartDataSet(this.pointId).then(function (result) {
                                        var d = new Dialog();
                                        var div = document.createElement("div");
                                        var c = new DataPointChart(result, div)
                                        d.setContent(div);
                                        d.show();
                                    });
                                }
                            }, default_node.appendChild(document.createElement("div")));
                            btnAck._destroyOnRemove = true;

                        })
                    },
                    settable: {
                        label: "Settable"
                    },
                    canonicalName: {
                        label: "Name"
                    },
                    value: {
                        anim: null,
                        lable: "Value",
                        renderCell: lang.hitch(this, function (point, value, default_node, options) {
                            this.anim = fx.animateProperty({
                                node: default_node, duration: 5000,
                                properties: {
                                    backgroundColor: {start: "gray", end: "white"}
                                }
                            });
                            default_node.innerHTML = value;
                            if (point.changed) {
                                this.anim.play();
                            }
                        })
                    },
                    timestamp: {
                        anim: null,
                        lable: "Timestamp",
                        renderCell: lang.hitch(this, function (point, value, default_node, options) {
                            this.anim = fx.animateProperty({
                                node: default_node, duration: 5000,
                                properties: {
                                    backgroundColor: {start: "gray", end: "white"}
                                }
                            });
                            var now = new Date();
                            var ts = new Date();
                            ts.setTime(point.timestamp);
                            if (date.compare(now, ts, "date") === 0) {
                                default_node.innerHTML = dateLocale.format(ts, {selector: "time", formatLength: "medium"});
                            } else {
                                default_node.innerHTML = dateLocale.format(ts, {selector: "datetime", formatLength: "medium"});
                            }
                            if (point.changed) {
                                this.anim.play();
                            }
                        })
                    },
                    id: {
                        label: '',
                        renderCell: lang.hitch(this, function (point, pointId, default_node, options) {
                            var myIconClass;
                            var myLabel;
                            myIconClass = 'scadaBrDeleteIcon';
                            myLabel = "Delete";

                            var btnAck = new Button({
                                myObj: this,
                                pointId: pointId,
                                showLabel: false,
                                iconClass: myIconClass,
                                label: myLabel,
                                onClick: function () {
                                    console.log("BTN Delete THIS: ", this);
                                    this.myObj.deletePointFromWatchlist(this.myObj.watchlistId, this.pointId);
                                }
                            }, default_node.appendChild(document.createElement("div")));
                            btnAck._destroyOnRemove = true;

                        })
                    }
                },
                dndParams: {
                    allowNested: true, // also pick up indirect children w/ dojoDndItem class
                    self: this,
                    checkAcceptance: function (source, nodes) {
                        console.log("DND EXT DROP " + nodes);
                        return true;
                    },
                    onDropInternal: function (source, nodes, copy, target) {

                    },
                    onDropExternal: function (source, nodes, copy, target) {
                        var grid = this.grid;
                        nodes.forEach(function (node) {
                            var i = source.getItem(node.id);
                            var d = i.data;
                            var a = i.type.indexOf("treeNode");

                            if (i.type.indexOf("treeNode") >= 0) {
                                var idx = 0;
                                if (target === undefined) {
                                    //dropped after last row    
                                    this.self.addPointToWatchlist(this.self.watchlistId, grid.store.data.length, d.item.id);
                                } else if (target === null) {
                                    //dropped not onto a row, so add it at the end
                                    this.self.addPointToWatchlist(this.self.watchlistId, grid.store.data.length, d.item.id);
                                } else {
                                    grid.store.data.forEach(function () {
                                        if (grid.store.data[idx].id === target.id) {
                                            this.self.addPointToWatchlist(this.self.watchlistId, idx, d.item.id);
                                            console.log("Dropped TreeNode" + d.item.name);
                                        }
                                        idx++;
                                    }, this);
                                }
                            } else if (i.type.indexOf("dgrid-row") >= 0) {
                                console.log("Dropped dgrid col" + d.canonicalName);

                            }
                        }, this);
                    },
                    loadingMessage: "Loading data...",
                    noDataMessage: "No results found.",
                    selectionMode: "single" // for Selection; only select a single row at a time
                            //cellNavigation: false, // for Keyboard; allow only row-level keyboard navigation
                }
            }, watchListNode);
            //    var wlTarget = new Target(watchListNode, { accept: true });
            //get initial list...
            this.fetchWatchList(watchlistId);
        },
        fetchWatchList: function (watchlistId) {
            request("rest/watchLists/", {
                query: {id: watchlistId},
                handleAs: "json"
            }).then(lang.hitch(this, function (data) {
                console.log("WL DATA: " + data);
                this.watchlistGrid.store.setData(data.points);
                this.watchlistGrid.refresh();
            }), function (err) {
                console.log("WL ERR: " + err);
            }, function (evt) {
                console.log("WL EVT: " + evt);
            });
        },
        addPointToWatchlist: function (watchlistId, index, dataPointId) {
            var grid = this.watchlistGrid;
            this.svc.addPointToWatchlist(watchlistId, index, dataPointId).then(function (result) {
                console.log("DataPoint Added CB: ", result);
                grid.store.setData(result.points);
                grid.refresh();
            });

        },
        deletePointFromWatchlist: function (watchlistId, dataPointId) {
            var grid = this.watchlistGrid;
            this.svc.deletePointFromWatchlist(watchlistId, dataPointId).then(function (result) {
                console.log("DataPoint deleted CB: ", result);
                grid.store.setData(result.points);
                grid.refresh();
            });

        }
    });
});