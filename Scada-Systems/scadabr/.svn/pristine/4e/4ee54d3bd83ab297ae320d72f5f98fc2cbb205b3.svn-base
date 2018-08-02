define(["dojo/_base/declare",
    "scadabr/desktop/TreeModel",
    "scadabr/desktop/NavigationTree",
    "dijit/layout/BorderContainer",
    "dijit/layout/ContentPane",
    "dijit/layout/TabContainer",
    "scadabr/desktop/DataPointProperties",
    "dojo/i18n!scadabr/desktop/nls/messages"
], function (declare, TreeModel, NavigationTree, BorderContainer, ContentPane, TabContainer, DataPointProperties, messages) {

    return declare("scadabr/desktop/DataPointsView", [BorderContainer], {
        gutters: true,
        liveSplitters: true,
        tree: null,
        treeContentPane: null,
        detailView: null,
        model: null,
        baseHref: null,
        dataTypes: [],
        restBaseUrl: "REST/",
        postCreate: function () {
            this.inherited(arguments);

            this.model = new TreeModel(this.restBaseUrl);
            this.tree = new NavigationTree({model: this.model, baseHref: this.baseHref});
//            this.tree.startup();
            this.treeContentPane = new ContentPane({region: 'left',
                splitter: 'true'});
            this.treeContentPane.addChild(this.tree);
            this.addChild(this.treeContentPane);

            this._initDetailViewModel();
        },
        setCurrentId: function (id) {
            this.tree.selectNode(id);
            var self = this;
            this.tree.model.refresh(id).then(function (object) {
                if (object.nodeType.startsWith("DATA_POINT")) {
                    self.addChild(self.detailView);
                    self.dataPointProperties.setDataPoint(object);
                } else {
                    self.removeChild(self.detailView);

                }
            }, function (error) {
                self.removeChild(self.detailView);
            });
            //TODO set DetailView
        },
        _initDetailViewModel: function () {
            this.detailView = new TabContainer({region: 'center'});
            this.detailView.chartDataView = new ContentPane({title: "Show data (chart)", selected: true});
            this.detailView.addChild(this.detailView.chartDataView);
            this.detailView.tableDataView = new ContentPane({title: "Show data (table)"});
            this.detailView.addChild(this.detailView.tableDataView);
            this.detailView.editDataView = new ContentPane({title: "Edit"});
            this.detailView.addChild(this.detailView.editDataView);
            //QUick and dirty!!!
            this.dataPointProperties = new DataPointProperties({});
            this.detailView.editDataView.addChild(this.dataPointProperties);
            this.detailView.eventsDataView = new ContentPane({title: "Point Events and Notes"});
            this.detailView.addChild(this.detailView.eventsDataView);
            this.detailView.usageDataView = new ContentPane({title: "Point Usage"});
            this.detailView.addChild(this.detailView.usageDataView);
            this.addChild(this.detailView);
        }
    });
});