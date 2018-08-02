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
package com.serotonin.mango.vo.dataSource.spinwave;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.Arrays;
import java.util.List;



import com.serotonin.mango.rt.dataSource.DataSourceRT;
import com.serotonin.mango.rt.dataSource.spinwave.SpinwaveDataSourceRT;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.util.ExportCodes;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.event.EventTypeVO;
import com.serotonin.spinwave.SpinwaveReceiver;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.util.StringUtils;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;

/**
 * @author Matthew Lohbihler
 */

public class SpinwaveDataSourceVO extends DataSourceVO<SpinwaveDataSourceVO> {

    public static final Type TYPE = Type.SPINWAVE;

    @Override
    protected void addEventTypes(List<EventTypeVO> ets) {
        ets.add(createEventType(SpinwaveDataSourceRT.DATA_SOURCE_EXCEPTION_EVENT, new LocalizableMessageImpl(
                "event.ds.dataSource")));
        ets.add(createEventType(SpinwaveDataSourceRT.SENSOR_HEARTBEAT_EVENT, new LocalizableMessageImpl(
                "event.ds.heartbeat")));
        ets.add(createEventType(SpinwaveDataSourceRT.UNKNOWN_SENSOR_EVENT, new LocalizableMessageImpl(
                "event.ds.unknownSensor")));
    }

    private static final ExportCodes EVENT_CODES = new ExportCodes();

    static {
        EVENT_CODES.addElement(SpinwaveDataSourceRT.DATA_SOURCE_EXCEPTION_EVENT, "DATA_SOURCE_EXCEPTION");
        EVENT_CODES.addElement(SpinwaveDataSourceRT.SENSOR_HEARTBEAT_EVENT, "SENSOR_HEARTBEAT");
        EVENT_CODES.addElement(SpinwaveDataSourceRT.UNKNOWN_SENSOR_EVENT, "UNKNOWN_SENSOR");
    }

    @Override
    public ExportCodes getEventCodes() {
        return EVENT_CODES;
    }

    @Override
    public LocalizableMessage getConnectionDescription() {
        return new LocalizableMessageImpl("common.default", commPortId);
    }

    @Override
    public Type getType() {
        return TYPE;
    }

    @Override
    public DataSourceRT createDataSourceRT() {
        return new SpinwaveDataSourceRT(this);
    }

    @Override
    public BaseSpinwavePointLocatorVO createPointLocator() {
        if (messageVersion == SpinwaveReceiver.VERSION_2) {
            return new SpinwaveV2PointLocatorVO();
        }
        return new SpinwaveV1PointLocatorVO();
    }

    
    private int messageVersion;
    
    private String commPortId;
    
    private long[] sensorAddresses = new long[0];
    
    private int heartbeatTimeout = 30;

    public String getCommPortId() {
        return commPortId;
    }

    public void setCommPortId(String commPortId) {
        this.commPortId = commPortId;
    }

    public long[] getSensorAddresses() {
        return sensorAddresses;
    }

    public void setSensorAddresses(long[] sensorAddresses) {
        this.sensorAddresses = sensorAddresses;
    }

    public int getMessageVersion() {
        return messageVersion;
    }

    public void setMessageVersion(int messageVersion) {
        this.messageVersion = messageVersion;
    }

    public int getHeartbeatTimeout() {
        return heartbeatTimeout;
    }

    public void setHeartbeatTimeout(int heartbeatTimeout) {
        this.heartbeatTimeout = heartbeatTimeout;
    }

    @Override
    public void validate(DwrResponseI18n response) {
        super.validate(response);
        if (messageVersion != SpinwaveReceiver.VERSION_1 && messageVersion != SpinwaveReceiver.VERSION_2) {
            response.addContextual("messageVersion", "validate.invalidValue");
        }
        if (commPortId.isEmpty()) {
            response.addContextual("commPortId", "validate.required");
        }
        for (long addr : sensorAddresses) {
            if (addr <= 0) {
                response.addContextual("sensorAddresses", "validate.invalidAddress", addr);
            }
        }
        if (heartbeatTimeout < 30) {
            response.addContextual("heartbeatTimeout", "validate.notLessThan30s");
        }
    }

    @Override
    protected void addPropertiesImpl(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.spinwave.port", commPortId);
        AuditEventType.addPropertyMessage(list, "dsEdit.spinwave.heartbeatTimeout", heartbeatTimeout);
        AuditEventType.addPropertyMessage(list, "dsEdit.spinwave.version", messageVersion);
        AuditEventType.addPropertyMessage(list, "dsEdit.spinwave.sensorAddresses", Arrays.toString(sensorAddresses));
    }

    @Override
    protected void addPropertyChangesImpl(List<LocalizableMessage> list, SpinwaveDataSourceVO from) {
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.spinwave.port", from.commPortId, commPortId);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.spinwave.heartbeatTimeout", from.heartbeatTimeout,
                heartbeatTimeout);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.spinwave.version", from.messageVersion,
                messageVersion);
        if (Arrays.equals(from.sensorAddresses, sensorAddresses)) {
            AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.spinwave.sensorAddresses", Arrays
                    .toString(from.sensorAddresses), Arrays.toString(sensorAddresses));
        }
    }

    //
    // /
    // / Serialization
    // /
    //
    private static final long serialVersionUID = -1;
    private static final int version = 4;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(version);
        SerializationHelper.writeSafeUTF(out, commPortId);
        out.writeObject(sensorAddresses);
        out.writeInt(messageVersion);
        out.writeInt(heartbeatTimeout);
    }

    private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            commPortId = SerializationHelper.readSafeUTF(in);
            int[] old = (int[]) in.readObject();
            sensorAddresses = new long[old.length];
            for (int i = 0; i < old.length; i++) {
                sensorAddresses[i] = old[i];
            }
            messageVersion = SpinwaveReceiver.VERSION_1;
            heartbeatTimeout = 30;
        } else if (ver == 2) {
            commPortId = SerializationHelper.readSafeUTF(in);
            sensorAddresses = (long[]) in.readObject();
            messageVersion = SpinwaveReceiver.VERSION_1;
            heartbeatTimeout = 30;
        } else if (ver == 3) {
            commPortId = SerializationHelper.readSafeUTF(in);
            sensorAddresses = (long[]) in.readObject();
            messageVersion = in.readInt();
            heartbeatTimeout = 30;
        } else if (ver == 4) {
            commPortId = SerializationHelper.readSafeUTF(in);
            sensorAddresses = (long[]) in.readObject();
            messageVersion = in.readInt();
            heartbeatTimeout = in.readInt();
        }
    }
}
