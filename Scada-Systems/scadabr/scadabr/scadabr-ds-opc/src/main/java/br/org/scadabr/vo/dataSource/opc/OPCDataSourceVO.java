package br.org.scadabr.vo.dataSource.opc;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;
import java.util.Map;

import br.org.scadabr.rt.dataSource.opc.OPCDataSource;

import br.org.scadabr.json.JsonException;
import br.org.scadabr.json.JsonObject;
import br.org.scadabr.json.JsonReader;


import com.serotonin.mango.Common;
import com.serotonin.mango.rt.dataSource.DataSourceRT;
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


public class OPCDataSourceVO extends DataSourceVO<OPCDataSourceVO> {

    public static final Type TYPE = Type.OPC;

    @Override
    protected void addEventTypes(List<EventTypeVO> eventTypes) {
        eventTypes.add(createEventType(
                OPCDataSource.POINT_READ_EXCEPTION_EVENT,
                new LocalizableMessageImpl("event.ds.pointRead")));
        eventTypes.add(createEventType(
                OPCDataSource.DATA_SOURCE_EXCEPTION_EVENT,
                new LocalizableMessageImpl("event.ds.dataSource")));
        eventTypes.add(createEventType(
                OPCDataSource.POINT_WRITE_EXCEPTION_EVENT,
                new LocalizableMessageImpl("event.ds.dataSource")));

    }

    private static final ExportCodes EVENT_CODES = new ExportCodes();

    static {
        EVENT_CODES.addElement(OPCDataSource.DATA_SOURCE_EXCEPTION_EVENT,
                "DATA_SOURCE_EXCEPTION");
        EVENT_CODES.addElement(OPCDataSource.POINT_READ_EXCEPTION_EVENT,
                "POINT_READ_EXCEPTION");
        EVENT_CODES.addElement(OPCDataSource.POINT_WRITE_EXCEPTION_EVENT,
                "POINT_WRITE_EXCEPTION");

    }

    @Override
    public DataSourceRT createDataSourceRT() {
        return new OPCDataSource(this);
    }

    @Override
    public PointLocatorVO createPointLocator() {
        return new OPCPointLocatorVO();
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
    
    private String host = "localhost";
    
    private String domain = "localhost";
    
    private String user = "";
    
    private String password = "";
    
    private String server = "";
    
    private boolean quantize;
    
    private int creationMode;

    public int getCreationMode() {
        return creationMode;
    }

    public void setCreationMode(int creationMode) {
        this.creationMode = creationMode;
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

    public String getHost() {
        return host;
    }

    public void setHost(String host) {
        this.host = host;
    }

    public String getDomain() {
        return domain;
    }

    public void setDomain(String domain) {
        this.domain = domain;
    }

    public String getUser() {
        return user;
    }

    public void setUser(String user) {
        this.user = user;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public String getServer() {
        return server;
    }

    public void setServer(String server) {
        this.server = server;
    }

    public boolean isQuantize() {
        return quantize;
    }

    public void setQuantize(boolean quantize) {
        this.quantize = quantize;
    }

    @Override
    public void validate(DwrResponseI18n response) {
        super.validate(response);
        if (host.isEmpty()) {
            response.addContextual("host", "validate.required");
        }
        // if (StringUtils.isEmpty(domain))
        // response.addContextualMessage("domain", "validate.required");
        if (user.isEmpty()) {
            response.addContextual("user", "validate.required");
        }
        if (password.isEmpty()) {
            response.addContextual("password", "validate.required");
        }
        if (server.isEmpty()) {
            response.addContextual("server", "validate.required");
        }
        if (updatePeriods <= 0) {
            response.addContextual("updatePeriods", "validate.greaterThanZero");
        }
    }

    @Override
    protected void addPropertiesImpl(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.dnp3.rbePeriod",
                updatePeriodType.getPeriodDescription(updatePeriods));
        AuditEventType.addPropertyMessage(list, "dsEdit.opc.host", host);
        AuditEventType.addPropertyMessage(list, "dsEdit.opc.domain", domain);
        AuditEventType.addPropertyMessage(list, "dsEdit.opc.user", user);
        AuditEventType
                .addPropertyMessage(list, "dsEdit.opc.password", password);
        AuditEventType.addPropertyMessage(list, "dsEdit.opc.server", server);

        AuditEventType.addPropertyMessage(list, "dsEdit.quantize", quantize);
        AuditEventType.addPropertyMessage(list, "dsEdit.opc.creationMode",
                creationMode);
    }

    @Override
    protected void addPropertyChangesImpl(List<LocalizableMessage> list, OPCDataSourceVO from) {
        final OPCDataSourceVO fromVO = (OPCDataSourceVO) from;
        AuditEventType.maybeAddPropertyChangeMessage(list,
                "dsEdit.dnp3.rbePeriod",
                fromVO.updatePeriodType.getPeriodDescription(fromVO.updatePeriods),
                updatePeriodType.getPeriodDescription(updatePeriods));
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.opc.host",
                fromVO.host, host);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.opc.domain",
                fromVO.domain, domain);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.opc.user",
                fromVO.user, user);
        AuditEventType.maybeAddPropertyChangeMessage(list,
                "dsEdit.opc.password", fromVO.password, password);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.opc.server",
                fromVO.server, server);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.quantize",
                fromVO.quantize, quantize);
        AuditEventType.maybeAddPropertyChangeMessage(list,
                "dsEdit.opc.creationMode", fromVO.creationMode, creationMode);
    }

    //
    // /
    // / Serialization
    // /
    //
    private static final long serialVersionUID = -1;
    private static final int version = 1;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(version);
        SerializationHelper.writeSafeUTF(out, host);
        SerializationHelper.writeSafeUTF(out, domain);
        SerializationHelper.writeSafeUTF(out, user);
        SerializationHelper.writeSafeUTF(out, password);
        SerializationHelper.writeSafeUTF(out, server);
        out.writeInt(updatePeriodType.mangoDbId);
        out.writeInt(updatePeriods);
        out.writeBoolean(quantize);
        out.writeInt(creationMode);

    }

    private void readObject(ObjectInputStream in) throws IOException,
            ClassNotFoundException {
        int ver = in.readInt();
        if (ver == 1) {
            host = SerializationHelper.readSafeUTF(in);
            domain = SerializationHelper.readSafeUTF(in);
            user = SerializationHelper.readSafeUTF(in);
            password = SerializationHelper.readSafeUTF(in);
            server = SerializationHelper.readSafeUTF(in);

            updatePeriodType = TimePeriods.fromMangoDbId(in.readInt());
            updatePeriods = in.readInt();
            quantize = in.readBoolean();
            creationMode = in.readInt();
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
