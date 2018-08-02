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
<%@page import="com.serotonin.mango.Common"%>

<tr>
  <td class="formLabelRequired"><fmt:message key="dsEdit.serial.port"/></td>
  <td class="formField">
    <c:choose>
      <c:when test="${!empty commPortError}">
        <input id="commPortId" type="hidden" value=""/>
        <span class="formError">${commPortError}</span>
      </c:when>
      <c:otherwise>
          <sbt:select id="commPortId" value="${serialPortSettings != null ? serialPortSettings.commPortId : ''}">
          <c:forEach items="${commPorts}" var="port">
            <sbt:option value="${port.name}">${port.name}</sbt:option>
          </c:forEach>
        </sbt:select>
      </c:otherwise>
    </c:choose>
  </td>
</tr>

<tr>
  <td class="formLabelRequired"><fmt:message key="dsEdit.serial.baud"/></td>
  <td class="formField">
    <sbt:select id="baudRate" value="${serialPortSettings != null ? serialPortSettings.baudRate : rxtxDefaultBaudrate}">
      <sbt:option>110</sbt:option>
      <sbt:option>300</sbt:option>
      <sbt:option>1200</sbt:option>
      <sbt:option>2400</sbt:option>
      <sbt:option>4800</sbt:option>
      <sbt:option>9600</sbt:option>
      <sbt:option>19200</sbt:option>
      <sbt:option>38400</sbt:option>
      <sbt:option>57600</sbt:option>
      <sbt:option>115200</sbt:option>
      <sbt:option>230400</sbt:option>
      <sbt:option>460800</sbt:option>
      <sbt:option>921600</sbt:option>
    </sbt:select>
  </td>
</tr>

<tr>
  <td class="formLabelRequired"><fmt:message key="dsEdit.serial.flowControlIn"/></td>
  <td class="formField">
    <sbt:select id="flowControlIn" value="${serialPortSettings != null ? serialPortSettings.flowControlIn : rxtxDefaultFlowControl}">
      <sbt:option value="NONE"><fmt:message key="dsEdit.serial.flow.none"/></sbt:option>
      <sbt:option value="RTS|CTS"><fmt:message key="dsEdit.serial.flow.rtsCts"/></sbt:option>
      <sbt:option value="XON|XOFF"><fmt:message key="dsEdit.serial.flow.xonXoff"/></sbt:option>
    </sbt:select>
  </td>
</tr>

<tr>
  <td class="formLabelRequired"><fmt:message key="dsEdit.serial.flowOut"/></td>
  <td class="formField">
    <sbt:select id="flowControlOut" value="${serialPortSettings != null ? serialPortSettings.flowControlOut : rxtxDefaultFlowControl}">
      <sbt:option value="NONE"><fmt:message key="dsEdit.serial.flow.none"/></sbt:option>
      <sbt:option value="RTS|CTS"><fmt:message key="dsEdit.serial.flow.rtsCts"/></sbt:option>
      <sbt:option value="XON|XOFF"><fmt:message key="dsEdit.serial.flow.xonXoff"/></sbt:option>
    </sbt:select>
  </td>
</tr>

<tr>
  <td class="formLabelRequired"><fmt:message key="dsEdit.serial.dataBits"/></td>
  <td class="formField">
    <sbt:select id="dataBits" value="${serialPortSettings != null ? serialPortSettings.dataBits : rxtxDefaultDataBits}">
      <sbt:option value="5">5</sbt:option>
      <sbt:option value="6">6</sbt:option>
      <sbt:option value="7">7</sbt:option>
      <sbt:option value="8">8</sbt:option>
    </sbt:select>
  </td>
</tr>

<tr>
  <td class="formLabelRequired"><fmt:message key="dsEdit.serial.stopBits"/></td>
  <td class="formField">
    <sbt:select id="stopBits" value="${serialPortSettings != null ? serialPortSettings.stopBits : rxtxDefaultStopBits}">
      <sbt:option value="1">1</sbt:option>
      <sbt:option value="1.5">1.5</sbt:option>
      <sbt:option value="2">2</sbt:option>
    </sbt:select>
  </td>
</tr>

<tr>
  <td class="formLabelRequired"><fmt:message key="dsEdit.serial.parity"/></td>
  <td class="formField">
    <sbt:select id="parity" value="${serialPortSettings != null ? serialPortSettings.parity : rxtxDefaultParity}">
      <sbt:option value="NONE"><fmt:message key="dsEdit.serial.parity.none"/></sbt:option>
      <sbt:option value="ODD"><fmt:message key="dsEdit.serial.parity.odd"/></sbt:option>
      <sbt:option value="EVEN"><fmt:message key="dsEdit.serial.parity.even"/></sbt:option>
      <sbt:option value="MARK"><fmt:message key="dsEdit.serial.parity.mark"/></sbt:option>
      <sbt:option value="SPACE"><fmt:message key="dsEdit.serial.parity.space"/></sbt:option>
    </sbt:select>
  </td>
</tr>

<tr>
    <td class="formLabelRequired" for="inputBufferSize" ><fmt:message key="dsEdit.serial.inputBufferSize"/></td>
    <td><input class="formShort" type="text" id="inputBufferSize" value="${serialPortSettings != null ? serialPortSettings.inputBufferSize : rxtxDefaultInputBufferSize}" /></td>
</tr>

<tr>
    <td class="formLabelRequired" for="outputBufferSize" ><fmt:message key="dsEdit.serial.outputBufferSize"/></td>
    <td><input class="formShort" type="text" id="outputBufferSize" value="${serialPortSettings != null ? serialPortSettings.outputBufferSize : rxtxDefaultOutputBufferSize}" /></td>
</tr>
