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
<%@page import="com.serotonin.spinwave.SpinwaveReceiver"%>
<%@page import="com.serotonin.mango.vo.dataSource.spinwave.SpinwaveV1PointLocatorVO"%>
<%@page import="com.serotonin.mango.vo.dataSource.spinwave.SpinwaveV2PointLocatorVO"%>
<%@page import="com.serotonin.spinwave.v2.SensorValue"%>

<script type="text/javascript">
  var sensorAddresses = new Array();
  var usedSensorAddresses = new Array();
  var v1Attrs = new Array();
  var v2Attrs = new Array();
  
  function initImpl() {
      <c:forEach items="${dataSource.sensorAddresses}" var="addr">
        sensorAddresses[sensorAddresses.length] = ${addr};
      </c:forEach>
      sortSensorAddresses();
      refreshSensorAddressList();
      swListenButtons(false);
      
      // Version 1 attributes
      v1Attrs[v1Attrs.length] = {id: <%= SpinwaveV1PointLocatorVO.AttributeTypes.TEMPURATURE %>,
              name: "<fmt:message key="<%= SpinwaveV1PointLocatorVO.getAttributeDescription(SpinwaveV1PointLocatorVO.AttributeTypes.TEMPURATURE) %>"/>"};
      v1Attrs[v1Attrs.length] = {id: <%= SpinwaveV1PointLocatorVO.AttributeTypes.SET_POINT %>,
              name: "<fmt:message key="<%= SpinwaveV1PointLocatorVO.getAttributeDescription(SpinwaveV1PointLocatorVO.AttributeTypes.SET_POINT) %>"/>"};
      v1Attrs[v1Attrs.length] = {id: <%= SpinwaveV1PointLocatorVO.AttributeTypes.BATTERY %>,
              name: "<%= SpinwaveV1PointLocatorVO.getAttributeDescription(SpinwaveV1PointLocatorVO.AttributeTypes.BATTERY) %>"/>"};
      v1Attrs[v1Attrs.length] = {id: <%= SpinwaveV1PointLocatorVO.AttributeTypes.OVERRIDE %>,
             name: "<fmt:message key="<%= SpinwaveV1PointLocatorVO.getAttributeDescription(SpinwaveV1PointLocatorVO.AttributeTypes.OVERRIDE) %>"/>"};
      
      // Version 2 attributes
      <c:forEach items="<%= SpinwaveV2PointLocatorVO.attributeTypes %>" var="attr">
        v2Attrs[v2Attrs.length] = {id: ${attr.key}, name: "<fmt:message key="${attr.value}"/>"};
      </c:forEach>
      
      messageVersionChange();
  }
  
  function addAddress(addr) {
      if (!addr)
          addr = $get("sensorAddressText");
      addr = parseInt(addr);
      
      if (isNaN(addr)) {
          alert("<fmt:message key="dsEdit.spinwave.errorParsingAddress"/>");
          return;
      }
      
      for (var i=sensorAddresses.length-1; i>=0; i--) {
          if (sensorAddresses[i] == addr) {
              alert("<fmt:message key="dsEdit.spinwave.invalidAddress"/> "+ addr);
              return;
          }
      }
      
      sensorAddresses[sensorAddresses.length] = addr;
      sortSensorAddresses();
      refreshSensorAddressList();
      $set("sensorAddressText", addr+1);
  }
  
  function removeAddress(addr) {
      if (contains(usedSensorAddresses, addr)) {
          alert("<fmt:message key="dsEdit.spinwave.addressInUse"/>");
          return;
      }
  
      for (var i=sensorAddresses.length-1; i>=0; i--) {
          if (sensorAddresses[i] == addr)
              sensorAddresses.splice(i, 1);
      }
      refreshSensorAddressList();
  }
  
  function sortSensorAddresses() {
      sensorAddresses.sort( function(a,b) { return a-b; } );
  }
  
  function refreshSensorAddressList() {
      dwr.util.removeAllRows("sensorAddressList");
      if (sensorAddresses.length == 0)
          show("noAddressesMessage");
      else {
          hide("noAddressesMessage");
          dwr.util.addRows("sensorAddressList", sensorAddresses, [
                  function(data) { return ""; },
                  function(data) {
                      return data +" <img src='images/bullet_delete.png' onclick='removeAddress("+ data + ");' "+
                              "class='ptr' title='<fmt:message key="common.delete"/>'/>";
                  }
                  ], null);
      }
      
      var addrSelect = $("sensorAddress");
      var oldValue = addrSelect.value;
      dwr.util.removeAllOptions(addrSelect);
      dwr.util.addOptions(addrSelect, sensorAddresses);
      addrSelect.value = oldValue;
  }
  
  function saveDataSourceImpl() {
      swListenCancel();
      DataSourceEditDwr.saveSpinwaveDataSource($get("dataSourceName"), $get("dataSourceXid"), $get("commPortId"),
              sensorAddresses, $get("messageVersion"), $get("heartbeatTimeout"), saveDataSourceCB);
  }
  
  function swListen() {
      $set("swListenMessage", "<fmt:message key="dsEdit.spinwave.listening"/>");
      dwr.util.removeAllOptions("swListenSensors");
      swListenButtons(true);
      DataSourceEditDwr.spinwaveListenForSensors($get("commPortId"), $get("messageVersion"), swListenCB);
  }
  
  function swListenCB(msg) {
      setTimeout(swListenUpdate, 2000);
  }
  
  function swListenButtons(listening) {
      setDisabled("swListenBtn", listening);
      setDisabled("swListenCancelBtn", !listening);
  }
  
  function swListenUpdate() {
      DataSourceEditDwr.spinwaveListenerUpdate(swListenUpdateCB);
  }
  
  function swListenUpdateCB(result) {
      if (result) {
          $set("swListenMessage", result.message);
          dwr.util.removeAllOptions("swListenSensors");
          dwr.util.addOptions("swListenSensors", result.nodes);
          swListenCB();
      }
  }
  
  function swListenCancel() {
      DataSourceEditDwr.cancelTestingUtility(swListenCancelCB);
  }
  
  function swListenCancelCB() {
      swListenButtons(false);
      $("swListenMessage").innerHTML = "<fmt:message key="common.cancelled"/>";
  }
  
  function swAddSensorsToList() {
      var sensors = $("swListenSensors").options;
      for (var i=0; i<sensors.length; i++)
          addAddress(sensors[i].value);
  }
  
  function writePointListImpl(points) {
      var msgVersion = $("messageVersion");
      if (!points || points.length == 0) {
          setDisabled(msgVersion, false);
          hide("messageVersionWarning");
      }
      else {
          setDisabled(msgVersion, true);
          show("messageVersionWarning");
      }
      
      updateUsedSensorAddresses(points);
  }
  
  function appendPointListColumnFunctions(pointListColumnHeaders, pointListColumnFunctions) {
      pointListColumnHeaders[pointListColumnHeaders.length] = "<fmt:message key="dsEdit.spinwave.address"/>";
      pointListColumnFunctions[pointListColumnFunctions.length] = function(p) { return p.pointLocator.sensorAddress; };
      
      pointListColumnHeaders[pointListColumnHeaders.length] = "<fmt:message key="dsEdit.spinwave.attribute"/>";
      pointListColumnFunctions[pointListColumnFunctions.length] = function(p) { return p.pointLocator.attributeDescription; };
  }
  
  function updateUsedSensorAddresses(points) {
      usedSensorAddresses.length = 0;
      if (points) {
          for (var i=0; i<points.length; i++) {
              if (!contains(usedSensorAddresses, points[i].sensorAddress))
                  usedSensorAddresses[usedSensorAddresses.length] = points[i].pointLocator.sensorAddress;
          }
      }
  }
  
  function editPointCBImpl(locator) {
      $set("sensorAddress", locator.sensorAddress);
      $set("attributeId", locator.attributeId);
      $set("convertToCelsius", locator.convertToCelsius);
      changeAttributeId();
  }
  
  function savePointImpl(locator) {
      delete locator.attributeDescription;
      delete locator.attributeTypes;
      delete locator.settable;
      delete locator.dataTypeId;
      
      locator.sensorAddress = $get("sensorAddress");
      locator.attributeId = $get("attributeId");
      locator.convertToCelsius = $get("convertToCelsius");
      
      if ($get("messageVersion") == <%= SpinwaveReceiver.VERSION_1 %>)
          DataSourceEditDwr.saveSpinwaveV1PointLocator(currentPoint.id, $get("xid"), $get("name"), locator,
                  savePointCB);
      else
          DataSourceEditDwr.saveSpinwaveV2PointLocator(currentPoint.id, $get("xid"), $get("name"), locator,
                  savePointCB);
  }
  
  function messageVersionChange() {
      dwr.util.removeAllOptions("attributeId");
      var opts;
      if ($get("messageVersion") == <%= SpinwaveReceiver.VERSION_1 %>)
          opts = v1Attrs;
      else
          opts = v2Attrs;
      dwr.util.addOptions("attributeId", opts, "id", "name");
  }
  
  function changeAttributeId() {
      var attributeId = $get("attributeId");
      var disable;
      if ($get("messageVersion") == <%= SpinwaveReceiver.VERSION_1 %>)
          disable = !(attributeId == <%= SpinwaveV1PointLocatorVO.AttributeTypes.TEMPURATURE %> ||
                attributeId == <%= SpinwaveV1PointLocatorVO.AttributeTypes.SET_POINT %>);
      else
          disable = !(attributeId == <%= SensorValue.TYPE_TEMPERATURE %>);
      setDisabled("convertToCelsius", disable);
  }
</script>

<c:set var="dsDesc"><fmt:message key="dsEdit.spinwave.desc"/></c:set>
<c:set var="dsHelpId" value="spinwaveDS"/>
<%@ include file="/WEB-INF/jsp/dataSourceEdit/dsHead.jspf" %>
        <tr>
          <td class="formLabelRequired"><fmt:message key="dsEdit.spinwave.port"/></td>
          <td class="formField">
            <c:choose>
              <c:when test="${!empty commPortError}">
                <input id="commPortId" type="hidden" value=""/>
                <span class="formError">${commPortError}</span>
              </c:when>
              <c:otherwise>
                <sbt:select id="commPortId" value="${dataSource.commPortId}">
                  <c:forEach items="${commPorts}" var="port">
                    <sbt:option value="${port.name}">${port.name}</sbt:option>
                  </c:forEach>
                </sbt:select>
              </c:otherwise>
            </c:choose>
          </td>
        </tr>
        
        <tr>
          <td class="formLabelRequired"><fmt:message key="dsEdit.spinwave.heartbeatTimeout"/></td>
          <td class="formField"><input id="heartbeatTimeout" type="text" value="${dataSource.heartbeatTimeout}"/></td>
        </tr>
        
        <tr>
          <td class="formLabelRequired"><fmt:message key="dsEdit.spinwave.version"/></td>
          <td class="formField">
            <sbt:select id="messageVersion" value="${dataSource.messageVersion}" onchange="messageVersionChange()">
              <sbt:option><%= SpinwaveReceiver.VERSION_1 %></sbt:option>
              <sbt:option><%= SpinwaveReceiver.VERSION_2 %></sbt:option>
            </sbt:select>
            <span id="messageVersionWarning" style="display:none"><tag:img png="warn"
                    title="dsEdit.spinwave.deleteWarn"/></span>
          </td>
        </tr>
        
        <tr>
          <td class="formLabelRequired"><fmt:message key="dsEdit.spinwave.sensorAddresses"/></td>
          <td class="formField">
            <table cellpadding="0" cellspacing="0">
              <tr>
                <td><input id="sensorAddressText" type="text"/></td>
                <td width="10"></td>
                <td><tag:img png="add" onclick="addAddress()" title="dsEdit.spinwave.addSensorAddresses"/></td>
              </tr>
            </table>
          </td>
        </tr>
        <tr id="noAddressesMessage" style="display:none;">
          <td></td>
          <td><fmt:message key="dsEdit.spinwave.noAddresses"/></td>
        </tr>
        <tbody id="sensorAddressList"></tbody>
      </table>
      <tag:dsEvents/>
    </div>
  </td>
  
  <td valign="top">
    <div class="borderDiv marB">
      <table>
        <tr><td colspan="2" class="smallTitle"><fmt:message key="dsEdit.spinwave.listener"/></td></tr>
        
        <tr>
          <td colspan="2" align="center">
            <input id="swListenBtn" type="button" value="<fmt:message key="dsEdit.spinwave.listen"/>" onclick="swListen();"/>
            <input id="swListenCancelBtn" type="button" value="<fmt:message key="common.cancel"/>" onclick="swListenCancel();"/>
          </td>
        </tr>
        
        <tr><td colspan="2" id="swListenMessage" class="formError"></td></tr>
        
        <tr>
          <td class="formLabel"><fmt:message key="dsEdit.spinwave.sensorsFound"/></td>
          <td class="formField"><select id="swListenSensors" size="8"></select></td>
        </tr>
        
        <tr>
          <td colspan="2" align="center">
            <input type="button" value="<fmt:message key="dsEdit.spinwave.addToList"/>" onclick="swAddSensorsToList();"/>
          </td>
        </tr>
<%@ include file="/WEB-INF/jsp/dataSourceEdit/dsFoot.jspf" %>

<tag:pointList pointHelpId="spinwavePP">
  <tr>
    <td class="formLabelRequired"><fmt:message key="dsEdit.spinwave.sensorAddress"/></td>
    <td class="formField"><select id="sensorAddress"></select></td>
  </tr>
  
  <tr>
    <td class="formLabelRequired"><fmt:message key="dsEdit.spinwave.attribute"/></td>
    <td class="formField"><select id="attributeId" onchange="changeAttributeId()"></select></td>
  </tr>
  
  <tr>
    <td class="formLabelRequired"><fmt:message key="dsEdit.spinwave.convert"/></td>
    <td class="formField"><input type="checkbox" id="convertToCelsius"/></td>
  </tr>
</tag:pointList>