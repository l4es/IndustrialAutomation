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
package com.serotonin.mango.vo.dataSource.nmea;

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
import com.serotonin.mango.rt.dataSource.nmea.NmeaPointLocatorRT;
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

public class NmeaPointLocatorVO extends AbstractPointLocatorVO implements JsonSerializable {

    @Override
    public boolean isSettable() {
        return false;
    }

    @Override
    public PointLocatorRT createRuntime() {
        return new NmeaPointLocatorRT(this);
    }

    @Override
    public LocalizableMessage getConfigurationDescription() {
        return new LocalizableMessageImpl("dsEdit.nmea.dpconn", messageName, fieldIndex);
    }

    
    private String messageName;
    
    private int fieldIndex = 1;
    private DataType dataType;
    
    private String binary0Value;

    public String getMessageName() {
        return messageName;
    }

    public void setMessageName(String messageName) {
        this.messageName = messageName;
    }

    public int getFieldIndex() {
        return fieldIndex;
    }

    public void setFieldIndex(int fieldIndex) {
        this.fieldIndex = fieldIndex;
    }

    @Override
    public DataType getDataType() {
        return dataType;
    }

    public void setDataType(DataType dataType) {
        this.dataType = dataType;
    }

    public String getBinary0Value() {
        return binary0Value;
    }

    public void setBinary0Value(String binary0Value) {
        this.binary0Value = binary0Value;
    }

    @Override
    public void validate(DwrResponseI18n response) {
        if (messageName.isEmpty()) {
            response.addContextual("messageName", "validate.required");
        }

        if (fieldIndex <= 0) {
            response.addContextual("fieldIndex", "validate.greaterThanZero");
        }
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.pointDataType", dataType);
        AuditEventType.addPropertyMessage(list, "dsEdit.nmea.messageName", messageName);
        AuditEventType.addPropertyMessage(list, "dsEdit.nmea.binary0Value", binary0Value);
        AuditEventType.addPropertyMessage(list, "dsEdit.nmea.fieldIndex", fieldIndex);
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, Object o) {
        NmeaPointLocatorVO from = (NmeaPointLocatorVO) o;
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.pointDataType", from.dataType, dataType);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.nmea.messageName", from.messageName, messageName);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.nmea.binary0Value", from.binary0Value, binary0Value);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.nmea.fieldIndex", from.fieldIndex, fieldIndex);
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
        SerializationHelper.writeSafeUTF(out, messageName);
        out.writeInt(fieldIndex);
        out.writeInt(dataType.mangoDbId);
        SerializationHelper.writeSafeUTF(out, binary0Value);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            messageName = SerializationHelper.readSafeUTF(in);
            fieldIndex = in.readInt();
            dataType = DataType.fromMangoDbId(in.readInt());
            binary0Value = SerializationHelper.readSafeUTF(in);
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
