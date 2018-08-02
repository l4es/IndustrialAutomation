define(["dojo/_base/declare",
    "dojox/layout/TableContainer",
    "dijit/form/ValidationTextBox",
    "dojo/i18n!scadabr/desktop/nls/messages"
], function (declare, TableContainer, ValidationTextBox, messages) {

    return declare("scadabr/desktop/DataPointProperties", [TableContainer], {
        cols: 1,
        nameTextBox: null,
        setDataPoint: function (object) {
            this.nameTextBox.set('value', object.name);
        },
        postCreate: function () {
            this.inherited(arguments);
            this.nameTextBox = new ValidationTextBox({type: "text", label: "Name", name: "name", id: "name", value: "someTestString", required: true});
            this.addChild(this.nameTextBox);
            this.addChild(new ValidationTextBox({type: "text", label: "Phone", name: "phone", id: "phone", value: "someTestString", required: true}));
            this.addChild(new ValidationTextBox({type: "text", label: "Phone1", name: "phone1", id: "phone1", value: "someTestString", required: true}));
            this.addChild(new ValidationTextBox({type: "text", label: "Phone2", name: "phone2", id: "phone2", value: "someTestString", required: true}));
            this.addChild(new ValidationTextBox({type: "text", label: "Phone3", name: "phone3", id: "phone3", value: "someTestString", required: true}));
        },
        setCurrentId: function (id) {
            this.tree.selectNode(id);
            //TODO set DetailView
        }

    });
});