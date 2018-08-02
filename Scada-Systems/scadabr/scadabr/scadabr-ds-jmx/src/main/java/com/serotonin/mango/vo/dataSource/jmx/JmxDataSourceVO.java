/*
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
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
package com.serotonin.mango.vo.dataSource.jmx;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;
import java.util.Map;

import br.org.scadabr.json.JsonException;
import br.org.scadabr.json.JsonObject;
import br.org.scadabr.json.JsonReader;


import br.org.scadabr.rt.event.type.DuplicateHandling;
import com.serotonin.mango.Common;
import com.serotonin.mango.rt.dataSource.DataSourceRT;
import com.serotonin.mango.rt.dataSource.jmx.JmxDataSourceRT;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.rt.event.type.EventType;
import com.serotonin.mango.util.ExportCodes;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.event.EventTypeVO;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.utils.TimePeriods;
import br.org.scadabr.vo.event.AlarmLevel;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;

/**
 * @author Matthew Lohbihler
 */

public class JmxDataSourceVO extends DataSourceVO<JmxDataSourceVO> {

    public static final Type TYPE = Type.JMX;

    @Override
    protected void addEventTypes(List<EventTypeVO> ets) {
        ets.add(createEventType(JmxDataSourceRT.DATA_SOURCE_EXCEPTION_EVENT, new LocalizableMessageImpl(
                "event.ds.dataSource"), DuplicateHandling.IGNORE_SAME_MESSAGE, AlarmLevel.URGENT));
        ets.add(createEventType(JmxDataSourceRT.POINT_READ_EXCEPTION_EVENT,
                new LocalizableMessageImpl("event.ds.pointRead")));
        ets.add(createEventType(JmxDataSourceRT.POINT_WRITE_EXCEPTION_EVENT, new LocalizableMessageImpl(
                "event.ds.pointWrite")));
    }

    private static final ExportCodes EVENT_CODES = new ExportCodes();

    static {
        EVENT_CODES.addElement(JmxDataSourceRT.DATA_SOURCE_EXCEPTION_EVENT, "DATA_SOURCE_EXCEPTION");
        EVENT_CODES.addElement(JmxDataSourceRT.POINT_READ_EXCEPTION_EVENT, "POINT_READ_EXCEPTION");
        EVENT_CODES.addElement(JmxDataSourceRT.POINT_WRITE_EXCEPTION_EVENT, "POINT_WRITE_EXCEPTION");
    }

    @Override
    public ExportCodes getEventCodes() {
        return EVENT_CODES;
    }

    @Override
    public Type getType() {
        return TYPE;
    }

    @Override
    public LocalizableMessage getConnectionDescription() {
        if (useLocalServer) {
            return new LocalizableMessageImpl("dsEdit.jmx.dsconn.local");
        }
        return new LocalizableMessageImpl("dsEdit.jmx.dsconn.remote", remoteServerAddr);
    }

    @Override
    public DataSourceRT createDataSourceRT() {
        return new JmxDataSourceRT(this);
    }

    @Override
    public JmxPointLocatorVO createPointLocator() {
        return new JmxPointLocatorVO();
    }

    
    private boolean useLocalServer;
    
    private String remoteServerAddr;
    private TimePeriods updatePeriodType = TimePeriods.MINUTES;
    
    private int updatePeriods = 5;
    
    private boolean quantize;

    public boolean isUseLocalServer() {
        return useLocalServer;
    }

    public void setUseLocalServer(boolean useLocalServer) {
        this.useLocalServer = useLocalServer;
    }

    public String getRemoteServerAddr() {
        return remoteServerAddr;
    }

    public void setRemoteServerAddr(String remoteServerAddr) {
        this.remoteServerAddr = remoteServerAddr;
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

    public boolean isQuantize() {
        return quantize;
    }

    public void setQuantize(boolean quantize) {
        this.quantize = quantize;
    }

    @Override
    public void validate(DwrResponseI18n response) {
        super.validate(response);

        if (!useLocalServer && remoteServerAddr.isEmpty()) {
            response.addContextual("remoteServerAddr", "validate.required");
        }
        if (updatePeriods <= 0) {
            response.addContextual("updatePeriods", "validate.greaterThanZero");
        }
    }

    @Override
    protected void addPropertiesImpl(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.jmx.useLocalServer", useLocalServer);
        AuditEventType.addPropertyMessage(list, "dsEdit.jmx.remoteServerAddr", remoteServerAddr);
        AuditEventType.addPropertyMessage(list, "dsEdit.updatePeriod", updatePeriodType.getPeriodDescription(updatePeriods));
        AuditEventType.addPropertyMessage(list, "dsEdit.quantize", quantize);
    }

    @Override
    protected void addPropertyChangesImpl(List<LocalizableMessage> list, JmxDataSourceVO from) {
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.jmx.useLocalServer", from.useLocalServer,
                useLocalServer);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.jmx.remoteServerAddr", from.remoteServerAddr,
                remoteServerAddr);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.updatePeriod",
                from.updatePeriodType.getPeriodDescription(from.updatePeriods),
                updatePeriodType.getPeriodDescription(updatePeriods));
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.quantize", from.quantize, quantize);
    }

    //
    //
    // Serialization
    //
    private static final long serialVersionUID = -1;
    private static final int version = 1;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(version);
        out.writeBoolean(useLocalServer);
        SerializationHelper.writeSafeUTF(out, remoteServerAddr);
        out.writeInt(updatePeriodType.mangoDbId);
        out.writeInt(updatePeriods);
        out.writeBoolean(quantize);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            useLocalServer = in.readBoolean();
            remoteServerAddr = SerializationHelper.readSafeUTF(in);
            updatePeriodType = TimePeriods.fromMangoDbId(in.readInt());
            updatePeriods = in.readInt();
            quantize = in.readBoolean();
        }
    }

    @Override
    public void jsonDeserialize(JsonReader reader, JsonObject json) throws JsonException {
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
