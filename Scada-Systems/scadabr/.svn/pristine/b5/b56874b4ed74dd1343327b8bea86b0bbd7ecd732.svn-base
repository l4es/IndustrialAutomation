define(["dojo/_base/declare",
    "dojo/dom",
    "dojo/dom-form",
    "dojo/request",
    "dojo/on",
    'dojo/_base/json'
], function (declare, dom, domForm, request, on, json) {

    return declare(null, {
        form: null,
        constructor: function (formId, postHref) {
            var form = dom.byId(formId);
            // Attach the onsubmit event handler of the form

            on(form, "submit", function (evt) {
                // prevent the page from navigating after submit
                evt.stopPropagation();
                evt.preventDefault();
                // Post the data to the server
                request.post(postHref, {
                    query: {},
                    handleAs: "json",
                    data: domForm.toObject(formId)
                            // Wait 10 seconds for a response
                            //timeout: 10000

                }).then(function (response) {
                    if (response.constraintViolations.length !== 0) {
                        // todo find the element and mark that as error
                        alert(json.toJson(response.constraintViolations)); // Todo Quick and dirtx
                    }
                }, function (error) {
                    alert(error);
                });
            });
        }
    });
});