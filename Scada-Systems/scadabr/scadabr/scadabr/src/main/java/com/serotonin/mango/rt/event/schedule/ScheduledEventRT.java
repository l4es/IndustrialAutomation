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
package com.serotonin.mango.rt.event.schedule;

import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.rt.SchedulerPool;
import com.serotonin.mango.rt.event.SimpleEventDetector;
import com.serotonin.mango.rt.event.type.ScheduledEventType;
import com.serotonin.mango.util.timeout.RunWithArgClient;
import com.serotonin.mango.vo.event.ScheduledEventVO;
import br.org.scadabr.timer.cron.CronExpression;
import br.org.scadabr.timer.cron.CronParser;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.event.type.ScheduledEventKey;
import com.serotonin.mango.util.timeout.EventRunWithArgTask;
import java.text.ParseException;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

/**
 * @author Matthew Lohbihler
 *
 */
@Configurable
public class ScheduledEventRT extends SimpleEventDetector implements RunWithArgClient<Boolean> {

    private final ScheduledEventVO vo;
    private ScheduledEventType eventType;
    private boolean eventActive;
    private EventRunWithArgTask<Boolean> activeTask;
    private EventRunWithArgTask<Boolean> inactiveTask;
    @Autowired
    private SchedulerPool schedulerPool;

    public ScheduledEventRT(ScheduledEventVO vo) {
        this.vo = vo;
    }

    public ScheduledEventVO getVo() {
        return vo;
    }

    private void raiseAlarm() {
        raiseAlarm(System.currentTimeMillis());
    }

    private void raiseAlarm(long time) {
        eventType.fire(time, getMessage());
        eventActive = true;
        fireEventDetectorStateChanged(time);
    }

    private void resetAlarm() {
        resetAlarm(System.currentTimeMillis());
    }

    private void resetAlarm(long time) {
        eventType.clearAlarm(time);
        eventActive = false;
        fireEventDetectorStateChanged(time);
    }

    public LocalizableMessage getMessage() {
        return new LocalizableMessageImpl("event.schedule.active", vo.getDescription());
    }

    @Override
    public boolean isEventActive() {
        return eventActive;
    }

    @Override
    synchronized public void run(Boolean active, long fireTime) {
        if (active) {
            raiseAlarm(fireTime);
        } else {
            resetAlarm(fireTime);
        }
    }

    //
    //
    // /
    // / Lifecycle interface
    // /
    //
    //
    @Override
    public void initialize() {
        eventType = new ScheduledEventType(vo);

        // Schedule the active event.
        CronExpression activeTrigger = createTrigger(true);
        activeTask = new EventRunWithArgTask<>(activeTrigger, this, true);
        schedulerPool.schedule(activeTask);

        if (vo.isStateful()) {
            CronExpression inactiveTrigger = createTrigger(false);
            inactiveTask = new EventRunWithArgTask<>(inactiveTrigger, this, false);
            schedulerPool.schedule(inactiveTask);

            if (vo.getScheduleType() != ScheduledEventKey.ONCE) {
                // Check if we are currently active.
                if (inactiveTask.getNextScheduledExecutionTime() < activeTask.getNextScheduledExecutionTime()) {
                    raiseAlarm();
                }
            }
        }
    }

    @Override
    public void terminate() {
        fireEventDetectorTerminated();
        if (activeTask != null) {
            activeTask.cancel();
        }
        if (inactiveTask != null) {
            inactiveTask.cancel();
        }
        resetAlarm();
    }

    @Override
    public void joinTermination() {
        // no op
    }

    public CronExpression createTrigger(boolean activeTrigger) {
        if (!activeTrigger && !vo.isStateful()) {
            return null;
        }

        final int month = activeTrigger ? vo.getActiveMonth() : vo.getInactiveMonth();
        final int day = activeTrigger ? vo.getActiveDay() : vo.getInactiveDay();
        final int hour = activeTrigger ? vo.getActiveHour() : vo.getInactiveHour();
        final int minute = activeTrigger ? vo.getActiveMinute() : vo.getInactiveMinute();
        final int second = activeTrigger ? vo.getActiveSecond() : vo.getInactiveSecond();
        switch (vo.getScheduleType()) {

            case CRON:
                try {
                    if (activeTrigger) {
                        return new CronParser().parse(vo.getActiveCron(), CronExpression.TIMEZONE_UTC);
                    }
                    return new CronParser().parse(vo.getInactiveCron(), CronExpression.TIMEZONE_UTC);
                } catch (ParseException e) {
                    // Should never happen, so wrap and rethrow
                    throw new ShouldNeverHappenException(e);
                }

            case ONCE:
                if (activeTrigger) {
                    return new CronExpression(vo.getActiveYear(), vo.getActiveMonth(), vo.getActiveDay(), vo.getActiveHour(),
                            vo.getActiveMinute(), vo.getActiveSecond(), 0, CronExpression.TIMEZONE_UTC);
                } else {
                    return new CronExpression(vo.getInactiveYear(), vo.getInactiveMonth(), vo.getInactiveDay(),
                            vo.getInactiveHour(), vo.getInactiveMinute(), vo.getInactiveSecond(), 0, CronExpression.TIMEZONE_UTC);
                }
            case HOURLY:
                return CronExpression.createPeriodByHour(1, minute, second, 0);
            case DAILY:
                return CronExpression.createDaily(hour, minute, second, 0);
            case WEEKLY:
                throw new ImplementMeException();
            case MONTHLY:
                throw new ImplementMeException();
            default:
                throw new RuntimeException();
        }
    }
}
