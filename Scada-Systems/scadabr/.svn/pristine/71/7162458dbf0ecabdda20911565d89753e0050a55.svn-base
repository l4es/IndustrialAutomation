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

<script type="text/javascript">
  var currentChangeType;

  function scanImpl() {
		DataSourceEditDwr.DrStorageHt5bScan($get("timeout"), $get("retries"),
				$get("commPortId"), $get("baudRate"), $get("dataBits"),
				$get("stopBits"), $get("parity"), scanCB);
  }
	
  function saveDataSourceImpl() {
      DataSourceEditDwr.saveDrStorageHt5bDataSource($get("dataSourceName"), $get("dataSourceXid"), $get("updatePeriods"),
              $get("updatePeriodType"),	$get("commPortId"), $get("baudRate"), $get("dataBits"),
				$get("stopBits"), $get("parity"), $get("timeout"), $get("retries"),$get("initString"), 
				$get("quantize"), saveDataSourceCB);
  }

  function editPointCBImpl(locator) {
	  $set("pointType", locator.pointType);
  }
  
  function savePointImpl(locator) {

	locator.pointType = $get("pointType");

 	DataSourceEditDwr.saveDrStorageHt5bPointLocator(currentPoint.id, $get("xid"), $get("name"), locator, savePointCB);
  }
  
  function initImpl() {
  }
  
  function changeDataType() {
      DataSourceEditDwr.getChangeTypes($get("dataTypeId"), changeDataTypeCB);
  }
  
  function changeDataTypeCB(changeTypes) {
      var changeTypeDD = $("changeTypeId");
      var savedType = changeTypeDD.value;
      dwr.util.removeAllOptions(changeTypeDD);
      dwr.util.addOptions(changeTypeDD, changeTypes, "key", "message");
      changeTypeDD.value = savedType;
      changeChangeType();
  }
  
  // List manipulation.
  function addListValue(prefix) {
  }
  
  function removeListValue(theValue, prefix) {
  }
  
  function refreshValueList(prefix, arr) {
  }
</script>

<c:set var="dsDesc"><fmt:message key="dsEdit.drStorageHt5b.desc"/></c:set>
<c:set var="dsHelpId" value="drStorageHt5bDS"/>
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
    <td class="formLabelRequired"><fmt:message key="dsEdit.quantize"/></td>
    <td class="formField"><sbt:checkbox id="quantize" selectedValue="${dataSource.quantize}"/></td>
  </tr>
  
  <tr>
    <td class="formLabelRequired"><fmt:message key="dsEdit.asciiSerial.initString"/></td>
    <td class="formField">
    	<input id="initString" type="text" value=""/>
    </td>
  </tr>
  
<%@ include file="/WEB-INF/jsp/dataSourceEdit/dsEventsFoot.jspf" %>

<tag:pointList pointHelpId="drStorageHt5bPP">

	 <tr>
    <td class="formLabelRequired"><fmt:message key="dsEdit.drStorageHt5b.pointType"/></td>
    <td class="formField">
    	<sbt:select id="pointType"
			value="${dataPoint.pointType}">
			<sbt:option value="Temperature" ><fmt:message key="dsEdit.drStorageHt5b.type.Temperature"/></sbt:option>
			<sbt:option value="Humidity" ><fmt:message key="dsEdit.drStorageHt5b.type.Humidity"/></sbt:option>
		</sbt:select>
    </td>
  </tr>

</tag:pointList>