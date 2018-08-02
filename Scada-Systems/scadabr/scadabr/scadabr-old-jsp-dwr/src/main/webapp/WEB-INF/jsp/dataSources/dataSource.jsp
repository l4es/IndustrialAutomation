<%@ include file="/WEB-INF/jsp/include/tech.jsp" %>
<script>
    require([
        "dojo/dom",
        "dojo/parser",
        "scadabr/AjaxFormPost"
    ], function (dom, parser, AjaxFormPost) {
        var _dsFormParserWrapper = dom.byId("dsFormParserWrapper");
        parser.parse(_dsFormParserWrapper).then(function () {
            var _DsFormPost = new AjaxFormPost("dsFormId", "dataSources/dataSource?id=" + ${dataSource.id});
        }, function (error) {
            alert(error);
        });
    });
</script>
<div id="dsFormParserWrapper">
    <form id="dsFormId">
        <dojox:tableContainer cols="1">
            <dijit:validationTextBox i18nLabel="pointEdit.props.name" name="name" value="${dataSource.name}"/>
            <dijit:validationTextBox i18nLabel="common.xid"  name="xid" value="${dataSource.xid}"/>
        </dojox:tableContainer>
        <dijit:button type="submit" i18nLabel="common.save" />
    </form>
</div>