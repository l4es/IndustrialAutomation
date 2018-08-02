define(["dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/on",
    "dijit/registry",
    "dojo/dom-construct",
    "dojo/request",
    "dojo/store/Memory",
    "dgrid/Grid",
    "dgrid/extensions/Pagination",
    "dgrid/Keyboard",
    "dgrid/Selection",
    "dijit/form/Button",
    "dojo/rpc/JsonService",
    "dojo/date",
    "dojo/date/locale",
    "dojo/i18n!scadabr/nls/messages"
], function (declare, lang, on, registry, domConstruct, request, Memory, Grid, Pagination, Keyboard, Selection, Button, JsonService, date, dateLocale, messages) {

    return declare("scadabr/desktop/EventsView", null, {
        constructor: function (pendingAlarmsTableNode) {
            this._initSvc();
            this._initPendingAlarmsTable(pendingAlarmsTableNode);
        },
        _initSvc: function () {
            this.svc = new JsonService({
                serviceUrl: 'rpc/events/', // Adress of the RPC service end point
                timeout: 1000,
                strictArgChecks: true,
                methods: [{
                        name: 'acknowledgePendingEvent',
                        parameters: [
                            {
                                name: 'id',
                                type: 'INTEGER'
                            }
                        ]
                    },
                    {
                        name: 'acknowledgeAllPendingEvents',
                        parameters: []
                    }

                ]
            });
        },
        _initPendingAlarmsTable: function (pendingAlarmsTableNode) {
            var _formatTimeStamp = function (timestamp) {
                var now = new Date();
                var ts = new Date();
                ts.setTime(timestamp);
                if (date.compare(now, ts, "date") === 0) {
                    return dateLocale.format(ts, {selector: "time", formatLength: "medium"});
                } else {
                    return dateLocale.format(ts, {selector: "datetime", formatLength: "medium"});
                }

            };
            this.grid = new (declare([Grid, Pagination, Keyboard, Selection]))({
                store: new Memory(),
                columns: {
                    id: {
                        label: messages["events.id"]
                    },
                    alarmLevel: {
                        label: messages["common.alarmLevel"],
                        renderCell: function (event, alarmLevel, default_node, options) {
                            var node = domConstruct.create("img");
                            var imgName;
                            switch (alarmLevel) {
                                case "INFORMATION":
                                    imgName = 'flag_blue';
                                    if (event.active) {
                                        node.alt = messages["common.alarmLevel.info"];
                                    } else {
                                        node.alt = messages["common.alarmLevel.info.rtn"];
                                    }
                                    break;
                                case  "URGENT":
                                    imgName = 'flag_yellow';
                                    if (event.active) {
                                        node.alt = messages["common.alarmLevel.urgent"];
                                    } else {
                                        node.alt = messages["common.alarmLevel.urgent_rtn"];
                                    }
                                    break;
                                case  "CRITICAL":
                                    if (event.active) {
                                        node.alt = messages["common.alarmLevel.critical"];
                                    } else {
                                        node.alt = messages["common.alarmLevel.critical.rtn"];
                                    }
                                    imgName = 'flag_orange';
                                    break;
                                case  "LIVE_SAVETY":
                                    if (event.active) {
                                        node.alt = messages["common.alarmLevel.lifeSafety"];
                                    } else {
                                        node.alt = messages["common.alarmLevel.lifeSafety.rtn"];
                                    }
                                    imgName = 'flag_red';
                                    break;
                                default :
                                    node.alt = alarmLevel;
                                    return  node;
                            }
                            node.src = 'images/' + imgName + (event.active ? '' : '_off') + '.png';
                            node.title = node.alt;
                            return node;
                        }
                    },
                    fireTimestamp: {
                        label: messages["common.time"],
                        resizable: true,
                        formatter: function (timestamp) {
                            return _formatTimeStamp(timestamp);
                        }
                    },
                    message: {
                        label: "Message",
                        resizable: true,
                        formatter: function (msg) {
                            return msg;
                        }
                    },
                    inactiveTimestamp: {
                        label: messages["common.inactiveTime"],
                        renderCell: function (event, timestamp, default_node, options) {
                            var node = domConstruct.create("div");
                            switch (event.eventState) {
                                case "ACTIVE":
                                    node.innerHTML = messages["common.active"];
                                    var img = domConstruct.create("img", null, node);
                                    img.src = "images/flag_white.png";
                                    img.title = messages["common.active"];
                                    break;
                                case "GONE":
                                    node.innerHTML = _formatTimeStamp(timestamp) + ' - ' + event.stateMessage;
                                    break;
                                case "SOURCE_DISABLED":
                                    node.innerHTML = _formatTimeStamp(timestamp) + ' - ' + event.stateMessage;
                                    break;
                                case "STATELESS":
                                    node.innerHTML = event.stateMessage;
                                    break;
                                default:
                                    console.log("eventstate unknown " + event);
                            }
                            return node;
                        }
                    },
                    acknowledged: {
                        label: '',
                        renderCell: lang.hitch(this, function (event, acknowledged, default_node, options) {
                            var myIconClass;
                            var myLabel;
                            if (acknowledged) {
                                myIconClass = 'scadaBrCantDoActionIcon';
                                myLabel = messages["events.acknowledged"];
                            } else {
                                myIconClass = 'scadaBrDoActionIcon';
                                myLabel = messages["events.acknowledge"];
                            }

                            var btnAck = new Button({
                                myObj: this,
                                eventId: event.id,
                                showLabel: false,
                                iconClass: myIconClass,
                                label: myLabel,
                                onClick: function () {
                                    console.log("BTN ACK THIS: ", this);
                                    this.myObj.svc.acknowledgePendingEvent(this.eventId).then(lang.hitch(this.myObj, function (result) {
                                        console.log("BTN ACK CB: ", this);
                                        this.grid.store.setData(result);
                                        this.grid.refresh();
                                    }));
                                }
                            }, default_node.appendChild(document.createElement("div")));
                            btnAck._destroyOnRemove = true;
                            var btnSilence = new Button({
                                eventId: event.id,
                                label: "Sil",
                                iconClass: 'scadaBrDoActionIcon',
                                showLabel: true
                            }, default_node.appendChild(document.createElement("div")));
                            btnSilence._destroyOnRemove = true;
                        })
                    }
                },
                loadingMessage: "Loading data...",
                noDataMessage: "No results found.",
                selectionMode: "single", // for Selection; only select a single row at a time
                //cellNavigation: false, // for Keyboard; allow only row-level keyboard navigation
                pagingLinks: 1,
                pagingTextBox: true,
                firstLastArrows: true,
                pageSizeOptions: [10, 25, 50, 100]
            }, pendingAlarmsTableNode);
            request("rest/events/", {
                handleAs: "json"
            }).then(lang.hitch(this, function (response) {
                this.grid.store.setData(response);
                this.grid.refresh();
            }));
        },
        wireEvents: function (btnAckAll) {
            on(registry.byId(btnAckAll).domNode, "click", lang.hitch(this, function () {
                this.svc.acknowledgeAllPendingEvents().then(lang.hitch(this, function (result) {
                    this.grid.store.setData(result);
                    this.grid.refresh();
                }));
            }));
            this.grid.on("dgrid-error", function (event) {
                console.log(event.error.message);
            });
        }
    });
});