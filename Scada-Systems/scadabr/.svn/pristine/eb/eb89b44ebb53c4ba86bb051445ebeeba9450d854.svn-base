<%@ taglib prefix="dijit" uri="/WEB-INF/tld/dijit.tld" %>
<%@ taglib prefix="dojox" uri="/WEB-INF/tld/dojox.tld" %>
<script>
    require([
        "dojo/dom",
        "dojo/parser",
        "scadabr/AjaxFormPost"
    ], function (dom, parser, AjaxFormPost) {
        var _plFormParseWrapper = dom.byId("plFormParseWrapperId");
        try {
            parser.parse(_plFormParseWrapper).then(function () {
                var _PlFormPost = new AjaxFormPost("plFormId", "dataSources/pointLocator?id=" + ${pointLocator.id});
            }, function (err) {
                alert(err);
            });
        } catch (error) {
            alert(error);
        }
    });
</script>
<div id="plFormParseWrapperId">
    <form id="plFormId">
        <dojox:tableContainer cols="1">
            <dijit:validationTextBox i18nLabel="pointEdit.props.name" name="name" value="${pointLocator.name}"/>
            <dijit:validationTextBox i18nLabel="common.xid" name="xid"  value="${pointLocator.xid}"/>
            <jsp:doBody />
        </dojox:tableContainer>
        <dijit:button type="submit" i18nLabel="login.loginButton" />
    </form>
</div>