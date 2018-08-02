package br.org.scadabr.vo.dataSource.dnp3;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;
import java.util.Map;

import br.org.scadabr.rt.dataSource.dnp3.Dnp3DataSource;

import br.org.scadabr.json.JsonException;
import br.org.scadabr.json.JsonObject;
import br.org.scadabr.json.JsonReader;

import br.org.scadabr.utils.TimePeriods;
import br.org.scadabr.vo.dataSource.PointLocatorVO;
import com.serotonin.mango.Common;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.util.ExportCodes;
import com.serotonin.mango.util.LocalizableJsonException;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.event.EventTypeVO;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;

abstract public class Dnp3DataSourceVO<T extends Dnp3DataSourceVO<T>> extends DataSourceVO<T> {

    @Override
    protected void addEventTypes(List<EventTypeVO> eventTypes) {
        eventTypes.add(createEventType(
                Dnp3DataSource.POINT_READ_EXCEPTION_EVENT,
                new LocalizableMessageImpl("event.ds.pointRead")));
        eventTypes.add(createEventType(
                Dnp3DataSource.DATA_SOURCE_EXCEPTION_EVENT,
                new LocalizableMessageImpl("event.ds.dataSource")));
    }

    private static final ExportCodes EVENT_CODES = new ExportCodes();

    static {
        EVENT_CODES.addElement(Dnp3DataSource.DATA_SOURCE_EXCEPTION_EVENT,
                "DATA_SOURCE_EXCEPTION");
        EVENT_CODES.addElement(Dnp3DataSource.POINT_READ_EXCEPTION_EVENT,
                "POINT_READ_EXCEPTION");
    }

    @Override
    public ExportCodes getEventCodes() {
        return EVENT_CODES;
    }

    @Override
    public PointLocatorVO createPointLocator() {
        return new Dnp3PointLocatorVO();
    }

    
    private int synchPeriods = 20;
    
    private int staticPollPeriods = 30;
    private TimePeriods rbePeriodType = TimePeriods.SECONDS;
    
    private int rbePollPeriods = 1;
    
    private boolean quantize;
    
    private int timeout = 800;
    
    private int retries = 2;
    
    private int sourceAddress = 1;
    
    private int slaveAddress = 2;

    public int getSynchPeriods() {
        return synchPeriods;
    }

    public void setSynchPeriods(int synchPeriods) {
        this.synchPeriods = synchPeriods;
    }

    public int getStaticPollPeriods() {
        return staticPollPeriods;
    }

    public void setStaticPollPeriods(int staticPollPeriods) {
        this.staticPollPeriods = staticPollPeriods;
    }

    public TimePeriods getRbePeriodType() {
        return rbePeriodType;
    }

    public void setRbePeriodType(TimePeriods rbePeriodType) {
        this.rbePeriodType = rbePeriodType;
    }

    public int getRbePollPeriods() {
        return rbePollPeriods;
    }

    public void setRbePollPeriods(int rbePollPeriods) {
        this.rbePollPeriods = rbePollPeriods;
    }

    public boolean isQuantize() {
        return quantize;
    }

    public void setQuantize(boolean quantize) {
        this.quantize = quantize;
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

    public int getSourceAddress() {
        return sourceAddress;
    }

    public void setSourceAddress(int sourceAddress) {
        this.sourceAddress = sourceAddress;
    }

    public int getSlaveAddress() {
        return slaveAddress;
    }

    public void setSlaveAddress(int slaveAddress) {
        this.slaveAddress = slaveAddress;
    }

    @Override
    public void validate(DwrResponseI18n response) {
        super.validate(response);
        if (synchPeriods <= 0) {
            response.addContextual("synchPeriods", "validate.greaterThanZero");
        }
        if (sourceAddress <= 0) {
            response.addContextual("sourceAddress", "validate.greaterThanZero");
        }
        if (slaveAddress <= 0) {
            response.addContextual("slaveAddress", "validate.greaterThanZero");
        }
        if (staticPollPeriods <= 0) {
            response.addContextual("staticPollPeriods", "validate.greaterThanZero");
        }
        if (rbePollPeriods <= 0) {
            response.addContextual("rbePollPeriods", "validate.greaterThanZero");
        }
        if (timeout <= 0) {
            response
                    .addContextual("timeout", "validate.greaterThanZero");
        }
        if (retries < 0) {
            response.addContextual("retries", "validate.cannotBeNegative");
        }
    }

    @Override
    protected void addPropertiesImpl(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.dnp3.rbePeriod",
                rbePeriodType.getPeriodDescription(rbePollPeriods));
        AuditEventType.addPropertyMessage(list, "dsEdit.dnp3.synchPeriod",
                synchPeriods);
        AuditEventType.addPropertyMessage(list, "dsEdit.dnp3.staticPeriod",
                staticPollPeriods);
        AuditEventType.addPropertyMessage(list, "dsEdit.dnp3.sourceAddress",
                sourceAddress);
        AuditEventType.addPropertyMessage(list, "dsEdit.dnp3.slaveAddress",
                slaveAddress);
        AuditEventType.addPropertyMessage(list, "dsEdit.quantize", quantize);
        AuditEventType.addPropertyMessage(list, "dsEdit.modbus.timeout",
                timeout);
        AuditEventType.addPropertyMessage(list, "dsEdit.modbus.retries",
                retries);
    }

    @Override
    protected void addPropertyChangesImpl(List<LocalizableMessage> list, T from) {
        final Dnp3DataSourceVO fromVO = (Dnp3DataSourceVO) from;
        AuditEventType.maybeAddPropertyChangeMessage(list,
                "dsEdit.dnp3.rbePeriod",
                fromVO.rbePeriodType.getPeriodDescription(fromVO.rbePollPeriods),
                rbePeriodType.getPeriodDescription(rbePollPeriods));
        AuditEventType.maybeAddPropertyChangeMessage(list,
                "dsEdit.dnp3.synchPeriod", fromVO.synchPeriods, synchPeriods);
        AuditEventType.maybeAddPropertyChangeMessage(list,
                "dsEdit.dnp3.staticPeriod", fromVO.staticPollPeriods,
                staticPollPeriods);
        AuditEventType.maybeAddPropertyChangeMessage(list,
                "dsEdit.dnp3.sourceAddress", fromVO.sourceAddress, sourceAddress);
        AuditEventType.maybeAddPropertyChangeMessage(list,
                "dsEdit.dnp3.slaveAddress", fromVO.slaveAddress, slaveAddress);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.quantize",
                fromVO.quantize, quantize);
        AuditEventType.maybeAddPropertyChangeMessage(list,
                "dsEdit.modbus.timeout", fromVO.timeout, timeout);
        AuditEventType.maybeAddPropertyChangeMessage(list,
                "dsEdit.modbus.retries", fromVO.retries, retries);
    }

    private static final long serialVersionUID = -1;
    private static final int version = 1;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(version);
        out.writeInt(synchPeriods);
        out.writeInt(staticPollPeriods);
        out.writeInt(rbePeriodType.mangoDbId);
        out.writeInt(rbePollPeriods);
        out.writeBoolean(quantize);
        out.writeInt(timeout);
        out.writeInt(retries);
        out.writeInt(sourceAddress);
        out.writeInt(slaveAddress);
    }

    private void readObject(ObjectInputStream in) throws IOException,
            ClassNotFoundException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be
        // elegantly handled.
        if (ver == 1) {
            synchPeriods = in.readInt();
            // staticPeriodType = in.readInt();
            staticPollPeriods = in.readInt();
            rbePeriodType = TimePeriods.fromMangoDbId(in.readInt());
            rbePollPeriods = in.readInt();
            quantize = in.readBoolean();
            timeout = in.readInt();
            retries = in.readInt();
            sourceAddress = in.readInt();
            slaveAddress = in.readInt();
        }
    }

    @Override
    public void jsonDeserialize(JsonReader reader, JsonObject json)
            throws JsonException {
        super.jsonDeserialize(reader, json);
        rbePeriodType = deserializePeriodType(json, "eventsPeriodType");
    }

    @Override
    public void jsonSerialize(Map<String, Object> map) {
        super.jsonSerialize(map);
        serializePeriodType(map, rbePeriodType, "eventsPeriodType");
    }

    protected void serializePeriodType(Map<String, Object> map,
            TimePeriods updatePeriodType, String name) {
        map.put(name, updatePeriodType.name());
    }

    protected TimePeriods deserializePeriodType(JsonObject json, String name)
            throws JsonException {
        String text = json.getString(name);
        if (text == null) {
            return null;
        }
        try {
            return TimePeriods.valueOf(text);
        } catch (Exception e) {
            throw new LocalizableJsonException("emport.error.invalid", name,
                    text, TimePeriods.values());
        }
    }

}
