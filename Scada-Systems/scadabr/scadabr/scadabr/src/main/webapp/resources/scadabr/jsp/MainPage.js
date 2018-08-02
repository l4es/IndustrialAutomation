define(["dojo/_base/declare",
    "dojo/rpc/JsonService"
], function (declare, JsonService) {

    window.onerror = function mangoHandler(desc, page, line) {
        alert(page + " Line: " + line + "<br/>" + desc);
        // We do not handle this error, Just log...
        return false;
    };


    /*TODO jsoin jsonrpc to push data to the server
     window.onerror = function mangoHandler(desc, page, line)  {
     MiscDwr.jsError(desc, page, line, BrowserDetect.browser, BrowserDetect.version, BrowserDetect.OS,
     window.location.href);
     return false;
     };
     */

    return declare(null, {
        constructor: function () {
            this._initUserSvc();
        },
        _initUserSvc: function () {
            this.svc = new JsonService({
                serviceUrl: 'rpc/users', // Adress of the RPC service end point
                timeout: 10000,
                strictArgChecks: true,
                methods: [{
                        name: 'setHomeUrl',
                        parameters: [
                            {
                                name: 'homeUrl',
                                type: 'STRING'
                            }
                        ]
                    }
                ]
            });
        },
        setHomeUrl: function (homeUrl) {
            return this.svc.setHomeUrl(homeUrl);
        },
        showHelp: function (helpId) {
            require(["dojox/widget/Dialog"], function (Dialog) {
                var params = {
                    widgetId: "HelpDialogId",
                    showTitle: true,
                    draggable: true,
                    title: "<i>LocalizeMe Help</i>",
                    dimensions: [600, 420], // sizeToViewport:"true", viewportPadding:"200",
                    href: "help?id=" + helpId};
                var myDialog = new Dialog(params);
                myDialog.show();
            });
        }

    });
})