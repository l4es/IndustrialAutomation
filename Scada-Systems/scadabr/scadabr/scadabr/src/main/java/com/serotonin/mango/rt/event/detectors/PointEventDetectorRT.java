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
package com.serotonin.mango.rt.event.detectors;

import java.util.HashMap;
import java.util.Map;

import com.serotonin.mango.rt.dataImage.DataPointListener;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.event.SimpleEventDetector;
import com.serotonin.mango.rt.event.type.DataPointEventType;
import com.serotonin.mango.vo.event.DoublePointEventDetectorVO;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;

abstract public class PointEventDetectorRT<T extends PointValueTime> extends SimpleEventDetector implements DataPointListener<T> {

    //TODO make this final ...
    protected DoublePointEventDetectorVO vo;

    protected DataPointEventType getEventType() {
        return new DataPointEventType(vo);
    }

    protected void raiseAlarm(long time, Map<String, Object> context) {
        LocalizableMessage msg;
        if (!vo.getAlias().isEmpty()) {
            msg = new LocalizableMessageImpl("common.default", vo.getAlias());
        } else {
            msg = getMessage();
        }

        getEventType().fire(context, time, msg);
        fireEventDetectorStateChanged(time);
    }

    protected void clearAlarm(long time) {
        getEventType().clearAlarm(time);
        fireEventDetectorStateChanged(time);
    }

    protected Map<String, Object> createEventContext() {
        Map<String, Object> context = new HashMap<>();
        context.put("pointEventDetector", vo);
        context.put("point", vo.njbGetDataPoint());
        return context;
    }

    abstract protected LocalizableMessage getMessage();

    public String getEventDetectorKey() {
        return vo.getEventDetectorKey();
    }

    //
    //
    // Lifecycle interface
    //
    @Override
    public void initialize() {
        // no op
    }

    @Override
    public void terminate() {
        fireEventDetectorTerminated();
    }

    @Override
    public void joinTermination() {
        // no op
    }

    //
    //
    // Point listener interface
    //
    @Override
    public void pointChanged(T oldValue, T newValue) {
        // no op
    }

    @Override
    public void pointSet(T oldValue, T newValue) {
        // no op
    }

    @Override
    public void pointUpdated(T newValue) {
        // no op
    }

    @Override
    public void pointBackdated(T value) {
        // no op
    }

    @Override
    public void pointInitialized() {
        // no op
    }

    @Override
    public void pointTerminated() {
        // no op
    }
}
