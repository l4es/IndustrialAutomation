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
import br.org.scadabr.vo.event.AlarmLevel;
import br.org.scadabr.vo.event.type.ScheduledEventKey;
import com.serotonin.mango.vo.event.ScheduledEventVO;

/**
 * @author Matthew Lohbihler
 *
 */
public class ScheduledEventType extends EventType<ScheduledEventKey> {

    private final int scheduleId;
    private final DuplicateHandling duplicateHandling;
    private final AlarmLevel alarmLevel;
    private final boolean stateful;

    @Deprecated
    public ScheduledEventType(int scheduleId, ScheduledEventKey eventKey, AlarmLevel alarmLevel, boolean stateful) {
        super(eventKey);
        this.scheduleId = scheduleId;
        this.alarmLevel = alarmLevel;
        this.stateful = stateful;
        if (this.stateful) {
            duplicateHandling = DuplicateHandling.ALLOW;
        } else {
            duplicateHandling = DuplicateHandling.IGNORE;
        }
    }

    public ScheduledEventType(ScheduledEventVO vo) {
        super(vo.getScheduleType());
        this.scheduleId = vo.getId();
        this.alarmLevel = vo.getAlarmLevel();
        this.stateful = vo.isStateful();
        if (this.stateful) {
            duplicateHandling = DuplicateHandling.ALLOW;
        } else {
            duplicateHandling = DuplicateHandling.IGNORE;
        }
    }

    @Override
    public EventSources getEventSource() {
        return EventSources.SCHEDULED;
    }

    @Override
    public int getScheduleId() {
        return scheduleId;
    }

    @Override
    public String toString() {
        return "ScheduledEventType(scheduleId=" + scheduleId + ")";
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
        return scheduleId;
    }

    public int getReferenceId2() {
        return 0;
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + scheduleId;
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
        ScheduledEventType other = (ScheduledEventType) obj;
        return scheduleId == other.scheduleId;
    }

    @Override
    public AlarmLevel getAlarmLevel() {
        return alarmLevel;
    }

    /**
     * @return the stateful
     */
    /* TODO
    @Override
    public boolean isStateful() {
        return stateful;
    }
*/
}
