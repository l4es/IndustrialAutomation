package br.org.scadabr.vo.dataSource.alpha2;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;
import java.util.Map;

import br.org.scadabr.rt.dataSource.alpha2.Alpha2DataSource;

import br.org.scadabr.json.JsonException;
import br.org.scadabr.json.JsonObject;
import br.org.scadabr.json.JsonReader;


import com.serotonin.mango.Common;
import com.serotonin.mango.rt.dataSource.DataSourceRT;
import com.serotonin.mango.rt.dataSource.modbus.ModbusDataSource;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.util.ExportCodes;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.event.EventTypeVO;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.utils.TimePeriods;
import br.org.scadabr.vo.dataSource.PointLocatorVO;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;


public class Alpha2DataSourceVO extends DataSourceVO<Alpha2DataSourceVO> {

    public static final Type TYPE = Type.ALPHA_2;

    private TimePeriods updatePeriodType = TimePeriods.SECONDS;
    
    private int updatePeriods = 1;
    
    private String commPortId;
    
    private int baudRate = 9600;
    
    private int dataBits = 8;
    
    private int stopBits = 1;
    
    private int parity = 0;
    
    private int timeout = 300;
    
    private int retries = 2;
    
    private int station = 0;

    @Override
    public com.serotonin.mango.vo.dataSource.DataSourceVO.Type getType() {
        return TYPE;
    }

    @Override
    public LocalizableMessage getConnectionDescription() {
        return new LocalizableMessageImpl("common.default", commPortId);
    }

    @Override
    public PointLocatorVO createPointLocator() {
        return new Alpha2PointLocatorVO();
    }

    @Override
    public DataSourceRT createDataSourceRT() {
        return new Alpha2DataSource(this);
    }

    @Override
    public ExportCodes getEventCodes() {
        return EVENT_CODES;
    }

    @Override
    protected void addPropertiesImpl(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.dnp3Serial.port",
                commPortId);
        AuditEventType.addPropertyMessage(list, "dsEdit.dnp3Serial.baud",
                baudRate);
        AuditEventType.addPropertyMessage(list, "dsEdit.dnp3Serial.dataBits",
                dataBits);
        AuditEventType.addPropertyMessage(list, "dsEdit.dnp3Serial.stopBits",
                stopBits);
        AuditEventType.addPropertyMessage(list, "dsEdit.dnp3Serial.parity",
                parity);
        AuditEventType.addPropertyMessage(list, "dsEdit.alpha2.station",
                station);

    }

    @Override
    protected void addPropertyChangesImpl(List<LocalizableMessage> list, Alpha2DataSourceVO from) {
        final Alpha2DataSourceVO fromVO = (Alpha2DataSourceVO) from;
        AuditEventType.maybeAddPropertyChangeMessage(list,
                "dsEdit.dnp3Serial.port", fromVO.commPortId, commPortId);
        AuditEventType.maybeAddPropertyChangeMessage(list,
                "dsEdit.dnp3Serial.baud", fromVO.baudRate, baudRate);
        AuditEventType.maybeAddPropertyChangeMessage(list,
                "dsEdit.dnp3Serial.dataBits", fromVO.dataBits, dataBits);
        AuditEventType.maybeAddPropertyChangeMessage(list,
                "dsEdit.dnp3Serial.stopBits", fromVO.stopBits, stopBits);
        AuditEventType.maybeAddPropertyChangeMessage(list,
                "dsEdit.dnp3Serial.parity", fromVO.parity, parity);
        AuditEventType.maybeAddPropertyChangeMessage(list,
                "dsEdit.alpha2.station", fromVO.station, station);

    }

    @Override
    public void validate(DwrResponseI18n response) {
        super.validate(response);
        if (station < 0) {
            response.addContextual("station", "validate.invalidValue");
        }
        if (timeout <= 0) {
            response.addContextual("timeout", "validate.greaterThanZero");
        }
        if (retries < 0) {
            response.addContextual("retries", "validate.cannotBeNegative");
        }
    }

    private static final long serialVersionUID = -1;
    private static final int version = 1;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(version);
        out.writeInt(updatePeriodType.mangoDbId);
        out.writeInt(updatePeriods);
        SerializationHelper.writeSafeUTF(out, commPortId);
        out.writeInt(baudRate);
        out.writeInt(stopBits);
        out.writeInt(dataBits);
        out.writeInt(parity);
        out.writeInt(timeout);
        out.writeInt(retries);
        out.writeInt(station);

    }

    private void readObject(ObjectInputStream in) throws IOException,
            ClassNotFoundException {
        int ver = in.readInt();
        if (ver == 1) {
            updatePeriodType = TimePeriods.fromMangoDbId(in.readInt());
            updatePeriods = in.readInt();
            commPortId = SerializationHelper.readSafeUTF(in);
            baudRate = in.readInt();
            stopBits = in.readInt();
            dataBits = in.readInt();
            parity = in.readInt();
            timeout = in.readInt();
            retries = in.readInt();
            station = in.readInt();
        }
    }

    @Override
    public void jsonDeserialize(JsonReader reader, JsonObject json)
            throws JsonException {
        super.jsonDeserialize(reader, json);
        TimePeriods value = deserializeUpdatePeriodType(json);
        if (value != null) {
            updatePeriodType = value;
        }
    }

    @Override
    public void jsonSerialize(Map<String, Object> map) {
        super.jsonSerialize(map);
        serializeUpdatePeriodType(map, updatePeriodType);
    }

    public TimePeriods getUpdatePeriodType() {
        return updatePeriodType;
    }

    public void setUpdatePeriodType(TimePeriods updatePeriodType) {
        this.updatePeriodType = updatePeriodType;
    }

    public int getUpdatePeriods() {
        return updatePeriods;
    }

    public void setUpdatePeriods(int updatePeriods) {
        this.updatePeriods = updatePeriods;
    }

    public String getCommPortId() {
        return commPortId;
    }

    public void setCommPortId(String commPortId) {
        this.commPortId = commPortId;
    }

    public int getBaudRate() {
        return baudRate;
    }

    public void setBaudRate(int baudRate) {
        this.baudRate = baudRate;
    }

    public int getDataBits() {
        return dataBits;
    }

    public void setDataBits(int dataBits) {
        this.dataBits = dataBits;
    }

    public int getStopBits() {
        return stopBits;
    }

    public void setStopBits(int stopBits) {
        this.stopBits = stopBits;
    }

    public int getParity() {
        return parity;
    }

    public void setParity(int parity) {
        this.parity = parity;
    }

    public int getTimeout() {
        return timeout;
    }

    public void setTimeout(int timeout) {
        this.timeout = timeout;
    }

    public int getRetries() {
        return retries;
    }

    public void setRetries(int retries) {
        this.retries = retries;
    }

    public int getStation() {
        return station;
    }

    public void setStation(int station) {
        this.station = station;
    }

    @Override
    protected void addEventTypes(List<EventTypeVO> ets) {
        ets.add(createEventType(ModbusDataSource.DATA_SOURCE_EXCEPTION_EVENT,
                new LocalizableMessageImpl("event.ds.dataSource")));
        ets.add(createEventType(ModbusDataSource.POINT_READ_EXCEPTION_EVENT,
                new LocalizableMessageImpl("event.ds.pointRead")));
        ets.add(createEventType(ModbusDataSource.POINT_WRITE_EXCEPTION_EVENT,
                new LocalizableMessageImpl("event.ds.pointWrite")));
    }

    private static final ExportCodes EVENT_CODES = new ExportCodes();

    static {
        EVENT_CODES.addElement(ModbusDataSource.DATA_SOURCE_EXCEPTION_EVENT,
                "DATA_SOURCE_EXCEPTION");
        EVENT_CODES.addElement(ModbusDataSource.POINT_READ_EXCEPTION_EVENT,
                "POINT_READ_EXCEPTION");
        EVENT_CODES.addElement(ModbusDataSource.POINT_WRITE_EXCEPTION_EVENT,
                "POINT_WRITE_EXCEPTION");
    }

}
