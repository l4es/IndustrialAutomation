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
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.vo.event.AlarmLevel;
import br.org.scadabr.vo.event.type.DataPointDetectorKey;
import com.serotonin.mango.vo.event.DoublePointEventDetectorVO;


public class DataPointEventType extends EventType<DataPointDetectorKey> {

    private final int dataPointId;
    private AlarmLevel alarmLevel;
    
    @Deprecated
    public DataPointEventType(int dataPointId, DataPointDetectorKey pointDetectorKey, AlarmLevel alarmLevel) {
        super(pointDetectorKey);
        this.dataPointId = dataPointId;
        this.alarmLevel = alarmLevel;
    }

    public DataPointEventType(DoublePointEventDetectorVO vo) {
        super(vo.getDataPointDetectorKey());
        this.dataPointId = vo.njbGetDataPoint().getId();
        this.alarmLevel = vo.getAlarmLevel();
    }

    @Override
    public EventSources getEventSource() {
        return EventSources.DATA_POINT;
    }

    @Override
    @Deprecated //TODO fethch if needed
    public int getDataSourceId() {
        throw new ImplementMeException();
        /*
        if (dataSourceId == -1) {
            dataSourceId = dataPointDao.getDataPoint(dataPointId).getDataSourceId();
        }
        return dataSourceId;
        */
    }

    @Override
    public int getDataPointId() {
        return dataPointId;
    }

    public int getPointEventDetectorId() {
        return eventKey.getId();
    }

    @Override
    public String toString() {
        return "DataPointEventType(dataPointId=" + dataPointId + ", detectorId=" + getPointEventDetectorId() + ")";
    }

    public int getReferenceId1() {
        return dataPointId;
    }

    public int getReferenceId2() {
        return getPointEventDetectorId();
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + dataPointId;
        result = prime * result + getPointEventDetectorId();
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
        DataPointEventType other = (DataPointEventType) obj;
        if (dataPointId != other.dataPointId) {
            return false;
        }
        return eventKey.getId() == other.eventKey.getId();
    }

    @Override
    public AlarmLevel getAlarmLevel() {
        return alarmLevel;
    }

}
