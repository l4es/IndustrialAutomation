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
import br.org.scadabr.vo.event.type.DataSourceEventKey;


public class DataSourceEventType<E extends DataSourceEventKey<E>> extends EventType<E> {

    private final int dataSourceId;
    private final AlarmLevel alarmLevel;
    
    public DataSourceEventType(int dataSourceId, E dataSourceEventKey, AlarmLevel alarmLevel) {
        super(dataSourceEventKey);
        this.dataSourceId = dataSourceId;
        this.alarmLevel = alarmLevel;
    }

    @Override
    public EventSources getEventSource() {
        return EventSources.DATA_SOURCE;
    }

    @Override
    public AlarmLevel getAlarmLevel() {
        return alarmLevel;
    }

    @Override
    public int getDataSourceId() {
        return dataSourceId;
    }

    @Override
    public String toString() {
        return "DataSoureEventType(dataSourceId=" + dataSourceId + ", eventKey=" + eventKey + ")";
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + eventKey.getId();
        result = prime * result + dataSourceId;
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
        DataSourceEventType other = (DataSourceEventType) obj;
        if (!eventKey.equals(other.eventKey)) {
            return false;
        }
        return dataSourceId == other.dataSourceId;
    }

    @Override
    public int getReferenceId1() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public int getReferenceId2() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

}
