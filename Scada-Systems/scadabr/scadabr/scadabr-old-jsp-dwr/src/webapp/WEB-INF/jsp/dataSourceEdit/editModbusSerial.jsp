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
<%@page import="com.serotonin.modbus4j.serial.SerialMaster"%>
<%@ include file="/WEB-INF/jsp/include/tech.jsp" %>
<%@page import="com.serotonin.mango.vo.dataSource.modbus.ModbusSerialDataSourceVO"%>

<script type="text/javascript">
  function scanImpl() {
      DataSourceEditDwr.modbusSerialScan($get("timeout"), $get("retries"), $get("commPortId"), $get("baudRate"),
              $get("flowControlIn"), $get("flowControlOut"), $get("dataBits"), $get("stopBits"), $get("parity"), 
              $get("encoding"), $get("concurrency"), scanCB);
  }
  
  function locatorTestImpl(locator) {
      DataSourceEditDwr.testModbusSerialLocator($get("timeout"), $get("retries"), $get("commPortId"), $get("baudRate"),
              $get("flowControlIn"), $get("flowControlOut"), $get("dataBits"), $get("stopBits"), $get("parity"), 
              $get("encoding"), $get("concurrency"), locator, locatorTestCB);
  }
  
  function dataTestImpl(slaveId, range, offset, length) {
      DataSourceEditDwr.testModbusSerialData($get("timeout"), $get("retries"), $get("commPortId"), $get("baudRate"),
              $get("flowControlIn"), $get("flowControlOut"), $get("dataBits"), $get("stopBits"), $get("parity"), 
              $get("encoding"), $get("concurrency"), slaveId, range, offset, length, dataTestCB);
  }
  
  function saveDataSourceImpl() {
      DataSourceEditDwr.saveModbusSerialDataSource($get("dataSourceName"), $get("dataSourceXid"), $get("updatePeriods"), 
              $get("updatePeriodType"), $get("quantize"), $get("timeout"), $get("retries"), $get("contiguousBatches"),
              $get("createSlaveMonitorPoints"), $get("maxReadBitCount"), $get("maxReadRegisterCount"),
              $get("maxWriteRegisterCount"), $get("commPortId"), $get("baudRate"), $get("flowControlIn"),
              $get("flowControlOut"), $get("dataBits"), $get("stopBits"), $get("parity"), $get("encoding"),
              $get("echo"), $get("concurrency"), saveDataSourceCB);
  }
</script>

<tr>
  <td class="formLabelRequired"><fmt:message key="dsEdit.modbusSerial.port"/></td>
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
  <td class="formLabelRequired"><fmt:message key="dsEdit.modbusSerial.baud"/></td>
  <td class="formField">
    <sbt:select id="baudRate" value="${dataSource.baudRate}">
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
  <td class="formLabelRequired"><fmt:message key="dsEdit.modbusSerial.flowIn"/></td>
  <td class="formField">
    <sbt:select id="flowControlIn" value="${dataSource.flowControlIn}">
      <sbt:option value="0"><fmt:message key="dsEdit.modbusSerial.flow.none"/></sbt:option>
      <sbt:option value="1"><fmt:message key="dsEdit.modbusSerial.flow.rtsCts"/></sbt:option>
      <sbt:option value="4"><fmt:message key="dsEdit.modbusSerial.flow.xonXoff"/></sbt:option>
    </sbt:select>
  </td>
</tr>

<tr>
  <td class="formLabelRequired"><fmt:message key="dsEdit.modbusSerial.flowOut"/></td>
  <td class="formField">
    <sbt:select id="flowControlOut" value="${dataSource.flowControlOut}">
      <sbt:option value="0"><fmt:message key="dsEdit.modbusSerial.flow.none"/></sbt:option>
      <sbt:option value="2"><fmt:message key="dsEdit.modbusSerial.flow.rtsCts"/></sbt:option>
      <sbt:option value="8"><fmt:message key="dsEdit.modbusSerial.flow.xonXoff"/></sbt:option>
    </sbt:select>
  </td>
</tr>

<tr>
  <td class="formLabelRequired"><fmt:message key="dsEdit.modbusSerial.dataBits"/></td>
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
  <td class="formLabelRequired"><fmt:message key="dsEdit.modbusSerial.stopBits"/></td>
  <td class="formField">
    <sbt:select id="stopBits" value="${dataSource.stopBits}">
      <sbt:option value="1">1</sbt:option>
      <sbt:option value="3">1.5</sbt:option>
      <sbt:option value="2">2</sbt:option>
    </sbt:select>
  </td>
</tr>

<tr>
  <td class="formLabelRequired"><fmt:message key="dsEdit.modbusSerial.parity"/></td>
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
  <td class="formLabelRequired"><fmt:message key="dsEdit.modbusSerial.encoding"/></td>
  <td class="formField">
    <sbt:select id="encoding" value="${dataSource.encoding}">
      <sbt:option value="<%= ModbusSerialDataSourceVO.EncodingType.RTU.toString() %>"><fmt:message key="dsEdit.modbusSerial.encoding.rtu"/></sbt:option>
      <sbt:option value="<%= ModbusSerialDataSourceVO.EncodingType.ASCII.toString() %>"><fmt:message key="dsEdit.modbusSerial.encoding.ascii"/></sbt:option>
    </sbt:select>
  </td>
</tr>

<tr>
  <td class="formLabelRequired"><fmt:message key="dsEdit.modbusSerial.echo"/></td>
  <td class="formField">
    <sbt:select id="echo" value="${dataSource.echo}">
      <sbt:option value="false"><fmt:message key="dsEdit.modbusSerial.echo.off"/></sbt:option>
      <sbt:option value="true"><fmt:message key="dsEdit.modbusSerial.echo.on"/></sbt:option>
    </sbt:select>
  </td>
</tr>

<tr>
  <td class="formLabelRequired"><fmt:message key="dsEdit.modbusSerial.concurrency"/></td>
  <td class="formField">
    <sbt:select id="concurrency" value="${dataSource.concurrency}">
      <sbt:option value="<%= Integer.toString(SerialMaster.SYNC_TRANSPORT) %>"><fmt:message key="dsEdit.modbusSerial.concurrency.transport"/></sbt:option>
      <sbt:option value="<%= Integer.toString(SerialMaster.SYNC_SLAVE) %>"><fmt:message key="dsEdit.modbusSerial.concurrency.slave"/></sbt:option>
      <sbt:option value="<%= Integer.toString(SerialMaster.SYNC_FUNCTION) %>"><fmt:message key="dsEdit.modbusSerial.concurrency.function"/></sbt:option>
    </sbt:select>
  </td>
</tr>