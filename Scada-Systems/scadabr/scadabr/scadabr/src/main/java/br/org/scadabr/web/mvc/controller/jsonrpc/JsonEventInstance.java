package br.org.scadabr.web.mvc.controller.jsonrpc;

import br.org.scadabr.l10n.Localizer;
import br.org.scadabr.vo.event.AlarmLevel;
import br.org.scadabr.vo.event.EventStatus;
import com.serotonin.mango.rt.event.EventInstance;
import java.util.ArrayList;
import java.util.Collection;
import java.util.LinkedList;
import java.util.List;

/**
 *
 * @author aploese
 */
public class JsonEventInstance {

    private int id;
    private AlarmLevel alarmLevel;
    private long fireTimestamp;
    private Long inactiveTimestamp;
    private String message;
    private EventStatus eventState;
    private String stateMessage;
    private boolean acknowledged;

    /**
     * @return the id
     */
    public int getId() {
        return id;
    }

    /**
     * @param id the id to set
     */
    public void setId(int id) {
        this.id = id;
    }

    /**
     * @return the alarmLevel
     */
    public AlarmLevel getAlarmLevel() {
        return alarmLevel;
    }

    /**
     * @param alarmLevel the alarmLevel to set
     */
    public void setAlarmLevel(AlarmLevel alarmLevel) {
        this.alarmLevel = alarmLevel;
    }

    /**
     * @return the message
     */
    public String getMessage() {
        return message;
    }

    /**
     * @param message the message to set
     */
    public void setMessage(String message) {
        this.message = message;
    }

    /**
     * @return the stateMessage
     */
    public String getStateMessage() {
        return stateMessage;
    }

    /**
     * @param stateMessage the stateMessage to set
     */
    public void setStateMessage(String stateMessage) {
        this.stateMessage = stateMessage;
    }

    /**
     * @return the acknowledged
     */
    public boolean isAcknowledged() {
        return acknowledged;
    }

    /**
     * @param acknowledged the acknowledged to set
     */
    public void setAcknowledged(boolean acknowledged) {
        this.acknowledged = acknowledged;
    }

    public static JsonEventInstance wrap(EventInstance eventInstance, Localizer localizer) {
        final JsonEventInstance result = new JsonEventInstance();
        result.setId(eventInstance.getId());
        result.setEventState(eventInstance.getEventState());
        result.setAlarmLevel(eventInstance.getAlarmLevel());
        result.setFireTimestamp(eventInstance.getFireTimestamp());
        result.setStateMessage(localizer.getMessage(eventInstance.getStateMessage()));
        if (eventInstance.isGone()) {
            result.setInactiveTimestamp(eventInstance.getGoneTimestamp());
        }
        result.setAcknowledged(eventInstance.isAcknowledged());
        result.setMessage(localizer.getMessage(eventInstance.getMessage()));
        return result;
    }

    public static Collection<JsonEventInstance> wrap(Iterable<EventInstance> eventInstances, Localizer localizer) {
        List<JsonEventInstance> result = new LinkedList<>();
        for (EventInstance ei : eventInstances) {
            result.add(JsonEventInstance.wrap(ei, localizer));
        }
        return result;
    }

    /**
     * @return the fireTimestamp
     */
    public long getFireTimestamp() {
        return fireTimestamp;
    }

    /**
     * @param fireTimestamp the fireTimestamp to set
     */
    public void setFireTimestamp(long fireTimestamp) {
        this.fireTimestamp = fireTimestamp;
    }

    /**
     * @return the inactiveTimestamp
     */
    public Long getInactiveTimestamp() {
        return inactiveTimestamp;
    }

    /**
     * @param inactiveTimestamp the inactiveTimestamp to set
     */
    public void setInactiveTimestamp(Long inactiveTimestamp) {
        this.inactiveTimestamp = inactiveTimestamp;
    }

    /**
     * @return the eventState
     */
    public EventStatus getEventState() {
        return eventState;
    }

    /**
     * @param eventState the eventState to set
     */
    public void setEventState(EventStatus eventState) {
        this.eventState = eventState;
    }

}
