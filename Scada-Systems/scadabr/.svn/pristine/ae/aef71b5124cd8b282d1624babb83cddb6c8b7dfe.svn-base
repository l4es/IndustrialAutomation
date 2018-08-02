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
package com.serotonin.mango.vo.dataSource.sql;

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
import com.serotonin.mango.rt.dataSource.sql.SqlPointLocatorRT;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.vo.dataSource.AbstractPointLocatorVO;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.util.StringUtils;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import java.util.EnumSet;

/**
 * @author Matthew Lohbihler
 */

public class SqlPointLocatorVO extends AbstractPointLocatorVO implements JsonSerializable {

    @Override
    public LocalizableMessage getConfigurationDescription() {
        return new LocalizableMessageImpl("common.default", fieldName);
    }

    @Override
    public boolean isSettable() {
        return !updateStatement.isEmpty();
    }

    @Override
    public PointLocatorRT createRuntime() {
        return new SqlPointLocatorRT(this);
    }

    
    private String fieldName;
    
    private String timeOverrideName;
    private DataType dataType;
    
    private String updateStatement;

    public String getFieldName() {
        return fieldName;
    }

    public void setFieldName(String fieldName) {
        this.fieldName = fieldName;
    }

    public String getTimeOverrideName() {
        return timeOverrideName;
    }

    public void setTimeOverrideName(String timeOverrideName) {
        this.timeOverrideName = timeOverrideName;
    }

    public String getUpdateStatement() {
        return updateStatement;
    }

    public void setUpdateStatement(String updateStatement) {
        this.updateStatement = updateStatement;
    }

    @Override
    public DataType getDataType() {
        return dataType;
    }

    public void setDataTypeId(DataType dataType) {
        this.dataType = dataType;
    }

    @Override
    public void validate(DwrResponseI18n response) {
        if (fieldName.isEmpty() && updateStatement.isEmpty()) {
            response.addContextual("fieldName", "validate.fieldName");
        }
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.pointDataType", dataType);
        AuditEventType.addPropertyMessage(list, "dsEdit.sql.rowId", fieldName);
        AuditEventType.addPropertyMessage(list, "dsEdit.sql.timeColumn", timeOverrideName);
        AuditEventType.addPropertyMessage(list, "dsEdit.sql.update", updateStatement);
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, Object o) {
        SqlPointLocatorVO from = (SqlPointLocatorVO) o;
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.pointDataType", from.dataType, dataType);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.sql.rowId", from.fieldName, fieldName);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.sql.timeColumn", from.timeOverrideName,
                timeOverrideName);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.sql.update", from.updateStatement, updateStatement);
    }

    //
    // /
    // / Serialization
    // /
    //
    private static final long serialVersionUID = -1;
    private static final int version = 2;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(version);
        SerializationHelper.writeSafeUTF(out, fieldName);
        SerializationHelper.writeSafeUTF(out, timeOverrideName);
        SerializationHelper.writeSafeUTF(out, updateStatement);
        out.writeInt(dataType.mangoDbId);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            fieldName = SerializationHelper.readSafeUTF(in);
            timeOverrideName = "";
            updateStatement = SerializationHelper.readSafeUTF(in);
            dataType = DataType.fromMangoDbId(in.readInt());
        } else if (ver == 2) {
            fieldName = SerializationHelper.readSafeUTF(in);
            timeOverrideName = SerializationHelper.readSafeUTF(in);
            updateStatement = SerializationHelper.readSafeUTF(in);
            dataType = DataType.fromMangoDbId(in.readInt());
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
