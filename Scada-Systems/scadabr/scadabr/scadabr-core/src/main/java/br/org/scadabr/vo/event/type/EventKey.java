/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo.event.type;

import br.org.scadabr.rt.event.type.DuplicateHandling;
import br.org.scadabr.vo.event.AlarmLevel;
import br.org.scadabr.utils.i18n.LocalizableMessage;

/**
 *
 * @author aploese
 * @param <E>
 */
public interface EventKey <E extends EventKey<E>> extends LocalizableMessage {

    /**
     * a numeric id that survives the storage in the db table...
     * @return 
     */
    int getId();
    
    /**
     * @see Enum.name();
     * @return 
     */
    String name();
    
    String getName();
    
    DuplicateHandling getDuplicateHandling();
    
    AlarmLevel getDefaultAlarmLevel();
    
    /**
     * Is this event causing an alarmstate, or is it just an event for (for instance) logging purpose??
     * @return 
     */
    boolean isStateful();
    
}
