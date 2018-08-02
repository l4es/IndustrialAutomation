/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo.event.type;

import br.org.scadabr.rt.event.type.DuplicateHandling;
import br.org.scadabr.vo.event.AlarmLevel;

/**
 *
 * @author aploese
 * @param <E>
 */
public interface DataSourceEventKey<E extends DataSourceEventKey<E>> extends EventKey<E> {
    /**
     * Default for DataSourceEvent is IGNORE
     */
    public final static DuplicateHandling DEFAULT_DUPLICATE_HANDLING = DuplicateHandling.IGNORE;
    /**
     * Default for DataSourceEvent is URGEN
     */
    public final static AlarmLevel DEFAULT_ALARMLEVEL =  AlarmLevel.URGENT;
    
}