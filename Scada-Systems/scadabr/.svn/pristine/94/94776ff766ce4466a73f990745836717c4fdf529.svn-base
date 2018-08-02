<%--
    Mango - Open Source M2M - http://mango.serotoninsoftware.com
    Copyright (C) 2006-2009 Serotonin Software Technologies Inc.
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
<%@ include file="/WEB-INF/jsp/include/tech.jsp"%>
<%@page import="br.org.scadabr.vo.dataSource.alpha2.Alpha2PointLocatorVO"%> 
<%@page import="com.i2msolucoes.alpha24j.DeviceLocator.DeviceCodes"%>
<script type="text/javascript">
  var currentChangeType;
  
  function initImpl() {
  }
	
  function saveDataSourceImpl() {
	  DataSourceEditDwr.saveAlpha2DataSource($get("dataSourceName"), $get("dataSourceXid"), $get("updatePeriods"),
              $get("updatePeriodType"),	$get("commPortId"), $get("baudRate"), $get("dataBits"),
				$get("stopBits"), $get("parity"), $get("timeout"), $get("retries"),$get("station"), 
				saveDataSourceCB);
  }
 
  
  function editPointCBImpl(locator) {
	  $set("deviceCode", locator.deviceCodeId);
	  $set("deviceNumber", locator.deviceNumber);
	  $set("accessMode", locator.accessMode);
	  deviceCodeChanged();
  }
  
  function savePointImpl(locator) {
	  
	  locator.deviceCodeId = $get("deviceCode");
      locator.deviceNumber = $get("deviceNumber");
      locator.accessMode = $get("accessMode");
      
      DataSourceEditDwr.saveAlpha2PointLocator(currentPoint.id, $get("xid"), $get("name"), locator, savePointCB);
  }
  
  function deviceCodeChanged() {
	  code = $get("deviceCode");
	  if(code == <%= Alpha2PointLocatorVO.RUN_STOP_CODE %>) {
		  hide("accessModeDiv");
	  } else {
		  show("accessModeDiv");
	  }
  }
</script>

<c:set var="dsDesc"><fmt:message key="dsEdit.alpha2.desc"/></c:set>
<c:set var="dsHelpId" value="asciiSerialDS"/>
<%@ include file="/WEB-INF/jsp/dataSourceEdit/dsHead.jspf" %>

<tr>
	<td class="formLabelRequired"><fmt:message
		key="dsEdit.asciiSerial.commPortId" /></td>
	<td class="formField"><c:choose>
		<c:when test="${!empty commPortError}">
			<input id="commPortId" type="hidden" value="" />
			<span class="formError">${commPortError}</span>
		</c:when>
		<c:otherwise>
			<sbt:select id="commPortId" value="${dataSource.commPortId}">
				<c:forEach items="${commPorts}" var="port">
					<sbt:option value="${port.name}">${port.name}</sbt:option>
				</c:forEach>
			</sbt:select>
		</c:otherwise>
	</c:choose></td>
</tr>

<tr>
	<td class="formLabelRequired"><fmt:message
		key="dsEdit.asciiSerial.baud" /></td>
	<td class="formField"><sbt:select id="baudRate"
		value="${dataSource.baudRate}">
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
	</sbt:select></td>
</tr>

<tr>
  <td class="formLabelRequired"><fmt:message key="dsEdit.asciiSerial.dataBits"/></td>
  <td class="formField">
    <sbt:select id="dataBits" value="${dataSource.dataBits}">
      <sbt:option value="5">5</sbt:option>
      <sbt:option value="6">6</sbt:option>
      <sbt:option value="7">7</sbt:option>
      <sbt:option value="8">8</sbt:option>
    </sbt:select>
  </td>
</tr>

<tr>
  <td class="formLabelRequired"><fmt:message key="dsEdit.asciiSerial.stopBits"/></td>
  <td class="formField">
    <sbt:select id="stopBits" value="${dataSource.stopBits}">
      <sbt:option value="1">1</sbt:option>
      <sbt:option value="3">1.5</sbt:option>
      <sbt:option value="2">2</sbt:option>
    </sbt:select>
  </td>
</tr>

<tr>
  <td class="formLabelRequired"><fmt:message key="dsEdit.asciiSerial.parity"/></td>
  <td class="formField">
    <sbt:select id="parity" value="${dataSource.parity}">
      <sbt:option value="0"><fmt:message key="dsEdit.modbusSerial.parity.none"/></sbt:option>
      <sbt:option value="1"><fmt:message key="dsEdit.modbusSerial.parity.odd"/></sbt:option>
      <sbt:option value="2"><fmt:message key="dsEdit.modbusSerial.parity.even"/></sbt:option>
      <sbt:option value="3"><fmt:message key="dsEdit.modbusSerial.parity.mark"/></sbt:option>
      <sbt:option value="4"><fmt:message key="dsEdit.modbusSerial.parity.space"/></sbt:option>
    </sbt:select>
  </td>
</tr>

  <tr>
    <td class="formLabelRequired"><fmt:message key="dsEdit.updatePeriod"/></td>
    <td class="formField">
      <input type="text" id="updatePeriods" value="${dataSource.updatePeriods}" class="formShort" />
      <sbt:select id="updatePeriodType" value="${dataSource.updatePeriodType}">
        <tag:timePeriodOptions sst="true" ms="true" s="true" min="true" h="true"/>
      </sbt:select>
    </td>
  </tr>
  
  <tr>
    <td class="formLabelRequired"><fmt:message key="dsEdit.asciiSerial.retries"/></td>
    <td class="formField"><input type="text" id="retries" value="${dataSource.retries}"/></td>
  </tr>
  <tr>
    <td class="formLabelRequired"><fmt:message key="dsEdit.asciiSerial.timeout"/></td>
    <td class="formField"><input type="text" id="timeout" value="${dataSource.timeout}"/></td>
  </tr>
  
  <tr>
    <td class="formLabelRequired"><fmt:message key="dsEdit.alpha2.station"/></td>
    <td class="formField">
    	<input id="station" type="text" value="${dataSource.station}"/>
    </td>
  </tr>
  
<%@ include file="/WEB-INF/jsp/dataSourceEdit/dsEventsFoot.jspf" %>

<tag:pointList pointHelpId="asciiSerialPP">
  <tr>
          <td class="formLabelRequired"><fmt:message key="dsEdit.alpha2.pointType"/></td>
          <td class="formField"> 
            <select id="deviceCode" onchange="deviceCodeChanged()">
              <option value="<c:out value="<%= Alpha2PointLocatorVO.RUN_STOP_CODE %>"/>"><fmt:message key="dsEdit.alpha2.deviceCode.runStop"/></option>
              <option value="<c:out value="<%= DeviceCodes.SYSTEM_BIT.getId() %>"/>"><fmt:message key="dsEdit.alpha2.deviceCode.systemBit"/></option>
              <option value="<c:out value="<%= DeviceCodes.INPUT_TERMINAL.getId() %>"/>"><fmt:message key="dsEdit.alpha2.deviceCode.inputTerminal"/></option>
              <option value="<c:out value="<%= DeviceCodes.EXTERNAL_INPUT.getId() %>"/>"><fmt:message key="dsEdit.alpha2.deviceCode.externalInput"/></option>
              <option value="<c:out value="<%= DeviceCodes.OUTPUT_TERMINAL.getId() %>"/>"><fmt:message key="dsEdit.alpha2.deviceCode.outputTerminal"/></option>
              <option value="<c:out value="<%= DeviceCodes.EXTERNAL_OUTPUT.getId() %>"/>"><fmt:message key="dsEdit.alpha2.deviceCode.externalOutput"/></option>
              <option value="<c:out value="<%= DeviceCodes.KEY_INPUT.getId() %>"/>"><fmt:message key="dsEdit.alpha2.deviceCode.keyInput"/></option>
              <option value="<c:out value="<%= DeviceCodes.LINK_INPUT.getId() %>"/>"><fmt:message key="dsEdit.alpha2.deviceCode.linkInput"/></option>
              <option value="<c:out value="<%= DeviceCodes.LINK_OUTPUT.getId() %>"/>"><fmt:message key="dsEdit.alpha2.deviceCode.linkOutput"/></option>
              <option value="<c:out value="<%= DeviceCodes.CONTROL_DEVICE.getId() %>"/>"><fmt:message key="dsEdit.alpha2.deviceCode.controlDevice"/></option>
              <option value="<c:out value="<%= DeviceCodes.ANALOG_IN.getId() %>"/>"><fmt:message key="dsEdit.alpha2.deviceCode.analogIn"/></option>
              <option value="<c:out value="<%= DeviceCodes.COMM_BIT_DEVICE.getId() %>"/>"><fmt:message key="dsEdit.alpha2.deviceCode.commBitDevice"/></option>
              <option value="<c:out value="<%= DeviceCodes.COMM_WORD_DEVICE.getId() %>"/>"><fmt:message key="dsEdit.alpha2.deviceCode.commWordDevice"/></option>
            </select>
          </td>
        </tr>
  <tr>
  
  <tbody id="accessModeDiv"> 
  	<td class="formLabelRequired"><fmt:message key="dsEdit.alpha2.deviceNumber"/></td>
	  	<td class="formField">
		<input id="deviceNumber" type="text" value=""/>
	</td>
	
		<tr>
          <td class="formLabelRequired"><fmt:message key="dsEdit.alpha2.accessMode"/></td>
          <td class="formField"> 
            <select id="accessMode">
              <option value="<c:out value="<%= Alpha2PointLocatorVO.READ_ONLY %>"/>"><fmt:message key="dsEdit.alpha2.accessMode.readOnly"/></option>
              <option value="<c:out value="<%= Alpha2PointLocatorVO.WRITE_ONLY %>"/>"><fmt:message key="dsEdit.alpha2.accessMode.writeOnly"/></option>
              <option value="<c:out value="<%= Alpha2PointLocatorVO.READ_WRITE %>"/>"><fmt:message key="dsEdit.alpha2.accessMode.readWrite"/></option>
            </select>
          </td>
        </tr>
	</tbody>
	  
  <tr>
  </tr>
</tag:pointList>