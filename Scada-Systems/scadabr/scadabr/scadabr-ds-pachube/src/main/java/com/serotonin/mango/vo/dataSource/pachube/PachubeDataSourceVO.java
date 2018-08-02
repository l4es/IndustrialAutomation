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
package com.serotonin.mango.vo.dataSource.pachube;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;
import java.util.Map;

import br.org.scadabr.json.JsonException;
import br.org.scadabr.json.JsonObject;
import br.org.scadabr.json.JsonReader;


import com.serotonin.mango.Common;
import com.serotonin.mango.rt.dataSource.DataSourceRT;
import com.serotonin.mango.rt.dataSource.pachube.PachubeDataSourceRT;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.util.ExportCodes;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.event.EventTypeVO;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.utils.TimePeriods;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;


public class PachubeDataSourceVO extends DataSourceVO<PachubeDataSourceVO> {

    public static final Type TYPE = Type.PACHUBE;

    @Override
    protected void addEventTypes(List<EventTypeVO> ets) {
        ets.add(createEventType(PachubeDataSourceRT.DATA_RETRIEVAL_FAILURE_EVENT, new LocalizableMessageImpl(
                "event.ds.dataRetrieval")));
        ets
                .add(createEventType(PachubeDataSourceRT.PARSE_EXCEPTION_EVENT, new LocalizableMessageImpl(
                                        "event.ds.dataParse")));
        ets.add(createEventType(PachubeDataSourceRT.POINT_WRITE_EXCEPTION_EVENT, new LocalizableMessageImpl(
                "event.ds.pointWrite")));
    }

    private static final ExportCodes EVENT_CODES = new ExportCodes();

    static {
        EVENT_CODES.addElement(PachubeDataSourceRT.DATA_RETRIEVAL_FAILURE_EVENT, "DATA_RETRIEVAL_FAILURE");
        EVENT_CODES.addElement(PachubeDataSourceRT.PARSE_EXCEPTION_EVENT, "PARSE_EXCEPTION");
        EVENT_CODES.addElement(PachubeDataSourceRT.POINT_WRITE_EXCEPTION_EVENT, "POINT_WRITE_EXCEPTION");
    }

    @Override
    public ExportCodes getEventCodes() {
        return EVENT_CODES;
    }

    @Override
    public LocalizableMessage getConnectionDescription() {
        return new LocalizableMessageImpl("common.noMessage");
    }

    @Override
    public Type getType() {
        return TYPE;
    }

    @Override
    public DataSourceRT createDataSourceRT() {
        return new PachubeDataSourceRT(this);
    }

    @Override
    public PachubePointLocatorVO createPointLocator() {
        return new PachubePointLocatorVO();
    }

    
    private String apiKey;
    private TimePeriods updatePeriodType = TimePeriods.MINUTES;
    
    private int updatePeriods = 5;
    
    private int timeoutSeconds = 30;
    
    private int retries = 2;

    public String getApiKey() {
        return apiKey;
    }

    public void setApiKey(String apiKey) {
        this.apiKey = apiKey;
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

    public int getTimeoutSeconds() {
        return timeoutSeconds;
    }

    public void setTimeoutSeconds(int timeoutSeconds) {
        this.timeoutSeconds = timeoutSeconds;
    }

    public int getRetries() {
        return retries;
    }

    public void setRetries(int retries) {
        this.retries = retries;
    }

    @Override
    public void validate(DwrResponseI18n response) {
        super.validate(response);
        if (apiKey.isEmpty()) {
            response.addContextual("apiKey", "validate.required");
        }
        if (updatePeriods <= 0) {
            response.addContextual("updatePeriods", "validate.greaterThanZero");
        }
        if (timeoutSeconds <= 0) {
            response.addContextual("timeoutSeconds", "validate.greaterThanZero");
        }
        if (retries < 0) {
            response.addContextual("retries", "validate.cannotBeNegative");
        }
    }

    @Override
    protected void addPropertiesImpl(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.pachube.apiKey", apiKey);
        AuditEventType.addPropertyMessage(list, "dsEdit.updatePeriod", updatePeriodType.getPeriodDescription(updatePeriods));
        AuditEventType.addPropertyMessage(list, "dsEdit.pachube.timeout", timeoutSeconds);
        AuditEventType.addPropertyMessage(list, "dsEdit.pachube.retries", retries);
    }

    @Override
    protected void addPropertyChangesImpl(List<LocalizableMessage> list, PachubeDataSourceVO from) {
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.pachube.apiKey", from.apiKey, apiKey);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.updatePeriod", 
                from.updatePeriodType.getPeriodDescription(from.updatePeriods), 
                updatePeriodType.getPeriodDescription(updatePeriods));
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.pachube.timeout", from.timeoutSeconds,
                timeoutSeconds);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.pachube.retries", from.retries, retries);
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
        SerializationHelper.writeSafeUTF(out, apiKey);
        out.writeInt(updatePeriodType.mangoDbId);
        out.writeInt(updatePeriods);
        out.writeInt(timeoutSeconds);
        out.writeInt(retries);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            apiKey = SerializationHelper.readSafeUTF(in);
            updatePeriodType = TimePeriods.fromMangoDbId(in.readInt());
            updatePeriods = in.readInt();
            timeoutSeconds = in.readInt();
            retries = in.readInt();
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
