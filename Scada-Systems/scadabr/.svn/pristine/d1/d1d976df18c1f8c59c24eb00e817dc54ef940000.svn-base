/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.dao;

import br.org.scadabr.rt.UserRT;
import com.serotonin.mango.rt.event.AlternateAcknowledgementSources;
import com.serotonin.mango.rt.event.EventInstance;
import com.serotonin.mango.rt.event.type.EventType;
import com.serotonin.mango.vo.event.EventHandlerVO;
import java.util.Collection;
import java.util.List;
import javax.inject.Named;

/**
 *
 * @author aploese
 */
@Named
public interface EventDao {

    void saveEvent(EventInstance evt);

    void insertUserEvents(int id, List<Integer> eventUserIds, boolean alarm);

    void ackEvent(int eventId, long ackTs, UserRT user, AlternateAcknowledgementSources alternateAckSource);

    Collection<EventInstance> getActiveEvents();

    List<EventHandlerVO> getEventHandlers(EventType eventType);

    Collection<EventInstance> getPendingEvents(UserRT user);

    int purgeEventsBefore(long millis);

    EventInstance getEventInstance(int id);
    
}
