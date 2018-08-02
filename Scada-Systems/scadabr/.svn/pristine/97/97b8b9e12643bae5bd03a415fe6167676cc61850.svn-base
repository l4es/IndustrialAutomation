/*
 *   Mango - Open Source M2M - http://mango.serotoninsoftware.com
 *   Copyright (C) 2010 Arne Pl\u00f6se
 *   @author Arne Pl\u00f6se
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
package br.org.scadabr.vo.datasource.mbus;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;
import java.util.Map;


import br.org.scadabr.json.JsonException;
import br.org.scadabr.json.JsonObject;
import br.org.scadabr.json.JsonReader;

import br.org.scadabr.logger.LogUtils;
import com.serotonin.mango.rt.dataSource.DataSourceRT;
import br.org.scadabr.rt.datasource.mbus.MBusDataSourceRT;
import br.org.scadabr.timer.cron.CronExpression;
import br.org.scadabr.timer.cron.CronParser;
import br.org.scadabr.timer.cron.CronPatterns;
import br.org.scadabr.vo.dataSource.PointLocatorVO;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.util.ExportCodes;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.event.EventTypeVO;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import java.text.ParseException;
import java.util.LinkedHashMap;
import java.util.Set;
import java.util.TimeZone;
import java.util.logging.Logger;
import net.sf.atmodem4j.spsw.SerialPortList;
import net.sf.mbus4j.Connection;
import net.sf.mbus4j.SerialPortConnection;
import net.sf.mbus4j.TcpIpConnection;
import net.sf.mbus4j.dataframes.MBusMedium;


public class MBusDataSourceVO extends DataSourceVO<MBusDataSourceVO> {

    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCARABR_DS_MBUS);

    public static MBusDataSourceVO createNewDataSource() {
        MBusDataSourceVO result = new MBusDataSourceVO();
        result.setConnection(new TcpIpConnection("192.168.1.210", 10001, Connection.DEFAULT_BAUDRATE, TcpIpConnection.DEFAULT_RESPONSE_TIMEOUT_OFFSET));
        return result;
    }
    private static final ExportCodes EVENT_CODES = new ExportCodes();
//TODO more events???

    static {
        EVENT_CODES.addElement(MBusDataSourceRT.DATA_SOURCE_EXCEPTION_EVENT, "DATA_SOURCE_EXCEPTION");
        EVENT_CODES.addElement(MBusDataSourceRT.POINT_READ_EXCEPTION_EVENT, "POINT_READ_EXCEPTION");
        EVENT_CODES.addElement(MBusDataSourceRT.POINT_WRITE_EXCEPTION_EVENT, "POINT_WRITE_EXCEPTION");
    }

    private Connection connection;
    private String cronPattern = CronPatterns._5_MINUTES.getPattern();
    private String cronTimeZone = CronExpression.TIMEZONE_UTC.getID();
    private boolean keepSerialPortOpen;

    @Override
    public Type getType() {
        return Type.M_BUS;
    }

    @Override
    protected void addEventTypes(List<EventTypeVO> eventTypes) {
        eventTypes.add(createEventType(MBusDataSourceRT.DATA_SOURCE_EXCEPTION_EVENT, new LocalizableMessageImpl(
                "event.ds.dataSource")));
        eventTypes.add(createEventType(MBusDataSourceRT.POINT_READ_EXCEPTION_EVENT, new LocalizableMessageImpl(
                "event.ds.pointRead")));
        eventTypes.add(createEventType(MBusDataSourceRT.POINT_WRITE_EXCEPTION_EVENT, new LocalizableMessageImpl(
                "event.ds.pointWrite")));
    }

    @Override
    public LocalizableMessage getConnectionDescription() {
        return new LocalizableMessageImpl("common.default", connection.getClass().getSimpleName());
    }

    @Override
    public PointLocatorVO createPointLocator() {
        return new MBusPointLocatorVO();
    }

    @Override
    public DataSourceRT createDataSourceRT() {
        return new MBusDataSourceRT(this);
    }

    @Override
    public ExportCodes getEventCodes() {
        return EVENT_CODES;
    }

    @Override
    protected void addPropertiesImpl(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.mbus.connection", connection);
        if (connection instanceof SerialPortConnection) {
        AuditEventType.addPropertyMessage(list, "dsEdit.mbus.keepSerialOpen", keepSerialPortOpen);
        }
        AuditEventType.addPropertyMessage(list, "dsEdit.cronPattern", cronPattern);
        AuditEventType.addPropertyMessage(list, "dsEdit.cronTimeZone", cronTimeZone);
    }

    @Override
    protected void addPropertyChangesImpl(List<LocalizableMessage> list, MBusDataSourceVO from) {
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.mbus.connection", from.connection, connection);
        if (connection instanceof SerialPortConnection) {
            AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.mbus.keepSerialOpen", from.keepSerialPortOpen, keepSerialPortOpen);
        }
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.cronPattern", from.cronPattern, cronPattern);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.cronTimeZone", from.cronTimeZone, cronTimeZone);
    }

    public String getCronPattern() {
        return cronPattern;
    }

    public void setCronPattern(String cronPattern) {
        this.cronPattern = cronPattern;
    }

    public String getCronTimeZone() {
        return cronTimeZone;
    }

    public void setCronTimeZone(String cronTimeZone) {
        this.cronTimeZone = cronTimeZone;
    }

    @Override
    public void validate(DwrResponseI18n response) {
        super.validate(response);

        if (connection == null) {
            response.addContextual("connection", "validate.required");
        }
        if (!TimeZone.getTimeZone(cronTimeZone).getID().equals(cronTimeZone)) {
            response.addContextual("cronTimeZone", "validate.invalidValue");
        }
        try {
            new CronParser().parse(cronPattern, TimeZone.getTimeZone(cronTimeZone));
        } catch (ParseException e) {
            response.addContextual("cronPatternh", "validate.invalidValue");
        }
    }
    //
    // /
    // / Serialization
    // /
    //
    private static final long serialVersionUID = -1;
    private static final int SERIAL_VERSION = 2;

    // Serialization for saveDataSource
    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(SERIAL_VERSION);
        out.writeObject(connection);
        out.writeBoolean(keepSerialPortOpen);
        out.writeObject(cronPattern);
        out.writeObject(cronTimeZone);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();
        try {
            switch (ver) {
                case 1:
                    connection = (Connection) in.readObject();
                    final int updatePeriodType = in.readInt();
                    final int updatePeriods = in.readInt();
                    break;
                case 2:
                    connection = (Connection) in.readObject();
                    keepSerialPortOpen = in.readBoolean();
                    cronPattern = (String) in.readObject();
                    cronTimeZone = (String) in.readObject();
            }
        } catch (ClassNotFoundException ex) {
            throw new RuntimeException(ex);
        }
    }

    @Override
    public void jsonDeserialize(JsonReader reader, JsonObject json) throws JsonException {
        super.jsonDeserialize(reader, json);
        JsonObject jsonConnection = null;
        if (!json.isNull("tcpConnection")) {
            LOG.severe("TCP FROM JSON");
            jsonConnection = json.getJsonObject("tcpConnection");
            TcpIpConnection tcpConnection = new TcpIpConnection();
            tcpConnection.setHost(jsonConnection.getString("host"));
            tcpConnection.setPort(jsonConnection.getInt("port"));
            connection = tcpConnection;
        } else {
            LOG.severe("NO TCP FROM JSON");
        }
        //TODO serial stuff
        connection.setBitPerSecond(jsonConnection.getInt("bitPerSecond"));
        connection.setResponseTimeOutOffset(jsonConnection.getInt("responseTimeOutOffset"));
    }

    @Override
    public void jsonSerialize(Map<String, Object> map) {
        super.jsonSerialize(map);
        Map<String, Object> connectionMap = new LinkedHashMap<>();
        connectionMap.put("bitPerSecond", connection.getBitPerSecond());
        connectionMap.put("responseTimeOutOffset", connection.getResponseTimeOutOffset());
        if (connection instanceof TcpIpConnection) {
            TcpIpConnection tcpConnection = (TcpIpConnection) connection;
            connectionMap.put("host", tcpConnection.getHost());
            connectionMap.put("port", tcpConnection.getPort());
            map.put("tcpConnection", connectionMap);
        }
        //TODO serial stuff
    }

    /**
     * Helper for JSP
     *
     * @return
     */
    public boolean isSerialDirect() {
        return SerialPortConnection.class.equals(connection.getClass());
    }

    /**
     * Helper for JSP
     *
     * @return
     */
    public boolean isTcpIp() {
        return TcpIpConnection.class.equals(connection.getClass());
    }

    public void setConnection(Connection connection) {
        this.connection = connection;
    }

    public Set<String> getCommPorts() {
        return SerialPortList.getPortNames(false);
    }

    public Connection getConnection() {
        return connection;
    }

    public String[] getLabels() {
        MBusMedium[] val = MBusMedium.values();
        String[] result = new String[val.length];
        for (int i = 0; i < val.length; i++) {
            result[i] = val[i].getLabel();
        }
        return result;
    }

    /**
     * @return the keepSerialPortOpen
     */
    public boolean isKeepSerialPortOpen() {
        return keepSerialPortOpen;
    }

    /**
     * @param keepSerialPortOpen the keepSerialPortOpen to set
     */
    public void setKeepSerialPortOpen(boolean keepSerialPortOpen) {
        this.keepSerialPortOpen = keepSerialPortOpen;
    }

}
