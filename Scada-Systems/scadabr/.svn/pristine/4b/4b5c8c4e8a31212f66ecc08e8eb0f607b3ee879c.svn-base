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


import br.org.scadabr.rt.event.type.DuplicateHandling;
import br.org.scadabr.rt.event.type.EventSources;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.vo.event.AlarmLevel;
import br.org.scadabr.vo.event.type.CompoundEventKey;
import com.serotonin.mango.vo.event.CompoundEventDetectorVO;

/**
 * @author Matthew Lohbihler
 */

public class CompoundDetectorEventType extends EventType<CompoundEventKey> {
    
    private final int compoundDetectorId;
    private final DuplicateHandling duplicateHandling = DuplicateHandling.IGNORE;
    private final AlarmLevel alarmLevel;
    private final boolean stateful;

    @Deprecated
    public CompoundDetectorEventType(int compoundDetectorId, CompoundEventKey eventKey, AlarmLevel alarmLevel, boolean stateful) {
        super(eventKey);
        this.compoundDetectorId = compoundDetectorId;
        this.alarmLevel = alarmLevel;
        this.stateful = stateful;
        throw new ImplementMeException();
/*        
        this.alarmLevel = vo.getAlarmLevel();
        if (!vo.isStateful()) {
            duplicateHandling = DuplicateHandling.ALLOW;
        }
        */
    }

    public CompoundDetectorEventType(CompoundEventDetectorVO vo) {
        super(vo.getCompoundEventKey());
        this.compoundDetectorId = vo.getId();
        this.alarmLevel = vo.getAlarmLevel();
        this.stateful = vo.isStateful();
        throw new ImplementMeException();
/*        if (!vo.isStateful()) {
            duplicateHandling = DuplicateHandling.ALLOW;
        }
        */
    }

    @Override
    public EventSources getEventSource() {
        return EventSources.COMPOUND;
    }

    public int getCompoundDetectorId() {
        return compoundDetectorId;
    }

    @Override
    public String toString() {
        return "CompoundDetectorEventType(compoundDetectorId=" + compoundDetectorId + ")";
    }

    /* TODO
    @Override
    public DuplicateHandling getDuplicateHandling() {
        return duplicateHandling;
    }

    public void setDuplicateHandling(DuplicateHandling duplicateHandling) {
        this.duplicateHandling = duplicateHandling;
    }
*/
    public int getReferenceId1() {
        return compoundDetectorId;
    }

    public int getReferenceId2() {
        return 0;
    }

    @Override
    public int getCompoundEventDetectorId() {
        return compoundDetectorId;
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + compoundDetectorId;
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
        CompoundDetectorEventType other = (CompoundDetectorEventType) obj;
        if (compoundDetectorId != other.compoundDetectorId) {
            return false;
        }
        return true;
    }

    @Override
    public AlarmLevel getAlarmLevel() {
        return alarmLevel;
    }

    /* TODO
    @Override
    public boolean isStateful() {
        return stateful;
    }
    */

}
