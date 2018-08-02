<%@ taglib prefix="dijit" uri="/WEB-INF/tld/dijit.tld" %>
<%@ taglib prefix="dojox" uri="/WEB-INF/tld/dojox.tld" %>
<link rel="stylesheet" href="resources/dojox/widget/ColorPicker/ColorPicker.css" />
<script>
    require([
        "dojo/dom",
        "dojo/parser",
        "scadabr/AjaxFormPost"
    ], function (dom, parser, AjaxFormPost) {
        var _dpFormParseWrapper = dom.byId("dpFormParseWrapperId");
        try {
            parser.parse(_dpFormParseWrapper).then(function () {
                var _DpFormPost = new AjaxFormPost("dpFormId", "REST/" + ${dataPoint.id});
            }, function (err) {
                alert(err);
            });
        } catch (error) {
            alert(error);
        }
    });
</script>
<div id="dpFormParseWrapperId">
        <form id="dpFormId">
        <dojox:tableContainer cols="1">
            <dijit:validationTextBox i18nLabel="pointEdit.props.name" name="name"  value="${dataPoint.name}" />
            <dijit:validationTextBox i18nLabel="common.xid" name="loggingType"  value="${dataPoint.loggingType}"/>
            <dijit:validationTextBox i18nLabel="common.xid" name="valuePattern"  value="${dataPoint.valuePattern}"/>
            <dijit:validationTextBox i18nLabel="common.xid" name="valueAndUnitPattern"  value="${dataPoint.valueAndUnitPattern}"/>
            <dijit:validationTextBox i18nLabel="common.xid" name="unit"  value="${dataPoint.unit}"/>
            <dijit:validationTextBox i18nLabel="common.xid" name="settable"  value="${dataPoint.settable}"/>
            <jsp:doBody />
        </dojox:tableContainer>
        <dijit:button type="submit" iconClass="commonSaveIcon" i18nLabel="common.save" />
    </form>
</div>