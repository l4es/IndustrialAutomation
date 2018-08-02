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
--%><%@include file="/WEB-INF/tags/decl.tagf" %><%--
--%><%@tag body-content="empty" %><%--
--%><%@attribute name="allOption" type="java.lang.Boolean" %><%--
--%><%@attribute name="sst" type="java.lang.Boolean" %><%--
--%><%@tag import="br.org.scadabr.vo.event.AlarmLevel"%><%--
--%><c:choose>
  <c:when test="${sst}">
    <c:if test="${allOption}">
      <sbt:option value="-1"><fmt:message key="common.all"/></sbt:option>
    </c:if>
    <sbt:option value="<%= AlarmLevel.NONE.getName() %>"><fmt:message key="<%= AlarmLevel.NONE.getI18nKey() %>"/></sbt:option>
    <sbt:option value="<%= AlarmLevel.INFORMATION.getName() %>"><fmt:message key="<%= AlarmLevel.INFORMATION.getI18nKey() %>"/></sbt:option>
    <sbt:option value="<%= AlarmLevel.URGENT.getName() %>"><fmt:message key="<%= AlarmLevel.URGENT.getI18nKey() %>"/></sbt:option>
    <sbt:option value="<%= AlarmLevel.CRITICAL.getName() %>"><fmt:message key="<%= AlarmLevel.CRITICAL.getI18nKey() %>"/></sbt:option>
    <sbt:option value="<%= AlarmLevel.LIFE_SAFETY.name() %>"><fmt:message key="<%= AlarmLevel.LIFE_SAFETY.getI18nKey() %>"/></sbt:option>
  </c:when>
  <c:otherwise>
    <c:if test="${allOption}">
      <option value="-1"><fmt:message key="common.all"/></option>
    </c:if>
    <option value="<%= AlarmLevel.NONE.getName() %>"><fmt:message key="<%= AlarmLevel.NONE.getI18nKey() %>"/></option>
    <option value="<%= AlarmLevel.INFORMATION.getName() %>"><fmt:message key="<%= AlarmLevel.INFORMATION.getI18nKey() %>"/></option>
    <option value="<%= AlarmLevel.URGENT.getName() %>"><fmt:message key="<%= AlarmLevel.URGENT.getI18nKey() %>"/></option>
    <option value="<%= AlarmLevel.CRITICAL.getName() %>"><fmt:message key="<%= AlarmLevel.CRITICAL.getI18nKey() %>"/></option>
    <option value="<%= AlarmLevel.LIFE_SAFETY.getName() %>"><fmt:message key="<%= AlarmLevel.LIFE_SAFETY.getI18nKey() %>"/></option>
  </c:otherwise>
</c:choose>
