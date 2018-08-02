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
package com.serotonin.mango.vo.dataSource.viconics;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;



import br.org.scadabr.rt.event.type.DuplicateHandling;
import com.serotonin.mango.rt.dataSource.DataSourceRT;
import com.serotonin.mango.rt.dataSource.viconics.ViconicsDataSourceRT;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.rt.event.type.EventType;
import com.serotonin.mango.util.ExportCodes;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.event.EventTypeVO;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.vo.event.AlarmLevel;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;

/**
 * To display in the add list, run this: insert into systemSettings
 * (settingName, settingValue) values ('VICONICS.display', 'Y')
 *
 * @author Matthew Lohbihler
 */

public class ViconicsDataSourceVO extends DataSourceVO<ViconicsDataSourceVO> {

    public static final Type TYPE = Type.VICONICS;

    @Override
    protected void addEventTypes(List<EventTypeVO> ets) {
        ets.add(createEventType(ViconicsDataSourceRT.INITIALIZATION_EXCEPTION_EVENT, new LocalizableMessageImpl(
                "event.ds.initialization")));
        ets
                .add(createEventType(ViconicsDataSourceRT.MESSAGE_EXCEPTION_EVENT, new LocalizableMessageImpl(
                                        "event.ds.message"), DuplicateHandling.IGNORE, AlarmLevel.INFORMATION));
        ets.add(createEventType(ViconicsDataSourceRT.DEVICE_OFFLINE_EVENT, new LocalizableMessageImpl("event.ds.device"),
                DuplicateHandling.IGNORE, AlarmLevel.INFORMATION));
        ets
                .add(createEventType(ViconicsDataSourceRT.NETWORK_OFFLINE_EVENT, new LocalizableMessageImpl(
                                        "event.ds.network")));
        ets.add(createEventType(ViconicsDataSourceRT.DUPLICATE_COMM_ADDRESS_EVENT, new LocalizableMessageImpl(
                "event.ds.duplicateComm")));
    }

    private static final ExportCodes EVENT_CODES = new ExportCodes();

    static {
        EVENT_CODES.addElement(ViconicsDataSourceRT.INITIALIZATION_EXCEPTION_EVENT, "INITIALIZATION_EXCEPTION");
        EVENT_CODES.addElement(ViconicsDataSourceRT.MESSAGE_EXCEPTION_EVENT, "MESSAGE_EXCEPTION");
        EVENT_CODES.addElement(ViconicsDataSourceRT.DEVICE_OFFLINE_EVENT, "DEVICE_OFFLINE");
        EVENT_CODES.addElement(ViconicsDataSourceRT.NETWORK_OFFLINE_EVENT, "NETWORK_OFFLINE");
        EVENT_CODES.addElement(ViconicsDataSourceRT.DUPLICATE_COMM_ADDRESS_EVENT, "DUPLICATE_COMM_ADDRESS");
    }

    @Override
    public ExportCodes getEventCodes() {
        return EVENT_CODES;
    }

    @Override
    public LocalizableMessage getConnectionDescription() {
        return new LocalizableMessageImpl("dsEdit.viconics.dpconn", commPortId, panId, channel);
    }

    @Override
    public Type getType() {
        return TYPE;
    }

    @Override
    public DataSourceRT createDataSourceRT() {
        return new ViconicsDataSourceRT(this);
    }

    @Override
    public ViconicsPointLocatorVO createPointLocator() {
        return new ViconicsPointLocatorVO();
    }

    
    private String commPortId;
    
    private int panId = 1;
    
    private int channel = 1;
    
    private int timeout = 30000;
    
    private int retries = 2;
    
    private int networkTimeoutSeconds = 20;
    
    private int deviceWarningTimeoutSeconds = 60;
    
    private int deviceRemoveTimeoutSeconds = 300;
    
    private int pointValueMinimumFreshnessSeconds = 1800;
    
    private boolean convertToCelsius = false;

    public String getCommPortId() {
        return commPortId;
    }

    public void setCommPortId(String commPortId) {
        this.commPortId = commPortId;
    }

    public int getPanId() {
        return panId;
    }

    public void setPanId(int panId) {
        this.panId = panId;
    }

    public int getChannel() {
        return channel;
    }

    public void setChannel(int channel) {
        this.channel = channel;
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

    public int getNetworkTimeoutSeconds() {
        return networkTimeoutSeconds;
    }

    public void setNetworkTimeoutSeconds(int networkTimeoutSeconds) {
        this.networkTimeoutSeconds = networkTimeoutSeconds;
    }

    public int getDeviceWarningTimeoutSeconds() {
        return deviceWarningTimeoutSeconds;
    }

    public void setDeviceWarningTimeoutSeconds(int deviceWarningTimeoutSeconds) {
        this.deviceWarningTimeoutSeconds = deviceWarningTimeoutSeconds;
    }

    public int getDeviceRemoveTimeoutSeconds() {
        return deviceRemoveTimeoutSeconds;
    }

    public void setDeviceRemoveTimeoutSeconds(int deviceRemoveTimeoutSeconds) {
        this.deviceRemoveTimeoutSeconds = deviceRemoveTimeoutSeconds;
    }

    public int getPointValueMinimumFreshnessSeconds() {
        return pointValueMinimumFreshnessSeconds;
    }

    public void setPointValueMinimumFreshnessSeconds(int pointValueMinimumFreshnessSeconds) {
        this.pointValueMinimumFreshnessSeconds = pointValueMinimumFreshnessSeconds;
    }

    public boolean isConvertToCelsius() {
        return convertToCelsius;
    }

    public void setConvertToCelsius(boolean convertToCelsius) {
        this.convertToCelsius = convertToCelsius;
    }

    @Override
    public void validate(DwrResponseI18n response) {
        super.validate(response);

        if (commPortId.isEmpty()) {
            response.addContextual("commPortId", "validate.required");
        }
        if (panId < 0 || panId > 66535) {
            response.addContextual("panId", "validate.between", 0, 66535);
        }
        if (channel < 0 || channel > 255) {
            response.addContextual("channel", "validate.between", 0, 255);
        }
        if (timeout <= 0) {
            response.addContextual("timeout", "validate.greaterThanZero");
        }
        if (retries <= 0) {
            response.addContextual("retries", "validate.greaterThanZero");
        }
        if (networkTimeoutSeconds <= 0) {
            response.addContextual("networkTimeoutSeconds", "validate.greaterThanZero");
        }
        if (deviceWarningTimeoutSeconds <= 0) {
            response.addContextual("deviceWarningTimeoutSeconds", "validate.greaterThanZero");
        }
        if (deviceRemoveTimeoutSeconds <= 0) {
            response.addContextual("deviceRemoveTimeoutSeconds", "validate.greaterThanZero");
        }
        if (pointValueMinimumFreshnessSeconds <= 0) {
            response.addContextual("pointValueMinimumFreshnessSeconds", "validate.greaterThanZero");
        }
    }

    @Override
    public void addPropertiesImpl(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.viconics.port", commPortId);
        AuditEventType.addPropertyMessage(list, "dsEdit.viconics.panId", panId);
        AuditEventType.addPropertyMessage(list, "dsEdit.viconics.channel", channel);
        AuditEventType.addPropertyMessage(list, "dsEdit.viconics.timeout", timeout);
        AuditEventType.addPropertyMessage(list, "dsEdit.viconics.retries", retries);
        AuditEventType.addPropertyMessage(list, "dsEdit.viconics.networkTimeout", networkTimeoutSeconds);
        AuditEventType.addPropertyMessage(list, "dsEdit.viconics.deviceWarning", deviceWarningTimeoutSeconds);
        AuditEventType.addPropertyMessage(list, "dsEdit.viconics.deviceRemove", deviceRemoveTimeoutSeconds);
        AuditEventType.addPropertyMessage(list, "dsEdit.viconics.pointFreshness", pointValueMinimumFreshnessSeconds);
        AuditEventType.addPropertyMessage(list, "dsEdit.viconics.convertCelsius", convertToCelsius);
    }

    @Override
    public void addPropertyChangesImpl(List<LocalizableMessage> list, ViconicsDataSourceVO from) {
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.viconics.port", from.commPortId, commPortId);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.viconics.panId", from.panId, panId);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.viconics.channel", from.channel, channel);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.viconics.timeout", from.timeout, timeout);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.viconics.retries", from.retries, retries);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.viconics.networkTimeout",
                from.networkTimeoutSeconds, networkTimeoutSeconds);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.viconics.deviceWarning",
                from.deviceWarningTimeoutSeconds, deviceWarningTimeoutSeconds);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.viconics.deviceRemove",
                from.deviceRemoveTimeoutSeconds, deviceRemoveTimeoutSeconds);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.viconics.pointFreshness",
                from.pointValueMinimumFreshnessSeconds, pointValueMinimumFreshnessSeconds);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.viconics.convertCelsius", from.convertToCelsius,
                convertToCelsius);
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
        SerializationHelper.writeSafeUTF(out, commPortId);
        out.writeInt(panId);
        out.writeInt(channel);
        out.writeInt(timeout);
        out.writeInt(retries);
        out.writeInt(networkTimeoutSeconds);
        out.writeInt(deviceWarningTimeoutSeconds);
        out.writeInt(deviceRemoveTimeoutSeconds);
        out.writeInt(pointValueMinimumFreshnessSeconds);
        out.writeBoolean(convertToCelsius);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            commPortId = SerializationHelper.readSafeUTF(in);
            panId = in.readInt();
            channel = in.readInt();
            timeout = in.readInt();
            retries = in.readInt();
            networkTimeoutSeconds = in.readInt();
            deviceWarningTimeoutSeconds = in.readInt();
            deviceRemoveTimeoutSeconds = in.readInt();
            pointValueMinimumFreshnessSeconds = in.readInt();
            convertToCelsius = in.readBoolean();
        }
    }
}
