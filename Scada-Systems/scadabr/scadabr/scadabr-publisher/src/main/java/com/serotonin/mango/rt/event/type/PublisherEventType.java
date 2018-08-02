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


import br.org.scadabr.rt.event.type.EventSources;
import br.org.scadabr.vo.event.AlarmLevel;
import br.org.scadabr.vo.event.type.PublisherEventKey;
import com.serotonin.mango.vo.publish.PublisherVO;

/**
 * @author Matthew Lohbihler
 */

public class PublisherEventType extends EventType<PublisherEventKey> {

    private final int publisherId;
    private final AlarmLevel alarmLevel;

    @Deprecated
    public PublisherEventType(int publisherId, PublisherEventKey publisherEventKey, AlarmLevel alarmLevel) {
        super(publisherEventKey);
        this.publisherId = publisherId;
        this.alarmLevel = alarmLevel;
    }

    public PublisherEventType(PublisherVO vo) {
        super(vo.getPublisherEventKey());
        this.publisherId = vo.getId();
        this.alarmLevel = AlarmLevel.URGENT;
    }

    @Override
    public EventSources getEventSource() {
        return EventSources.PUBLISHER;
    }

    @Override
    public int getPublisherId() {
        return publisherId;
    }

    @Override
    public String toString() {
        return "PublisherEventType(publisherId=" + publisherId + ", eventTypeId=" + eventKey.getId() + ")";
    }

    /* TODO
    @Override
    public DuplicateHandling getDuplicateHandling() {
        return DuplicateHandling.IGNORE;
    }
*/
    public int getReferenceId1() {
        return publisherId;
    }

    public int getReferenceId2() {
        return eventKey.getId();
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + eventKey.getId();
        result = prime * result + publisherId;
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
        PublisherEventType other = (PublisherEventType) obj;
        if (eventKey.getId() != other.eventKey.getId()) {
            return false;
        }
        if (publisherId != other.publisherId) {
            return false;
        }
        return true;
    }

    @Override
    public AlarmLevel getAlarmLevel() {
        return alarmLevel;
    }

}
