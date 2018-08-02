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
package com.serotonin.mango.vo.dataSource.pop3;

import br.org.scadabr.DataType;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.text.DecimalFormat;
import java.text.SimpleDateFormat;
import java.util.List;
import java.util.Map;
import java.util.regex.Pattern;
import java.util.regex.PatternSyntaxException;

import br.org.scadabr.json.JsonException;
import br.org.scadabr.json.JsonObject;
import br.org.scadabr.json.JsonReader;


import br.org.scadabr.json.JsonSerializable;
import com.serotonin.mango.rt.dataSource.PointLocatorRT;
import com.serotonin.mango.rt.dataSource.pop3.Pop3PointLocatorRT;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.vo.dataSource.AbstractPointLocatorVO;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.util.StringUtils;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.web.taglib.Functions;
import java.util.EnumSet;

/**
 * @author Matthew Lohbihler
 */

public class Pop3PointLocatorVO extends AbstractPointLocatorVO implements JsonSerializable {

    @Override
    public boolean isSettable() {
        return false;
    }

    @Override
    public PointLocatorRT createRuntime() {
        return new Pop3PointLocatorRT(this);
    }

    @Override
    public LocalizableMessage getConfigurationDescription() {
        return new LocalizableMessageImpl("dsEdit.pop3.dpconn", Functions.escapeLessThan(valueRegex));
    }

    
    private boolean findInSubject;
    
    private String valueRegex;
    
    private boolean ignoreIfMissing;
    
    private String valueFormat;
    private DataType dataType;
    
    private boolean useReceivedTime;
    
    private String timeRegex;
    
    private String timeFormat;

    public boolean isFindInSubject() {
        return findInSubject;
    }

    public void setFindInSubject(boolean findInSubject) {
        this.findInSubject = findInSubject;
    }

    public String getValueRegex() {
        return valueRegex;
    }

    public void setValueRegex(String valueRegex) {
        this.valueRegex = valueRegex;
    }

    public boolean isIgnoreIfMissing() {
        return ignoreIfMissing;
    }

    public void setIgnoreIfMissing(boolean ignoreIfMissing) {
        this.ignoreIfMissing = ignoreIfMissing;
    }

    public String getValueFormat() {
        return valueFormat;
    }

    public void setValueFormat(String valueFormat) {
        this.valueFormat = valueFormat;
    }

    @Override
    public DataType getDataType() {
        return dataType;
    }

    public void setDataType(DataType dataType) {
        this.dataType = dataType;
    }

    public boolean isUseReceivedTime() {
        return useReceivedTime;
    }

    public void setUseReceivedTime(boolean useReceivedTime) {
        this.useReceivedTime = useReceivedTime;
    }

    public String getTimeRegex() {
        return timeRegex;
    }

    public void setTimeRegex(String timeRegex) {
        this.timeRegex = timeRegex;
    }

    public String getTimeFormat() {
        return timeFormat;
    }

    public void setTimeFormat(String timeFormat) {
        this.timeFormat = timeFormat;
    }

    @Override
    public void validate(DwrResponseI18n response) {
        if (valueRegex.isEmpty()) {
            response.addContextual("valueRegex", "validate.required");
        } else {
            try {
                Pattern pattern = Pattern.compile(valueRegex);
                if (pattern.matcher("").groupCount() < 1) {
                    response.addContextual("valueRegex", "validate.captureGroup");
                }
            } catch (PatternSyntaxException e) {
                response.addContextual("valueRegex", "common.default", e);
            }
        }

        if (dataType == DataType.NUMERIC && !valueFormat.isEmpty()) {
            try {
                new DecimalFormat(valueFormat);
            } catch (IllegalArgumentException e) {
                response.addContextual("valueFormat", "common.default", e);
            }
        }

        if (!timeRegex.isEmpty()) {
            try {
                Pattern pattern = Pattern.compile(timeRegex);
                if (pattern.matcher("").groupCount() < 1) {
                    response.addContextual("timeRegex", "validate.captureGroup");
                }
            } catch (PatternSyntaxException e) {
                response.addContextual("timeRegex", "common.default", e);
            }

            if (timeFormat.isEmpty()) {
                response.addContextual("timeFormat", "validate.required");
            } else {
                try {
                    new SimpleDateFormat(timeFormat);
                } catch (IllegalArgumentException e) {
                    response.addContextual("timeFormat", "common.default", e);
                }
            }
        }
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.pointDataType", dataType);
        AuditEventType.addPropertyMessage(list, "dsEdit.pop3.findInSubject", findInSubject);
        AuditEventType.addPropertyMessage(list, "dsEdit.pop3.valueRegex", valueRegex);
        AuditEventType.addPropertyMessage(list, "dsEdit.pop3.ignoreIfMissing", ignoreIfMissing);
        AuditEventType.addPropertyMessage(list, "dsEdit.pop3.numberFormat", valueFormat);
        AuditEventType.addPropertyMessage(list, "dsEdit.pop3.useMessageTime", useReceivedTime);
        AuditEventType.addPropertyMessage(list, "dsEdit.pop3.timeRegex", timeRegex);
        AuditEventType.addPropertyMessage(list, "dsEdit.pop3.timeFormat", timeFormat);
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, Object o) {
        Pop3PointLocatorVO from = (Pop3PointLocatorVO) o;
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.pointDataType", from.dataType, dataType);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.pop3.findInSubject", from.findInSubject,
                findInSubject);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.pop3.valueRegex", from.valueRegex, valueRegex);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.pop3.ignoreIfMissing", from.ignoreIfMissing,
                ignoreIfMissing);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.pop3.numberFormat", from.valueFormat, valueFormat);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.pop3.useMessageTime", from.useReceivedTime,
                useReceivedTime);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.pop3.timeRegex", from.timeRegex, timeRegex);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.pop3.timeFormat", from.timeFormat, timeFormat);
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
        out.writeBoolean(findInSubject);
        SerializationHelper.writeSafeUTF(out, valueRegex);
        out.writeBoolean(ignoreIfMissing);
        out.writeInt(dataType.mangoDbId);
        SerializationHelper.writeSafeUTF(out, valueFormat);
        out.writeBoolean(useReceivedTime);
        SerializationHelper.writeSafeUTF(out, timeRegex);
        SerializationHelper.writeSafeUTF(out, timeFormat);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            findInSubject = false;
            valueRegex = SerializationHelper.readSafeUTF(in);
            ignoreIfMissing = in.readBoolean();
            dataType = DataType.fromMangoDbId(in.readInt());
            valueFormat = SerializationHelper.readSafeUTF(in);
            useReceivedTime = in.readBoolean();
            timeRegex = SerializationHelper.readSafeUTF(in);
            timeFormat = SerializationHelper.readSafeUTF(in);
        } else if (ver == 2) {
            findInSubject = in.readBoolean();
            valueRegex = SerializationHelper.readSafeUTF(in);
            ignoreIfMissing = in.readBoolean();
            dataType = DataType.fromMangoDbId(in.readInt());
            valueFormat = SerializationHelper.readSafeUTF(in);
            useReceivedTime = in.readBoolean();
            timeRegex = SerializationHelper.readSafeUTF(in);
            timeFormat = SerializationHelper.readSafeUTF(in);
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
