<%--
    Mango - Open Source M2M - http://mango.serotoninsoftware.com
    Copyright (C) 2009 Arne Pl�se.
    @author Arne Pl�se

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
<%@include file="/WEB-INF/jsp/include/tech.jsp" %>
<%@ taglib prefix="tag" tagdir="/WEB-INF/tags" %>
<tag:page>

    <jsp:body>

        <dijit:form action="editDataSource.shtml.htm" method="post">
        <dojox:tableContainer cols="1">
                    <dijit:validationTextBox i18nLabel="dsEdit.head.name" path="dataSource.name"/>
        </dojox:tableContainer>

        <dijit:button type="submit" i18nLabel="common.save" />
    </dijit:form>

    </jsp:body>
</tag:page>
