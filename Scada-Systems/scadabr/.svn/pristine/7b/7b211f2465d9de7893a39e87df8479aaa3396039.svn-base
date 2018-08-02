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
package com.serotonin.mango.rt.event.type;

import br.org.scadabr.json.JsonException;
import br.org.scadabr.json.JsonObject;
import br.org.scadabr.json.JsonValue;
import br.org.scadabr.json.TypeFactory;
import br.org.scadabr.rt.event.type.EventSources;
import com.serotonin.mango.util.LocalizableJsonException;

public class EventTypeFactory implements TypeFactory {

    @Override
    public Class<?> getType(JsonValue jsonValue) throws JsonException {
        if (jsonValue.isNull()) {
            throw new LocalizableJsonException("emport.error.eventType.null");
        }

        JsonObject json = jsonValue.toJsonObject();

        String text = json.getString("sourceType");
        if (text == null) {
            throw new LocalizableJsonException("emport.error.eventType.missing", "sourceType",
                    EventSources.values());
        }
        try {
            switch (EventSources.valueOf(text)) {
                case DATA_POINT:
                    return DataPointEventType.class;
                case DATA_SOURCE:
                    return DataSourceEventType.class;
                case SYSTEM:
                    return SystemEventType.class;
                case COMPOUND:
                    return CompoundDetectorEventType.class;
                case SCHEDULED:
                    return ScheduledEventType.class;
                case PUBLISHER:
                    return PublisherEventType.class;
                case AUDIT:
                    return AuditEventType.class;
                case MAINTENANCE:
                    return MaintenanceEventType.class;
                default:
                    return null;
            }
        } catch (Exception e) {
            throw new LocalizableJsonException("emport.error.eventType.invalid", "sourceType", text,
                    EventSources.values());
        }
    }
}
