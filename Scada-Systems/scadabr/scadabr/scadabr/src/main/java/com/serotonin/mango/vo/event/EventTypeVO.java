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
package com.serotonin.mango.vo.event;

import br.org.scadabr.rt.event.type.DuplicateHandling;
import br.org.scadabr.rt.event.type.EventSources;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.vo.event.AlarmLevel;
import java.util.List;

import com.serotonin.mango.rt.event.type.DataPointEventType;
import com.serotonin.mango.rt.event.type.EventType;
import com.serotonin.mango.rt.event.type.SystemEventType;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.vo.event.type.DataPointDetectorKey;
import br.org.scadabr.vo.event.type.SystemEventKey;

@Deprecated //TODO Whats this for anyway currently stateful is not handled at all...
public class EventTypeVO {

    /**
     * The type of event. @see EventSources
     */
    private EventSources eventSource;
    /**
     * For data point event, the data point id For data source event, the data
     * source id For system event, the type id
     */
    private int typeRef1;
    /**
     * For data point event, the point event detector id For data source event,
     * the data source event type For system event, undefined
     */
    private int typeRef2;
    private LocalizableMessage description;
    private List<EventHandlerVO> handlers;
    private AlarmLevel alarmLevel;
    private String eventDetectorKey;
    private DuplicateHandling duplicateHandling;
    private boolean stateful;

    public EventTypeVO(EventSources eventSource, int typeRef1, int typeRef2) {
        this.eventSource = eventSource;
        this.typeRef1 = typeRef1;
        this.typeRef2 = typeRef2;
    }

    public EventTypeVO(EventSources eventSource, int typeRef1, int typeRef2, LocalizableMessage description, AlarmLevel alarmLevel) {
        this(eventSource, typeRef1, typeRef2);
        this.description = description;
        this.alarmLevel = alarmLevel;
    }

    public EventTypeVO(EventSources eventSource, int typeRef1, int typeRef2, LocalizableMessage description, AlarmLevel alarmLevel,
            DuplicateHandling duplicateHandling) {
        this(eventSource, typeRef1, typeRef2);
        this.description = description;
        this.alarmLevel = alarmLevel;
        this.duplicateHandling = duplicateHandling;
    }

    public EventTypeVO(EventSources eventSource, int typeRef1, int typeRef2, LocalizableMessage description, AlarmLevel alarmLevel,
            String eventDetectorKey) {
        this(eventSource, typeRef1, typeRef2, description, alarmLevel);
        this.eventDetectorKey = eventDetectorKey;
    }

    public EventType createEventType() {
        switch (eventSource) {
            case DATA_POINT:
                return new DataPointEventType(typeRef1, DataPointDetectorKey.fromId(typeRef2), alarmLevel);
            case DATA_SOURCE:
                throw new ImplementMeException();
                //TODO  signature   public DataSourceEventType(int dataSourceId, DataSourceEventKey dataSourceEventKey, AlarmLevel alarmLevel) {
                // return new DataSourceEventType(typeRef1, typeRef2, alarmLevel, duplicateHandling, stateful);
            case SYSTEM:
                return new SystemEventType(SystemEventKey.fromId(typeRef1), typeRef2);
            case COMPOUND:
                throw new ImplementMeException();
                //TODO return new CompoundDetectorEventType(typeRef1);
            case SCHEDULED:
                throw new ImplementMeException();
                //TODO return new ScheduledEventType(typeRef1);
            case PUBLISHER:
                throw new ImplementMeException();
                //TODO return new PublisherEventType(typeRef1, typeRef2);
            case AUDIT:
                throw new ImplementMeException();
                //TODO return new AuditEventType(AuditEventKey.fromId(typeRef1), typeRef2, null);
            case MAINTENANCE:
                throw new ImplementMeException();
                //TODO return new MaintenanceEventType(typeRef1);
            default:
                throw new RuntimeException("Cant handle EventType");
        }
    }

    public EventSources getEventSource() {
        return eventSource;
    }

    public void setEventSource(EventSources eventSource) {
        this.eventSource = eventSource;
    }

    public int getTypeRef1() {
        return typeRef1;
    }

    public void setTypeRef1(int typeRef1) {
        this.typeRef1 = typeRef1;
    }

    public int getTypeRef2() {
        return typeRef2;
    }

    public void setTypeRef2(int typeRef2) {
        this.typeRef2 = typeRef2;
    }

    public LocalizableMessage getDescription() {
        return description;
    }

    public void setDescription(LocalizableMessage description) {
        this.description = description;
    }

    public List<EventHandlerVO> getHandlers() {
        return handlers;
    }

    public void setHandlers(List<EventHandlerVO> handlers) {
        this.handlers = handlers;
    }

    public AlarmLevel getAlarmLevel() {
        return alarmLevel;
    }

    public void setAlarmLevel(AlarmLevel alarmLevel) {
        this.alarmLevel = alarmLevel;
    }

    public String getEventDetectorKey() {
        return eventDetectorKey;
    }

    public void setEventDetectorKey(String eventDetectorKey) {
        this.eventDetectorKey = eventDetectorKey;
    }
}
