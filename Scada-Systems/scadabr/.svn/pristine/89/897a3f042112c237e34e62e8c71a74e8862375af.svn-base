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
<%@ taglib prefix="tag" tagdir="/WEB-INF/tags" %>
<tag:page helpId="watchList">

    <jsp:body>
        <style>

            #pointTreeContentPane{
                width: 200px; 
            }

            #watchListContainer {
                width: 100%;
                height: 100%;
            }

            .dgrid-cell {
                border: none;
            }
            .dgrid-column-id {
                width: 10em;
            }
            .dgrid-column-chartType {
                width: 5em;
            }
            .dgrid-column-settable {
                width: 5em;
            }

        </style>

        <script type="text/javascript">
            var _watchList;
            require([
                "scadabr/jsp/WatchList",
                "dojo/domReady!"
            ], function (WatchList) {
                _watchList = new WatchList("dataPointTree", "watchListTable", ${selectedWatchList});
            });

        </script>

        <dijit:sidebarBorderContainer gutters="true" liveSplitters="true" >
            <dijit:leftContentPane id="pointTreeContentPane" splitter="true" >
                <div id="dataPointTree"></div>
            </dijit:leftContentPane>
            <dijit:centerContentPane >
                <dijit:headlineLayoutContainer >
                    <dijit:topContentPane >
                        <dijit:select id="watchListSelect" selectedValue="${selectedWatchList}" items="${watchLists}" />
                    </dijit:topContentPane>
                    <dijit:centerContentPane >
                        <div id="watchListTable"></div>
                    </dijit:centerContentPane>
                </dijit:headlineLayoutContainer>
            </dijit:centerContentPane>
        </dijit:sidebarBorderContainer>
    </jsp:body>
</tag:page>
