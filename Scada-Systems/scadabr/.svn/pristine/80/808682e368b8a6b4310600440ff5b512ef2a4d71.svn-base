<%--
    Mango - Open Source M2M - http://mango.serotoninsoftware.com
    Copyright (C) 2006-2011 Serotonin Software Technologies Inc.
    @author Matthew Lohbihler
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see http://www.gnu.org/licenses/.
--%>
<%@ include file="/WEB-INF/jsp/include/tech.jsp" %>
<tag:page helpId="dataPoints">
    <jsp:body>
        <style>

            #pointTreeContentPane{
                width: 250px; 
            }

        </style>
        <script type="text/javascript">

            require([
                "scadabr/jsp/DataPoints",
                "dojo/domReady!"
            ], function (DataPoints) {
                var dataTypes = [
            <c:forEach var="dataType" items="${dataTypes}" varStatus="loopStatus">
                {key:'${dataType.name}',
                        label: '<fmt:message key="${dataType.i18nKey}"/>'}${loopStatus.last ? '' : ','}
            </c:forEach>
                ];
                        var _dataPoints = new DataPoints("dataPointTree", "detailViewTabContainer", dataTypes);
            });

        </script>

        <dijit:sidebarBorderContainer gutters="true" liveSplitters="true" >
            <dijit:leftContentPane id="pointTreeContentPane" splitter="true" >
                <div id="dataPointTree"></div>
            </dijit:leftContentPane>
            <dijit:centerContentPane >
                <div id="detailViewTabContainer" data-dojo-type="dijit/layout/TabContainer" >
                    <div id="dataPointDetailsChartTab" data-dojo-type="dojox/layout/ContentPane" title="Show data (chart)" data-dojo-props="selected:true, parseOnLoad: false, contentUrl: 'dataPointDetails/renderChart'">
                    </div>
                    <div id="dataPointEditTab" data-dojo-type="dojox/layout/ContentPane" title="Edit" data-dojo-props="parseOnLoad: false, contentUrl: 'dataPointDetails/editProperties'">
                    </div>
                    <div id="dataPointDetailsTableTab" data-dojo-type="dojox/layout/ContentPane" title="Show data (table)" data-dojo-props="parseOnLoad: false, contentUrl: 'dataPointDetails/editProperties'">
                    </div>
                    <div id="dataPointDetailsEventsAndNotesTab" data-dojo-type="dojox/layout/ContentPane" title="point Events and Notes" data-dojo-props="parseOnLoad: false, contentUrl: 'dataPointDetails/editProperties'">
                    </div>
                    <div id="dataPointUsagesTab" data-dojo-type="dojox/layout/ContentPane" title="point usage" data-dojo-props="parseOnLoad: false, contentUrl: 'dataPointDetails/editProperties'">
                    </div>
                </div>
            </dijit:centerContentPane>
        </dijit:sidebarBorderContainer>
    </jsp:body>
</tag:page>