define([
    "dojo/_base/declare",
    "dijit/_TemplatedMixin",
    "dijit/layout/ContentPane"
], function (
    declare,
    _TemplatedMixin,
    ContentPane
) {
    return declare("scadabr/desktop/Header", [ContentPane, _TemplatedMixin], {
        templateString: '<div><b><i>ScadaBR @' + window.location.origin + window.location.pathname + '</i></b><img src="images/mangoLogoMed.jpg" alt="Logo" style="float:right;"/><div>',
        
        postCreate: function () {
            this.inherited(arguments);
        }
    });
});