<%@ include file="/WEB-INF/jsp/include/tech.jsp" %>
<tag:page helpId="dataSources">
    <jsp:body>
        <script type="text/javascript">
            var _dataSources;
                    require([
                            "scadabr/jsp/DataSources",
                            "dojo/domReady!"
                    ], function (DataSources) {
                    var dsTypes = [
            <c:forEach var="dsType" items="${dataSourceTypes}" varStatus="loopStatus">
                        {key:'${dsType.typeKey}',
                         label: '<fmt:message key="${dsType.i18nKey}"/>'}${loopStatus.last ? '' : ','}
            </c:forEach>
                    ];
                    _dataSources = new DataSources("dataSourcesTree", "treeNodeDetailsWidget", dsTypes);
                    });

        </script>

        <dijit:sidebarBorderContainer gutters="true" liveSplitters="true">
            <dijit:leftContentPane splitter="true" >
                <div id="dataSourcesTree"></div>
            </dijit:leftContentPane>
            <dojox:centerContentPane id="treeNodeDetailsWidget"/>
        </dijit:sidebarBorderContainer>
    </jsp:body>
</tag:page>