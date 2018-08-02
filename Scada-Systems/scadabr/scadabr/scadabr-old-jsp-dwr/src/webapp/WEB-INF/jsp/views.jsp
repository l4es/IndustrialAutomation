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
<tag:page dwr="ViewDwr" js="view">
  <script type="text/javascript" src="resources/wz_jsgraphics.js"></script>
  <script type="text/javascript">
    <c:if test="${!empty currentView}">
      mango.view.initNormalView();
    </c:if>
        
    function unshare() {
        ViewDwr.deleteViewShare(function() { window.location = 'views.shtm'; });
    }
  </script>
  
  <table class="borderDiv">
    <tr>
      <td class="smallTitle"><fmt:message key="views.title"/> <tag:help id="graphicalViews"/></td>
      <td width="50"></td>
      <td align="right">
        <sbt:select value="${currentView.id}" onchange="window.location='?viewId='+ this.value;">
          <c:forEach items="${views}" var="aView">
            <sbt:option value="${aView.key}">${sbt:escapeLessThan(aView.value)}</sbt:option>
          </c:forEach>
        </sbt:select>
        <c:if test="${!empty currentView}">
          <c:choose>
            <c:when test="${owner}">
              <a href="view_edit.shtm?viewId=${currentView.id}"><tag:img png="icon_view_edit" title="viewEdit.editView"/></a>
            </c:when>
            <c:otherwise>
              <tag:img png="icon_view_delete" title="viewEdit.deleteView" onclick="unshare()"/>
            </c:otherwise>
          </c:choose>
        </c:if>
        <a href="view_edit.shtm"><tag:img png="icon_view_new" title="views.newView"/></a>
      </td>
    </tr>
  </table>
  
  <tag:displayView view="${currentView}" emptyMessageKey="views.noViews"/>
</tag:page>