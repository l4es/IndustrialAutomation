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
package com.serotonin.mango.rt;

import br.org.scadabr.dao.EventDao;
import br.org.scadabr.l10n.AbstractLocalizer;
import br.org.scadabr.rt.UserRT;
import br.org.scadabr.rt.event.maintenance.MaintenanceEventManager;
import br.org.scadabr.rt.event.type.EventSources;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.CopyOnWriteArrayList;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.serotonin.mango.Common;
import com.serotonin.mango.rt.event.EventInstance;
import com.serotonin.mango.rt.event.handlers.EmailHandlerRT;
import com.serotonin.mango.rt.event.handlers.EventHandlerRT;
import com.serotonin.mango.rt.event.type.DataPointEventType;
import com.serotonin.mango.rt.event.type.DataSourceEventType;
import com.serotonin.mango.rt.event.type.EventType;
import com.serotonin.mango.rt.event.type.SystemEventType;
import com.serotonin.mango.vo.event.EventHandlerVO;
import com.serotonin.mango.vo.permission.Permissions;
import br.org.scadabr.util.ILifecycle;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.event.AlarmLevel;
import br.org.scadabr.vo.event.type.SystemEventKey;
import com.serotonin.mango.rt.event.AlternateAcknowledgementSources;
import java.io.Serializable;
import javax.inject.Inject;
import javax.inject.Named;
import org.springframework.context.annotation.Scope;

/**
 * @author Matthew Lohbihler
 */
@Named
@Scope("singleton")
public class EventManager implements ILifecycle, Serializable {

    private final Log log = LogFactory.getLog(EventManager.class);

    private final List<EventInstance> activeEvents = new CopyOnWriteArrayList<>();
    @Inject
    private MaintenanceEventManager maintenanceEventManager;
    @Inject
    private EventDao eventDao;
    @Inject
    private RuntimeManager runtimeManager;
    private long lastAlarmTimestamp = 0;
    private AlarmLevel highestActiveAlarmLevel = AlarmLevel.NONE;

    //
    //
    // Basic event management.
    //
    /**
     * An event get fired as single point in time An alarm gets raised if the
     * outcom of the event exists longer - and must be cleared...
     *
     * TODO ducument this properly
     *
     * @param type
     * @param i18nKey
     * @param i18nArgs
     */
    public void handleFiredEvent(EventType type, long timestamp, LocalizableMessage message, Map<String, Object> context) {
        // Check if there is an event for this type already active.
        EventInstance dup = get(type);
        if (dup != null) {
            // Check the duplicate handling.
            switch (type.getDuplicateHandling()) {
                case DO_NOT_ALLOW:
                    // Create a log error...
                    log.error("An event was raised for a type that is already active: type=" + type + ", message="
                            + message.getI18nKey());
                    // ... but ultimately just ignore the thing.
                    return;
                case IGNORE: // Safely return.
                    return;
                case IGNORE_SAME_MESSAGE:
                    // Ignore only if the message is the same. There may be events of this type with different messages,
                    // so look through them all for a match.
                    for (EventInstance e : getAll(type)) {
                        if (e.getMessage().equals(message)) {
                            return;
                        }
                    }
                    break;
                default:
            }

            // Otherwise we just continue...
        }

        // Determine if the event should be suppressed.
        boolean suppressed = isSuppressed(type);

        EventInstance evt = new EventInstance(type, timestamp, message, context);

        if (!suppressed) {
            setHandlers(evt);
        }

        // Get id from database by inserting event immediately.
        eventDao.saveEvent(evt);

        // Create user alarm records for all applicable users
        List<Integer> eventUserIds = new ArrayList<>();
        Set<String> emailUsers = new HashSet<>();

        runtimeManager.getActiveUsers().forEach((user) -> {
            // Do not create an event for this user if the event type says the user should be skipped.
            if (!type.excludeUser(user)) {
                if (Permissions.hasEventTypePermission(user, type)) {
                    eventUserIds.add(user.getId());
                    if (evt.isAlarm() && user.isReceiveAlarmEmails() && type.getAlarmLevel().otherIsLower(user.getReceiveAlarmEmails())) {
                        emailUsers.add(user.getEmail());
                    }
                }
            }
        });

        if (eventUserIds.size() > 0) {
            eventDao.insertUserEvents(evt.getId(), eventUserIds, evt.isAlarm());
            if (!suppressed && evt.isAlarm()) {
                lastAlarmTimestamp = System.currentTimeMillis();
            }
        }

        if (evt.isActive()) {
            activeEvents.add(evt);
        }

        if (suppressed) {
            eventDao.ackEvent(evt.getId(), timestamp, null, AlternateAcknowledgementSources.MAINTENANCE_MODE);
        } else {
            if (evt.isActive()) {
                if (type.getAlarmLevel().meIsHigher(highestActiveAlarmLevel)) {
                    AlarmLevel oldValue = highestActiveAlarmLevel;
                    highestActiveAlarmLevel = type.getAlarmLevel();
                    new SystemEventType(SystemEventKey.MAX_ALARM_LEVEL_CHANGED).fire(timestamp, "event.alarmMaxIncreased", oldValue, highestActiveAlarmLevel);
                }
            }

            // Call raiseEvent handlers.
            handleRaiseEvent(evt, emailUsers);

            if (log.isDebugEnabled()) {
                log.debug("Event raised: type=" + type + ", message=" + AbstractLocalizer.localizeMessage(message, Common.getBundle()));
            }
        }
    }

    public void handleAlarmCleared(EventType type, long timestamp) {
        EventInstance evt = remove(type);

        // Loop in case of multiples
        while (evt != null) {
            resetHighestAlarmLevel(timestamp, false);

            evt.setAlarmGone(timestamp);
            eventDao.saveEvent(evt);

            // Call inactiveEvent handlers.
            handleInactiveEvent(evt);

            // Check for another
            evt = remove(type);
        }

        if (log.isDebugEnabled()) {
            log.debug("Event returned to normal: type=" + type);
        }
    }

    public void handleAlarmDisabled(EventType type, long timestamp) {
        EventInstance evt = remove(type);

        // Loop in case of multiples
        while (evt != null) {
            resetHighestAlarmLevel(timestamp, false);

            evt.setAlarmDisabled(timestamp);
            eventDao.saveEvent(evt);

            // Call inactiveEvent handlers.
            handleInactiveEvent(evt);

            // Check for another
            evt = remove(type);
        }

        if (log.isDebugEnabled()) {
            log.debug("Event returned to normal: type=" + type);
        }
    }

    private void disableAlarm(EventInstance evt, long time) {
        activeEvents.remove(evt);
        resetHighestAlarmLevel(time, false);
        evt.setAlarmDisabled(time);
        eventDao.saveEvent(evt);

        // Call inactiveEvent handlers.
        handleInactiveEvent(evt);
    }

    public long getLastAlarmTimestamp() {
        return lastAlarmTimestamp;
    }

    //
    //
    // Canceling events.
    //
    public void cancelEventsForDataPoint(int dataPointId) {
        for (EventInstance e : activeEvents) {
            final EventType et = e.getEventType();
            if (et.getEventSource() == EventSources.DATA_POINT && et.getDataPointId() == dataPointId) {
                disableAlarm(e, System.currentTimeMillis());
            }
        }
    }

    public void cancelEventsForDataSource(int dataSourceId) {
        for (EventInstance e : activeEvents) {
            final EventType et = e.getEventType();
            if (et.getEventSource() == EventSources.DATA_SOURCE && et.getDataSourceId() == dataSourceId) {
                disableAlarm(e, System.currentTimeMillis());
            }
        }
    }

    public void cancelEventsForPublisher(int publisherId) {
        for (EventInstance e : activeEvents) {
            final EventType et = e.getEventType();
            if (et.getEventSource() == EventSources.PUBLISHER && et.getPublisherId() == publisherId) {
                disableAlarm(e, System.currentTimeMillis());
            }
        }
    }

    private void resetHighestAlarmLevel(long time, boolean init) {
        AlarmLevel max = AlarmLevel.NONE;
        for (EventInstance e : activeEvents) {
            if (e.getAlarmLevel().otherIsHigher(max)) {
                max = e.getAlarmLevel();
            }
        }

        if (!init) {
            if (max.meIsHigher(highestActiveAlarmLevel)) {
                AlarmLevel oldValue = highestActiveAlarmLevel;
                highestActiveAlarmLevel = max;
                new SystemEventType(SystemEventKey.MAX_ALARM_LEVEL_CHANGED).fire(time, "event.alarmMaxIncreased", oldValue, highestActiveAlarmLevel);
            } else if (max.meIsLower(highestActiveAlarmLevel)) {
                AlarmLevel oldValue = highestActiveAlarmLevel;
                highestActiveAlarmLevel = max;
                new SystemEventType(SystemEventKey.MAX_ALARM_LEVEL_CHANGED).fire(time, "event.alarmMaxDecreased", oldValue, highestActiveAlarmLevel);
            }
        }
    }

    private LocalizableMessage getAlarmLevelChangeMessage(String key, AlarmLevel oldValue) {
        return new LocalizableMessageImpl(key, oldValue.getI18nKey(), highestActiveAlarmLevel.getI18nKey());
    }

    //
    //
    // Lifecycle interface
    //
    @Override
    public void initialize() {
        // Get all active events from the database.
        activeEvents.addAll(eventDao.getActiveEvents());
        lastAlarmTimestamp = System.currentTimeMillis();
        resetHighestAlarmLevel(lastAlarmTimestamp, true);
    }

    @Override
    public void terminate() {
        // no op
    }

    @Override
    public void joinTermination() {
        // no op
    }

    //
    //
    // Convenience
    //
    /**
     * Returns the first event instance with the given type, or null is there is
     * none.
     */
    private EventInstance get(EventType type) {
        for (EventInstance e : activeEvents) {
            if (e.getEventType().equals(type)) {
                return e;
            }
        }
        return null;
    }

    private List<EventInstance> getAll(EventType type) {
        List<EventInstance> result = new ArrayList<>();
        for (EventInstance e : activeEvents) {
            if (e.getEventType().equals(type)) {
                result.add(e);
            }
        }
        return result;
    }

    /**
     * Finds and removes the first event instance with the given type. Returns
     * null if there is none.
     *
     * @param type
     * @return
     */
    private EventInstance remove(EventType type) {
        for (EventInstance e : activeEvents) {
            if (e.getEventType().equals(type)) {
                activeEvents.remove(e);
                return e;
            }
        }
        return null;
    }

    private void setHandlers(EventInstance evt) {
        List<EventHandlerVO> vos = eventDao.getEventHandlers(evt.getEventType());
        List<EventHandlerRT> rts = null;
        for (EventHandlerVO vo : vos) {
            if (!vo.isDisabled()) {
                if (rts == null) {
                    rts = new ArrayList<>();
                }
                rts.add(vo.createRuntime());
            }
        }
        if (rts != null) {
            evt.setHandlers(rts);
        }
    }

    private void handleRaiseEvent(EventInstance evt, Set<String> defaultAddresses) {
        if (evt.getHandlers() != null) {
            for (EventHandlerRT h : evt.getHandlers()) {
                h.eventRaised(evt);

                // If this is an email handler, remove any addresses to which it was sent from the default addresses
                // so that the default users do not receive multiple notifications.
                if (h instanceof EmailHandlerRT) {
                    for (String addr : ((EmailHandlerRT) h).getActiveRecipients()) {
                        defaultAddresses.remove(addr);
                    }
                }
            }
        }

        if (!defaultAddresses.isEmpty()) {
            // If there are still any addresses left in the list, send them the notification.
            throw new ImplementMeException(); //TODO WAS  EmailHandlerRT.sendActiveEmail(evt, defaultAddresses);
        }
    }

    private void handleInactiveEvent(EventInstance evt) {
        if (evt.getHandlers() != null) {
            for (EventHandlerRT h : evt.getHandlers()) {
                h.eventInactive(evt);
            }
        }
    }

    private boolean isSuppressed(EventType eventType) {
        if (eventType instanceof DataSourceEventType) // Data source events can be suppressed by maintenance events.
        {
            return maintenanceEventManager.isActiveMaintenanceEvent(eventType.getDataSourceId());
        }

        if (eventType instanceof DataPointEventType) // Data point events can be suppressed by maintenance events on their data sources.
        {
            return maintenanceEventManager.isActiveMaintenanceEvent(eventType.getDataSourceId());
        }

        return false;
    }

}
