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

import br.org.scadabr.DataType;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;
import java.util.Map;

import br.org.scadabr.json.JsonException;
import br.org.scadabr.json.JsonObject;
import br.org.scadabr.json.JsonReader;


import br.org.scadabr.json.JsonSerializable;
import com.serotonin.mango.rt.dataSource.PointLocatorRT;
import com.serotonin.mango.rt.dataSource.jmx.JmxPointLocatorRT;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.vo.dataSource.AbstractPointLocatorVO;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import java.util.EnumSet;

/**
 * @author Matthew Lohbihler
 */

public class JmxPointLocatorVO extends AbstractPointLocatorVO implements JsonSerializable {

    
    private String objectName;
    
    private String attributeName;
    
    private String compositeItemName;
    private DataType dataType;
    
    private boolean settable;

    @Override
    public boolean isSettable() {
        return settable;
    }

    @Override
    public PointLocatorRT createRuntime() {
        return new JmxPointLocatorRT(this);
    }

    @Override
    public LocalizableMessage getConfigurationDescription() {
        StringBuilder sb = new StringBuilder();
        sb.append(objectName).append(" > ").append(attributeName);
        if (!compositeItemName.isEmpty()) {
            sb.append(" > ").append(compositeItemName);
        }
        return new LocalizableMessageImpl("common.default", sb.toString());
    }

    public String getObjectName() {
        return objectName;
    }

    public void setObjectName(String objectName) {
        this.objectName = objectName;
    }

    public String getAttributeName() {
        return attributeName;
    }

    public void setAttributeName(String attributeName) {
        this.attributeName = attributeName;
    }

    public String getCompositeItemName() {
        return compositeItemName;
    }

    public void setCompositeItemName(String compositeItemName) {
        this.compositeItemName = compositeItemName;
    }

    @Override
    public DataType getDataType() {
        return dataType;
    }

    public void setDataType(DataType dataType) {
        this.dataType = dataType;
    }

    public void setSettable(boolean settable) {
        this.settable = settable;
    }

    @Override
    public void validate(DwrResponseI18n response) {
        if (objectName.isEmpty()) {
            response.addContextual("objectName", "validate.required");
        }
        if (attributeName.isEmpty()) {
            response.addContextual("attributeName", "validate.required");
        }
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.jmx.objectName", objectName);
        AuditEventType.addPropertyMessage(list, "dsEdit.jmx.attributeName", attributeName);
        AuditEventType.addPropertyMessage(list, "dsEdit.jmx.compositeItemName", compositeItemName);
        AuditEventType.addPropertyMessage(list, "dsEdit.pointDataType", dataType);
        AuditEventType.addPropertyMessage(list, "dsEdit.settable", settable);
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, Object o) {
        JmxPointLocatorVO from = (JmxPointLocatorVO) o;
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.jmx.objectName", from.objectName, objectName);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.jmx.attributeName", from.attributeName,
                attributeName);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.jmx.compositeItemName", from.compositeItemName,
                compositeItemName);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.pointDataType", from.dataType, dataType);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.settable", from.settable, settable);
    }

    //
    //
    // Serialization
    //
    private static final long serialVersionUID = -1;
    private static final int version = 1;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(version);
        SerializationHelper.writeSafeUTF(out, objectName);
        SerializationHelper.writeSafeUTF(out, attributeName);
        SerializationHelper.writeSafeUTF(out, compositeItemName);
        out.writeInt(dataType.mangoDbId);
        out.writeBoolean(settable);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            objectName = SerializationHelper.readSafeUTF(in);
            attributeName = SerializationHelper.readSafeUTF(in);
            compositeItemName = SerializationHelper.readSafeUTF(in);
            dataType = DataType.fromMangoDbId(in.readInt());
            settable = in.readBoolean();
        }
    }

    @Override
    public void jsonDeserialize(JsonReader reader, JsonObject json) throws JsonException {
        DataType value = deserializeDataType(json, EnumSet.of(DataType.IMAGE));
        if (value != null) {
            dataType = value;
        }
    }

    @Override
    public void jsonSerialize(Map<String, Object> map) {
        serializeDataType(map);
    }
}
