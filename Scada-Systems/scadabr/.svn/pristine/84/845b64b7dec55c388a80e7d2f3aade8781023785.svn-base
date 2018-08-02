package br.org.scadabr.vo.dataSource.drStorageHt5b;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;
import java.util.Map;

import br.org.scadabr.rt.dataSource.drStorageHt5b.DrStorageHt5bDataSource;

import br.org.scadabr.json.JsonException;
import br.org.scadabr.json.JsonObject;
import br.org.scadabr.json.JsonReader;


import com.serotonin.mango.Common;
import com.serotonin.mango.rt.dataSource.DataSourceRT;
import com.serotonin.mango.util.ExportCodes;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.event.EventTypeVO;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.utils.TimePeriods;
import br.org.scadabr.vo.dataSource.PointLocatorVO;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;


public class DrStorageHt5bDataSourceVO extends DataSourceVO<DrStorageHt5bDataSourceVO> {

    public static final Type TYPE = Type.DR_STORAGE_HT5B;

    @Override
    protected void addEventTypes(List<EventTypeVO> eventTypes) {
        eventTypes.add(createEventType(
                DrStorageHt5bDataSource.POINT_READ_EXCEPTION_EVENT,
                new LocalizableMessageImpl("event.ds.pointRead")));
        eventTypes.add(createEventType(
                DrStorageHt5bDataSource.DATA_SOURCE_EXCEPTION_EVENT,
                new LocalizableMessageImpl("event.ds.dataSource")));
    }

    private static final ExportCodes EVENT_CODES = new ExportCodes();

    static {
        EVENT_CODES.addElement(
                DrStorageHt5bDataSource.DATA_SOURCE_EXCEPTION_EVENT,
                "DATA_SOURCE_EXCEPTION");
        EVENT_CODES.addElement(
                DrStorageHt5bDataSource.POINT_READ_EXCEPTION_EVENT,
                "POINT_READ_EXCEPTION");
    }

    @Override
    protected void addPropertiesImpl(List<LocalizableMessage> list) {
        // TODO Auto-generated method stub

    }

    @Override
    protected void addPropertyChangesImpl(List<LocalizableMessage> list, DrStorageHt5bDataSourceVO from) {
        // TODO Auto-generated method stub

    }

    @Override
    public DataSourceRT createDataSourceRT() {
        return new DrStorageHt5bDataSource(this);
    }

    @Override
    public PointLocatorVO createPointLocator() {
        return new DrStorageHt5bPointLocatorVO();
    }

    @Override
    public LocalizableMessage getConnectionDescription() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public ExportCodes getEventCodes() {
        return EVENT_CODES;
    }

    @Override
    public com.serotonin.mango.vo.dataSource.DataSourceVO.Type getType() {
        return TYPE;
    }

    private TimePeriods updatePeriodType = TimePeriods.SECONDS;
    
    private int updatePeriods = 1;
    
    private String commPortId;
    
    private int baudRate = 9600;
    
    private int dataBits = 8;
    
    private int stopBits = 1;
    
    private int parity = 0;
    
    private int timeout = 300;
    
    private int retries = 2;
    
    private String initString = "";
    
    private boolean quantize;

    
    @Override
    public void validate(DwrResponseI18n response) {
        super.validate(response);
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

    public String getInitString() {
        return initString;
    }

    public void setInitString(String initString) {
        this.initString = initString;
    }

    public boolean isQuantize() {
        return quantize;
    }

    public void setQuantize(boolean quantize) {
        this.quantize = quantize;
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
        out.writeBoolean(quantize);

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
            quantize = in.readBoolean();
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
}
