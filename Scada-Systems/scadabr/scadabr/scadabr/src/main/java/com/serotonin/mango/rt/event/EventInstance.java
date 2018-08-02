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
package com.serotonin.mango.rt.event;

import br.org.scadabr.ScadaBrConstants;
import br.org.scadabr.rt.event.type.EventSources;
import br.org.scadabr.vo.event.AlarmLevel;
import br.org.scadabr.vo.event.EventStatus;
import java.util.List;
import java.util.Map;

import com.serotonin.mango.rt.event.handlers.EventHandlerRT;
import com.serotonin.mango.rt.event.type.EventType;
import com.serotonin.mango.vo.UserComment;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import com.fasterxml.jackson.annotation.JsonIgnore;
import java.util.Collections;
import java.util.Objects;

public class EventInstance {

    /**
     * @return the stateful
     */
    public boolean isStateful() {
        return eventState != EventStatus.STATELESS;
    }

    @JsonIgnore
    public boolean isNew() {
        return id == null;
    }

    public boolean isGone() {
        return eventState == EventStatus.GONE;
    }

    public void setEventComments(List<UserComment> eventComments) {
        this.eventComments = eventComments;
    }

    /**
     * Configuration field. Assigned by the database.
     */
    private Integer id;

    /**
     * Configuration field. Provided by the event producer. Identifies where the
     * event came from and what it means.
     */
    private final EventType eventType;

    /**
     * State field. The time that the event was fired.
     */
    private final long fireTimestamp;

    /**
     * State field. The time that the event returned to normal.
     */
    private long goneTimestamp;

    /**
     * State field. The action that caused the event to INACTIVE. One of
     * {@link StatefulState}
     */
    private EventStatus eventState;

    /**
     * Configuration field. The alarm level assigned to the event.
     *
     * @see AlarmLevels
     */
    private final AlarmLevel alarmLevel;

    /**
     * Configuration field. The message associated with the event.
     */
    private final LocalizableMessage message;

    /**
     * User comments on the event. Added in the events interface after the event
     * has been raised.
     */
    private List<UserComment> eventComments;

    private List<EventHandlerRT> handlers;

    private long acknowledgedTimestamp;
    private int acknowledgedByUserId;
    private String acknowledgedByUsername;
    private AlternateAcknowledgementSources alternateAckSource;

    //
    //
    // These fields are used only in the context of access by a particular user, providing state filled in from
    // the userEvents table.
    private boolean userNotified;
    private boolean silenced;

    //
    // Contextual data from the source that raised the event.
    private final Map<String, Object> context;

    /**
     * init from dao
     *
     * @param type
     * @param activeTimestamp
     * @param alarmLevel
     * @param state
     * @param goneTimestamp
     * @param message
     */
    public EventInstance(EventType type, long activeTimestamp, AlarmLevel alarmLevel, EventStatus state, long goneTimestamp, LocalizableMessage message) {
        this.eventType = type;
        this.fireTimestamp = activeTimestamp;
        this.eventState = state;
        this.alarmLevel = alarmLevel;
        this.message = message;
        this.goneTimestamp = goneTimestamp;
        this.context = null;
    }

    /**
     * Create a new if staeful active or stateless event.
     *
     * @param eventType
     * @param activeTimestamp
     * @param message
     * @param context
     */
    public EventInstance(EventType eventType, long activeTimestamp, LocalizableMessage message, Map<String, Object> context) {
        this.eventType = eventType;
        this.fireTimestamp = activeTimestamp;
        this.eventState = eventType.isStateful() ? EventStatus.ACTIVE : EventStatus.STATELESS;
        this.alarmLevel = eventType.getAlarmLevel();
        if (message == null) {
            this.message = new LocalizableMessageImpl("common.noMessage");
        } else {
            this.message = message;
        }
        this.context = context;
    }

    public LocalizableMessage getStateMessage() {
        switch (eventState) {
            case STATELESS:
                return new LocalizableMessageImpl("event.state.stateless");
            case ACTIVE:
                return new LocalizableMessageImpl("event.state.active");
            case GONE:
                return new LocalizableMessageImpl("event.state.gone");
            case SOURCE_DISABLED:
                switch (eventType.getEventSource()) {
                    case DATA_POINT:
                        return new LocalizableMessageImpl("event.state.pointDisabled");
                    case DATA_SOURCE:
                        return new LocalizableMessageImpl("event.state.dsDisabled");
                    case PUBLISHER:
                        return new LocalizableMessageImpl("event.state.pubDisabled");
                    case MAINTENANCE:
                        return new LocalizableMessageImpl("event.state.maintDisabled");
                    default:
                        return new LocalizableMessageImpl("event.state.shutdown");
                }
            default:
                throw new RuntimeException("Unknown eventState: " + eventState);
        }
    }

    public LocalizableMessage getAckMessage() {
        if (isAcknowledged()) {
            if (acknowledgedByUserId != 0) {
                return new LocalizableMessageImpl("events.ackedByUser", acknowledgedByUsername);
            }
            switch (alternateAckSource) {
                case DELETED_USER:
                    return new LocalizableMessageImpl("events.ackedByDeletedUser");
                case MAINTENANCE_MODE:
                    return new LocalizableMessageImpl("events.ackedByMaintenance");
            }
        }

        return null;
    }

    public LocalizableMessage getExportAckMessage() {
        if (isAcknowledged()) {
            if (acknowledgedByUserId != 0) {
                return new LocalizableMessageImpl("events.export.ackedByUser", acknowledgedByUsername);
            }
            switch (alternateAckSource) {
                case DELETED_USER:
                    return new LocalizableMessageImpl("events.export.ackedByDeletedUser");
                case MAINTENANCE_MODE:
                    return new LocalizableMessageImpl("events.export.ackedByMaintenance");
            }
        }

        return null;
    }

    public boolean isAlarm() {
        return alarmLevel != AlarmLevel.NONE;
    }

    /**
     * This method should only be used by the EventDao for creating and
     * updating.
     *
     * @param id
     */
    public void setId(Integer id) {
        this.id = id;
    }

    public EventStatus getEventState() {
        return eventState;
    }

    public boolean isActive() {
        return eventState == EventStatus.ACTIVE;
    }

    public void setAlarmGone(long time) {
        goneTimestamp = time;
        this.eventState = EventStatus.GONE;
    }

    public void setAlarmDisabled(long time) {
        goneTimestamp = time;
        this.eventState = EventStatus.SOURCE_DISABLED;
    }

    public boolean isAcknowledged() {
        return acknowledgedTimestamp > 0;
    }

    public long getFireTimestamp() {
        return fireTimestamp;
    }

    public AlarmLevel getAlarmLevel() {
        return alarmLevel;
    }

    public EventType getEventType() {
        return eventType;
    }

    public EventSources getEventSource() {
        return eventType.getEventSource();
    }

    public int getId() {
        return id;
    }

    public long getGoneTimestamp() {
        return goneTimestamp;
    }

    public LocalizableMessage getMessage() {
        return message;
    }

    public void addEventComment(UserComment comment) {
        eventComments.add(comment);
    }

    public Iterable<UserComment> getEventComments() {
        if (eventComments == null) {
            return Collections.EMPTY_LIST;
        } else {
            return eventComments;
        }
    }

    public Iterable<EventHandlerRT> getHandlers() {
        if (handlers == null) {
            return Collections.EMPTY_LIST;
        } else {
            return handlers;
        }
    }

    public void setHandlers(List<EventHandlerRT> handlers) {
        this.handlers = handlers;
    }

    public boolean isUserNotified() {
        return userNotified;
    }

    public void setUserNotified(boolean userNotified) {
        this.userNotified = userNotified;
    }

    public boolean isSilenced() {
        return silenced;
    }

    public void setSilenced(boolean silenced) {
        this.silenced = silenced;
    }

    public long getAcknowledgedTimestamp() {
        return acknowledgedTimestamp;
    }

    public void setAcknowledgedTimestamp(long acknowledgedTimestamp) {
        this.acknowledgedTimestamp = acknowledgedTimestamp;
    }

    public int getAcknowledgedByUserId() {
        return acknowledgedByUserId;
    }

    public void setAcknowledgedByUserId(int acknowledgedByUserId) {
        this.acknowledgedByUserId = acknowledgedByUserId;
    }

    public String getAcknowledgedByUsername() {
        return acknowledgedByUsername;
    }

    public void setAcknowledgedByUsername(String acknowledgedByUsername) {
        this.acknowledgedByUsername = acknowledgedByUsername;
    }

    public AlternateAcknowledgementSources getAlternateAckSource() {
        return alternateAckSource;
    }

    public void setAlternateAckSource(AlternateAcknowledgementSources alternateAckSource) {
        this.alternateAckSource = alternateAckSource;
    }

    public Map<String, Object> getContext() {
        return context;
    }

    @Override
    public int hashCode() {
        int hash = 3;
        hash = 67 * hash + Objects.hashCode(this.eventType);
        hash = 67 * hash + (int) (this.fireTimestamp ^ (this.fireTimestamp >>> 32));
        return hash;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final EventInstance other = (EventInstance) obj;
        if (!Objects.equals(this.eventType, other.eventType)) {
            return false;
        }
        if (this.fireTimestamp != other.fireTimestamp) {
            return false;
        }
        return true;
    }
    
}
