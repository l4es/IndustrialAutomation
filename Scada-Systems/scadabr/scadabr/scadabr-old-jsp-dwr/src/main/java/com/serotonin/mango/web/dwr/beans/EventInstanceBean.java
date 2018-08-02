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
package com.serotonin.mango.web.dwr.beans;

import br.org.scadabr.vo.event.AlarmLevel;

/**
 * @author Matthew Lohbihler
 */
public class EventInstanceBean {

    private final boolean active;
    private final AlarmLevel alarmLevel;
    private final long activeTimestamp;
    private final String message;

    public EventInstanceBean(boolean active, AlarmLevel alarmLevel, long activeTimestamp, String message) {
        this.active = active;
        this.alarmLevel = alarmLevel;
        this.activeTimestamp = activeTimestamp;
        this.message = message;
    }

    public boolean isActive() {
        return active;
    }

    public AlarmLevel getAlarmLevel() {
        return alarmLevel;
    }

    public long getActiveTimestamp() {
        return activeTimestamp;
    }

    public String getMessage() {
        return message;
    }
}
