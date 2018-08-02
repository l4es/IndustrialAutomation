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
package com.serotonin.mango.rt.event.handlers;

import br.org.scadabr.DataType;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.l10n.AbstractLocalizer;
import br.org.scadabr.rt.SchedulerPool;
import br.org.scadabr.utils.ImplementMeException;
import com.serotonin.mango.Common;
import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.rt.event.EventInstance;
import com.serotonin.mango.rt.event.type.EventType;
import com.serotonin.mango.rt.event.type.SystemEventType;
import com.serotonin.mango.rt.maint.work.SetPointWorkItem;
import com.serotonin.mango.vo.event.EventHandlerVO;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.event.type.SystemEventKey;
import com.serotonin.mango.rt.RuntimeManager;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

@Configurable
public class SetPointHandlerRT extends EventHandlerRT {

    private static final Log LOG = LogFactory.getLog(SetPointHandlerRT.class);
    @Autowired
    private RuntimeManager runtimeManager;
    @Autowired
    private SchedulerPool schedulerPool;

    public SetPointHandlerRT(EventHandlerVO vo) {
        this.vo = vo;
    }

    @Override
    public void eventRaised(EventInstance evt) {
        if (vo.getActiveAction() == EventHandlerVO.SetActionType.NONE) {
            return;
        }

        // Validate that the target point is available.
        DataPointRT targetPoint = runtimeManager.getDataPoint(vo.getTargetPointId());
        if (targetPoint == null) {
            raiseFailureEvent(new LocalizableMessageImpl("event.setPoint.targetPointMissing"), evt.getEventType());
            return;
        }

        if (!targetPoint.getPointLocator().isSettable()) {
            raiseFailureEvent(new LocalizableMessageImpl("event.setPoint.targetNotSettable"), evt.getEventType());
            return;
        }

        final DataType targetDataType = targetPoint.getDataType();

        MangoValue value;
        if (vo.getActiveAction() == EventHandlerVO.SetActionType.POINT_VALUE) {
            // Get the source data point.
            DataPointRT sourcePoint = runtimeManager.getDataPoint(vo.getActivePointId());
            if (sourcePoint == null) {
                raiseFailureEvent(new LocalizableMessageImpl("event.setPoint.activePointMissing"), evt.getEventType());
                return;
            }

            PointValueTime valueTime = sourcePoint.getPointValue();
            if (valueTime == null) {
                raiseFailureEvent(new LocalizableMessageImpl("event.setPoint.activePointValue"), evt.getEventType());
                return;
            }

            if (valueTime.getDataType() != targetDataType) {
                raiseFailureEvent(new LocalizableMessageImpl("event.setPoint.activePointDataType"), evt.getEventType());
                return;
            }

            value = valueTime.toMangoValue();
        } else if (vo.getActiveAction() == EventHandlerVO.SetActionType.STATIC_VALUE) {
            value = MangoValue.stringToValue(vo.getActiveValueToSet(), targetDataType);
        } else {
            throw new ShouldNeverHappenException("Unknown active action: " + vo.getActiveAction());
        }

        // Queue a work item to perform the set point.
       throw new ImplementMeException();
        //TODO schedulerPool.execute(new SetPointWorkItem(vo.getTargetPointId(), PointValueTime.fromMangoValue(value, vo.getTargetPointId(), evt.getFireTimestamp()), this));
    }

    @Override
    public void eventInactive(EventInstance evt) {
        if (vo.getInactiveAction() == EventHandlerVO.SetActionType.NONE) {
            return;
        }

        // Validate that the target point is available.
        DataPointRT targetPoint = runtimeManager.getDataPoint(vo.getTargetPointId());
        if (targetPoint == null) {
            raiseFailureEvent(new LocalizableMessageImpl("event.setPoint.targetPointMissing"), evt.getEventType());
            return;
        }

        if (!targetPoint.getPointLocator().isSettable()) {
            raiseFailureEvent(new LocalizableMessageImpl("event.setPoint.targetNotSettable"), evt.getEventType());
            return;
        }

        final DataType targetDataType = targetPoint.getDataType();

        MangoValue value;
        if (vo.getInactiveAction() == EventHandlerVO.SetActionType.POINT_VALUE) {
            // Get the source data point.
            DataPointRT sourcePoint = runtimeManager.getDataPoint(vo.getInactivePointId());
            if (sourcePoint == null) {
                raiseFailureEvent(new LocalizableMessageImpl("event.setPoint.inactivePointMissing"), evt.getEventType());
                return;
            }

            PointValueTime valueTime = sourcePoint.getPointValue();
            if (valueTime == null) {
                raiseFailureEvent(new LocalizableMessageImpl("event.setPoint.inactivePointValue"), evt.getEventType());
                return;
            }

            if (valueTime.getDataType() != targetDataType) {
                raiseFailureEvent(new LocalizableMessageImpl("event.setPoint.inactivePointDataType"), evt.getEventType());
                return;
            }

            value = valueTime.toMangoValue();
        } else if (vo.getInactiveAction() == EventHandlerVO.SetActionType.STATIC_VALUE) {
            value = MangoValue.stringToValue(vo.getInactiveValueToSet(), targetDataType);
        } else {
            throw new ShouldNeverHappenException("Unknown active action: " + vo.getInactiveAction());
        }
throw new ImplementMeException();
//TODO        schedulerPool.execute(new SetPointWorkItem(vo.getTargetPointId(), PointValueTime.fromMangoValue(value, vo.getTargetPointId(), evt.getGoneTimestamp()), this));
    }

    private void raiseFailureEvent(LocalizableMessage message, EventType et) {
        if (et != null) {
            if (et.getEventKey() == SystemEventKey.SET_POINT_HANDLER_FAILURE) {
                // The set point attempt failed for an event that is a set point handler failure in the first place.
                // Do not propagate the event, but rather just write a log message.
                LOG.warn("A set point event due to a set point handler failure itself failed. The failure event "
                        + "has been discarded: " + AbstractLocalizer.localizeMessage(message, Common.getBundle()));
                return;
            }
        }

        final SystemEventType eventType = new SystemEventType(SystemEventKey.SET_POINT_HANDLER_FAILURE, vo.getId());
        if (vo.getAlias().isEmpty()) {
            eventType.fire("event.setPointFailed", message);
        } else {
            eventType.fire("event.setPointFailed.alias", vo.getAlias(), message);
        }
    }

//TODO    @Override
    public void raiseRecursionFailureEvent() {
        raiseFailureEvent(new LocalizableMessageImpl("event.setPoint.recursionFailure"), null);
    }

}
