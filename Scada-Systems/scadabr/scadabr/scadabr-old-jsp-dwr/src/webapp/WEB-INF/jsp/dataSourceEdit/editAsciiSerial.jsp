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
		DataSourceEditDwr.asciiSerialScan($get("timeout"), $get("retries"),
				$get("commPortId"), $get("baudRate"), $get("dataBits"),
				$get("stopBits"), $get("parity"), scanCB);
  }
	
  function saveDataSourceImpl() {
      DataSourceEditDwr.saveASCIISerialDataSource($get("dataSourceName"), $get("dataSourceXid"), $get("updatePeriods"),
              $get("updatePeriodType"),	$get("commPortId"), $get("baudRate"), $get("dataBits"),
				$get("stopBits"), $get("parity"), $get("timeout"), $get("retries"), $get("stopMode"), 
				$get("nChar"), $get("charStopMode"), $get("charX"), $get("hexValue"), $get("stopTimeout"), 
				$get("initString"), $get("bufferSize"), $get("quantize"), saveDataSourceCB);
  }
  
  function initImpl() {
	  hide("tsDiv1");
	  hide("tsDiv2");
	  changeStopMode();
  }
  
  function editPointCBImpl(locator) {
	  $set("valueRegex", locator.valueRegex);
	  $set("dataTypeId", locator.dataType);
	  $set("command", locator.command);
	  $set("customTimestamp",locator.customTimestamp);
	  checkTimestampChanged();

	  if(locator.customTimestamp) {
		  $set("timestampRegex", locator.timestampRegex);
		  $set("timestampFormat",locator.timestampFormat);
	  }
  }
  
  function savePointImpl(locator) {
	  locator.dataType = $get("dataTypeId");
	  locator.command = $get("command");
      locator.valueRegex = $get("valueRegex");
	  locator.customTimestamp = $get("customTimestamp");
	  locator.timestampFormat = $get("timestampFormat");
      locator.timestampRegex = $get("timestampRegex");
      
      DataSourceEditDwr.saveASCIISerialPointLocator(currentPoint.id, $get("xid"), $get("name"), locator, savePointCB);
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
  
  function valueSuggestChanged() {
	  $set("valueRegex",$get("valueSuggestions"));
  }

  function timestampSuggestChanged() {
	  suggest = document.getElementById("timestampSuggestions");
	  index = suggest.selectedIndex;
	  dataFormat = suggest.options[index].title;
	  $set("timestampFormat",dataFormat);
	  $set("timestampRegex",$get("timestampSuggestions"));
  }
  
  function checkTimestampChanged() {
	  if($get("customTimestamp")) {
		  show("tsDiv1");
		  show("tsDiv2");
	  } else {
		  hide("tsDiv1");
		  hide("tsDiv2");
	  }
  }

  function changeStopMode() {
      var sm = $get("stopMode");
      if(sm == 0) {
      	setDisabled("nChar", false);
      	setDisabled("charStopMode", true);
      	setDisabled("charX", true);
      	setDisabled("hexValue", true);
      	setDisabled("stopTimeout", true);
      } else if(sm == 1) {
      	setDisabled("nChar", true);
       	setDisabled("charStopMode", false);
       	changeCharStopMode();
       	setDisabled("stopTimeout", true);
      } else {
	    setDisabled("nChar", true);
       	setDisabled("charStopMode", true);
       	setDisabled("charX", true);
      	setDisabled("hexValue", true);
       	setDisabled("stopTimeout", false);
      }
  }

  function changeCharStopMode() {
      var sm = $get("charStopMode");
      if(sm == 0) {
      	setDisabled("charX", false);
      	setDisabled("hexValue", true);
      } else {
       	setDisabled("charX", true);
       	setDisabled("hexValue", false);
      }
  }
  //
  // List manipulation.
  function addListValue(prefix) {
  }
  
  function removeListValue(theValue, prefix) {
  }
  
  function refreshValueList(prefix, arr) {
  }
</script>

<c:set var="dsDesc"><fmt:message key="dsEdit.asciiSerial.desc"/></c:set>
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
    <td class="formLabelRequired"><fmt:message key="dsEdit.quantize"/></td>
    <td class="formField"><sbt:checkbox id="quantize" selectedValue="${dataSource.quantize}"/></td>
  </tr>
  
  <tr>
  <td class="formLabelRequired"><fmt:message key="dsEdit.asciiSerial.stopMode"/></td>
  <td class="formField">
    <sbt:select id="stopMode" onchange="changeStopMode()" value="${dataSource.stopMode}">
      <sbt:option value="0"><fmt:message key="dsEdit.asciiSerial.stopMode.nChar"/></sbt:option>
      <sbt:option value="1"><fmt:message key="dsEdit.asciiSerial.stopMode.charX"/></sbt:option>
      <sbt:option value="2"><fmt:message key="dsEdit.asciiSerial.stopMode.stopTimeout"/></sbt:option>
    </sbt:select>
  </td>
</tr>
  
  <tr>
    <td class="formLabelRequired"><fmt:message key="dsEdit.asciiSerial.nChar"/></td>
    <td class="formField">
    	<input id="nChar" type="text" value="${dataSource.nChar}"/>
    </td>
  </tr>
  <tr>
  <td class="formLabelRequired"><fmt:message key="dsEdit.asciiSerial.charStopMode"/></td>
  <td class="formField">
    <sbt:select id="charStopMode" onchange="changeCharStopMode()" value="${dataSource.charStopMode}">
      <sbt:option value="0"><fmt:message key="dsEdit.asciiSerial.charStopMode.charASCII"/></sbt:option>
      <sbt:option value="1"><fmt:message key="dsEdit.asciiSerial.charStopMode.hexValue"/></sbt:option>
    </sbt:select>
  </td>
</tr>
  <tr>
    <td class="formLabelRequired"><fmt:message key="dsEdit.asciiSerial.charStopMode.charASCII"/></td>
    <td class="formField">
    	<input id="charX" type="text" value="${dataSource.charX}" disabled="disabled"/>
    </td>
  </tr>
  <tr>
    <td class="formLabelRequired"><fmt:message key="dsEdit.asciiSerial.charStopMode.hexValue"/></td>
    <td class="formField">
    	<input id="hexValue" type="text" value="${dataSource.hexValue}" disabled="disabled"/>
    </td>
  </tr>
  
  <tr>
    <td class="formLabelRequired"><fmt:message key="dsEdit.asciiSerial.stopTimeout"/></td>
    <td class="formField">
    	<input id="stopTimeout" type="text" value="${dataSource.stopTimeout}" disabled="disabled"/>
    </td>
  </tr>
   
  <tr>
  <td class="formLabelRequired"><fmt:message key="dsEdit.asciiSerial.bufferSize"/></td>
  <td class="formField">
    <sbt:select id="bufferSize" value="${dataSource.bufferSize}">
      <sbt:option value="2">2</sbt:option>
      <sbt:option value="4">4</sbt:option>
      <sbt:option value="8">8</sbt:option>
      <sbt:option value="16">16</sbt:option>
      <sbt:option value="32">32</sbt:option>
      <sbt:option value="64">64</sbt:option>
      <sbt:option value="256">256</sbt:option>
      <sbt:option value="1024">1024</sbt:option>
    </sbt:select>
  	</td>
  </tr>
  
  <tr>
    <td class="formLabelRequired"><fmt:message key="dsEdit.asciiSerial.initString"/></td>
    <td class="formField">
    	<input id="initString" type="text" value=""/>
    </td>
  </tr>
  
<%@ include file="/WEB-INF/jsp/dataSourceEdit/dsEventsFoot.jspf" %>

<tag:pointList pointHelpId="asciiSerialPP">
  <tr>
    <td class="formLabelRequired"><fmt:message key="dsEdit.pointDataType"/></td>
    <td class="formField">
      <select id="dataTypeId">
        <tag:dataTypeOptions excludeImage="true"/>
      </select>
    </td>
  </tr>
  <tr>
	    <td class="formLabelRequired"><fmt:message key="dsEdit.asciiSerial.command"/></td>
	    <td class="formField">
	    	<input id="command" type="text" value=""/>
	    </td>
  </tr>
  <tr>
    <td class="formLabelRequired"><fmt:message key="dsEdit.asciiFile.valueRegex"/></td>
    <td class="formField">
    	<input id="valueRegex" type="text" value=""/>
    	<select id="valueSuggestions" onchange="valueSuggestChanged();">
    		<option value=""> &nbsp; </option>
        	<option value="((\b[0-9]+)?\.)?[0-9]+\b"> <fmt:message key="dsEdit.asciiFile.regex.number"/>  </option>
      	</select>
    </td>
  </tr>
  <tr>
    <td class="formLabelRequired"><fmt:message key="dsEdit.asciiFile.timestampCheck"/></td>
    <td class="formField"><input id="customTimestamp" type="checkbox" onchange="checkTimestampChanged();"/></td>
  </tr>
	  <tr id="tsDiv1">
	    <td class="formLabelRequired"><fmt:message key="dsEdit.asciiFile.timestampFormat"/></td>
	    <td class="formField">
	    	<input id="timestampFormat" type="text" value=""/>
	    	<select id="timestampSuggestions" onchange="timestampSuggestChanged();">
	    		<option value=""> &nbsp; </option>
	        	<option value="20\d{2}\/((0[1-9])|(1[0-2]))\/((0[1-9])|([1-2][0-9])|(3[0-1]))\s(([0-1][0-9])|(2[0-3])):([0-5][0-9]):([0-5][0-9])" title="yyyy/MM/dd KK:mm:ss"> YYYY/MM/DD HH:MM:SS (2010/12/25 18:30:00) </option>
	        	<option value="20\d{2}-((0[1-9])|(1[0-2]))-((0[1-9])|([1-2][0-9])|(3[0-1]))\s(([0-1][0-9])|(2[0-3])):([0-5][0-9]):([0-5][0-9])" title="yyyy-MM-dd KK:mm:ss"> YYYY-MM-DD HH:MM:SS (2010-12-25 18:30:00)  </option>
	        	<option value="\d{2}\/((0[1-9])|(1[0-2]))\/((0[1-9])|([1-2][0-9])|(3[0-1]))\s(([0-1][0-9])|(2[0-3])):([0-5][0-9]):([0-5][0-9])" title="yy/MM/dd KK:mm:ss"> YY/MM/DD HH:MM:SS (10/12/25 18:30:00) </option>
	        	<option value="\d{2}-((0[1-9])|(1[0-2]))-((0[1-9])|([1-2][0-9])|(3[0-1]))\s(([0-1][0-9])|(2[0-3])):([0-5][0-9]):([0-5][0-9])" title="yy-MM-dd KK:mm:ss"> YY-MM-DD HH:MM:SS (10-12-25 18:30:00) </option>
	      	</select>
	    </td>
	  </tr>
	  <tr id="tsDiv2">
	    <td class="formLabelRequired"><fmt:message key="dsEdit.asciiFile.timestampRegex"/></td>
	    <td class="formField">
	    	<input id="timestampRegex" type="text" value=""/>
	    </td>
	  </tr>
	  
</tag:pointList>