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

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.rt.UserRT;
import java.util.Map;

import br.org.scadabr.rt.event.type.DuplicateHandling;
import br.org.scadabr.rt.event.type.EventSources;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.event.AlarmLevel;
import br.org.scadabr.vo.event.type.EventKey;
import com.serotonin.mango.rt.EventManager;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

/**
 * An event class specifies the type of event that was raised.
 *
 * @author Matthew Lohbihler
 * @param <K>
 */
@Configurable
abstract public class EventType<K extends EventKey<K>> {

    @Autowired
    @Deprecated //TODO make an aspect out of it...
    private EventManager eventManager;

    //TODO do we need the context or can this data be retieved later???
    public void fire(Map<String, Object> context, String i18nKey, Object... i18nArgs) {
        if (eventManager != null) {
            eventManager.handleFiredEvent(this, System.currentTimeMillis(), new LocalizableMessageImpl(i18nKey, i18nArgs), context);
        }
    }

    //TODO do we need the context or can this data be retieved later???
    public void fire(Map<String, Object> context, long timestamp, LocalizableMessage msg) {
        if (eventManager != null) {
            eventManager.handleFiredEvent(this, timestamp, msg, context);
        }
    }

    //TODO do we need the context or can this data be retieved later???
    public void fire(Map<String, Object> context, long timestamp, String i18nKey, Object... i18nArgs) {
        if (eventManager != null) {
            eventManager.handleFiredEvent(this, timestamp, new LocalizableMessageImpl(i18nKey, i18nArgs), context);
        }
    }

    public void fire(String i18nKey, Object... i18nArgs) {
        if (eventManager != null) {
            eventManager.handleFiredEvent(this, System.currentTimeMillis(), new LocalizableMessageImpl(i18nKey, i18nArgs), null);
        }
    }

    public void fire(long timestamp, String i18nKey, Object... i18nArgs) {
        if (eventManager != null) {
            eventManager.handleFiredEvent(this, timestamp, new LocalizableMessageImpl(i18nKey, i18nArgs), null);
        }
    }

    public void fire(LocalizableMessage msg) {
        if (eventManager != null) {
            eventManager.handleFiredEvent(this, System.currentTimeMillis(), msg, null);
        }
    }

    public void fire(long timestamp, LocalizableMessage msg) {
        if (eventManager != null) {
            eventManager.handleFiredEvent(this, timestamp, msg, null);
        }
    }

    /**
     * Alarm is gone, so clear it
     *
     */
    public void clearAlarm() {
        if (eventManager != null) {
            eventManager.handleAlarmCleared(this, System.currentTimeMillis());
        }
    }

    /**
     * Alarm is gone, so clear it
     *
     * @param timestamp
     */
    public void clearAlarm(long timestamp) {
        if (eventManager != null) {
            eventManager.handleAlarmCleared(this, timestamp);
        }
    }

    public void disableAlarm() {
        if (eventManager != null) {
            eventManager.handleAlarmDisabled(this, System.currentTimeMillis());
        }
    }

    abstract public EventSources getEventSource();

    abstract public int getReferenceId1();

    abstract public int getReferenceId2();

    /**
     * Convenience method that keeps us from having to cast.
     *
     * @return false here, but the system message implementation will return
     * true.
     */
    public boolean isSystemMessage() {
        return false;
    }

    /**
     * Convenience method that keeps us from having to cast.
     *
     * @return
     * @throws ShouldNeverHappenException if accessed from wrong childInstance
     */
    public int getDataSourceId() throws ShouldNeverHappenException {
        throw new ShouldNeverHappenException("getDataSourceId() from" + this.getClass().getCanonicalName());
    }

    /**
     * Convenience method that keeps us from having to cast.
     *
     * @return
     * @throws ShouldNeverHappenException if accessed from wrong childInstance
     */
    public int getDataPointId() throws ShouldNeverHappenException {
        throw new ShouldNeverHappenException("getDataPointId() from" + this.getClass().getCanonicalName());
    }

    /**
     * Convenience method that keeps us from having to cast.
     *
     * @return
     * @throws ShouldNeverHappenException if accessed from wrong childInstance
     */
    public int getScheduleId() throws ShouldNeverHappenException {
        throw new ShouldNeverHappenException("getScheduleId() from" + this.getClass().getCanonicalName());
    }

    /**
     * Convenience method that keeps us from having to cast.
     *
     * @return
     * @throws ShouldNeverHappenException if accessed from wrong childInstance
     */
    public int getCompoundEventDetectorId() throws ShouldNeverHappenException {
        throw new ShouldNeverHappenException("getCompoundEventDetectorId() from" + this.getClass().getCanonicalName());
    }

    /**
     * Convenience method that keeps us from having to cast.
     *
     * @return
     * @throws ShouldNeverHappenException if accessed from wrong childInstance
     */
    public int getPublisherId() throws ShouldNeverHappenException {
        throw new ShouldNeverHappenException("getPublisherId() from" + this.getClass().getCanonicalName());
    }

    protected final K eventKey;

    public EventType(K eventKey) {
        this.eventKey = eventKey;
    }

    /**
     * Determines whether an event type that, once raised, will always first be
     * deactivated or whether overriding events can be raised. Overrides can
     * occur in data sources and point locators where a retry of a failed action
     * causes the same event type to be raised without the previous having
     * returned to normal.
     *
     * @return whether this event type can be overridden with newer event
     * instances.
     */
    public final DuplicateHandling getDuplicateHandling() {
        return eventKey.getDuplicateHandling();
    }

    public K getEventKey() {
        return eventKey;
    }

    /**
     * Determines if the notification of this event to the given user should be
     * suppressed. Useful if the action of the user resulted in the event being
     * raised.
     *
     * @return
     */
    public boolean excludeUser(@SuppressWarnings("unused") UserRT user) {
        return false;
    }

    /**
     * Currently ony the Alarmlevel for firing the event or raising the alarm.
     * EventIntsance does not know the Alarmlevel.
     *
     * @return the alarmLevel
     */
    public abstract AlarmLevel getAlarmLevel();

    public final boolean isStateful() {
        return eventKey.isStateful();
    }

    @Override
    public String toString() {
        return "EventType(eventType=" + eventKey + ")";
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + eventKey.getId();
        return result;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        EventType other = (EventType) obj;
        return eventKey == other.eventKey;
    }

}
