<%--
    Mango - Open Source M2M - http://mango.serotoninsoftware.com
    Copyright (C) 2006-2011 Serotonin Software Technologies Inc.
    @author Matthew Lohbihler
    y
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
<%@taglib prefix="tag" tagdir="/WEB-INF/tags" %>
<%@include file="/WEB-INF/jsp/include/tech.jsp" %>
<%@page import="com.serotonin.mango.vo.UserComment"%>
<%@page import="com.serotonin.mango.rt.event.type.EventType"%>
<%@page import="br.org.scadabr.vo.event.EventStatus"%>
<tag:page helpId="events">
    <!--%@ include file="/WEB-INF/jsp/include/userComment.jsp" %-->
    <style>
        .incrementControl { width: 2em; }
        .dgrid-column-id {
            width: 10em;
        }
        .dgrid-column-alarmLevel {
            width: 5em;
        }
        .dgrid-column-activeTimestamp {
            width: 10em;
        }
        .dgrid-column-acknowledged {
            width: 10em;
        }
        .dgrid-row-odd {
            background: #F2F5F9;
        }
    </style>

    <script type="text/javascript">
        var events;
        require(["scadabr/jsp/Events",
            "dojo/domReady!"
        ], function (Events) {
            events = new Events("pendingAlarms");
            require(["dojo/ready"], function (ready) {
                ready(function () {
                    events.wireEvents("btnAcknowledgeAll");
                });
            });
        });
    </script>

    <dijit:headlineLayoutContainer>
        <dijit:topContentPane>
            <div class="smallTitle titlePadding" style="float:left;">
                <!--tag:img png="flag_white" title="events.alarms"/-->
                <fmt:message key="events.pending"/>
            </div>

            <div id="ackAllDiv" class="titlePadding" style="float:right;">
                <dijit:button id="btnAcknowledgeAll" iconClass="scadaBrDoActionIcon" i18nLabel="events.acknowledgeAll"/>
                <dijit:button iconClass="scadaBrDoSilenceIcon" i18nLabel="events.silenceAll"/>
            </div>

        </dijit:topContentPane>
        <dijit:centerContentPane id="pendingAlarms">
        </dijit:centerContentPane>
        <dijit:rightContentPane>
            AddMSG
        </dijit:rightContentPane>
        <dijit:bottomContentPane>
            <div class="smallTitle titlePadding"><fmt:message key="events.search"/></div>
            <dijit:form >
                <dojox:tableContainer cols="1" >
                    <%-- dijit:textBox name="eventId" i18nLabel="events.id"/--%>

                    <select id="eventSourceType" data-dojo-type="dijit/form/Select" title="<fmt:message key="events.search.type"/>">
                        <option value="-1"><fmt:message key="common.all"/></option>
                    </select>
                    <!-- TODO use checkboxes for this ???-->
                    <%--select id="eventStatus" data-dojo-type="dijit/form/Select" title="<fmt:message key="common.status"/>">
                        <option value="<c:out value="<%= EventStatus.ANY%>"/>"><fmt:message key="common.all"/></option>
                        <option value="<c:out value="<%= EventStatus.ACTIVE.getName()%>"/>"><fmt:message key="common.active"/></option>
                        <option value="<c:out value="<%= EventStatus.RTN.getName()%>"/>"><fmt:message key="event.rtn.rtn"/></option>
                        <option value="<c:out value="<%= EventStatus.NORTN.getName()%>"/>"><fmt:message key="common.nortn"/></option>
</select--%>>
                        <%--select id="alarmLevel"data-dojo-type="dijit/form/Select" title="<fmt:message key="common.alarmLevel"/>"><tag:alarmLevelOptions allOption="true"/></select--%>
                        <%--dijit:textBox id="keywords" i18nLabel="events.search.keywords"/--%>
                    <dijit:button i18nLabel="events.search.search"/>
                    <span id="searchMessage" class="formError"></span>
                </dojox:tableContainer>
                <div id="searchResults"></div>
            </dijit:form>

        </dijit:bottomContentPane>
    </dijit:headlineLayoutContainer>



    <!--div id="datePickerDiv" style="position:absolute; top:0px; left:0px;" onmouseover="cancelDatePickerExpiry()" onmouseout="expireDatePicker()">
        <div widgetId="datePicker" dojoType="datepicker" dayWidth="narrow" lang="${lang}"></div>
    </div-->

</tag:page>