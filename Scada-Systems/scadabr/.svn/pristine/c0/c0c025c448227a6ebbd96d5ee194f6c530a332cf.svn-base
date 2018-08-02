package com.serotonin.mango.rt.event.maintenance;

import br.org.scadabr.utils.ImplementMeException;
import java.text.ParseException;

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.rt.SchedulerPool;
import com.serotonin.mango.rt.event.type.MaintenanceEventType;
import com.serotonin.mango.util.timeout.RunWithArgClient;
import com.serotonin.mango.util.timeout.SystemRunWithArgTask;
import com.serotonin.mango.vo.event.MaintenanceEventVO;
import br.org.scadabr.timer.cron.CronExpression;
import br.org.scadabr.timer.cron.CronParser;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.event.type.MaintenanceEventKey;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

@Configurable
public class MaintenanceEventRT implements RunWithArgClient<Boolean> {

    private final MaintenanceEventVO vo;
    private MaintenanceEventType eventType;
    private boolean eventActive;
    private SystemRunWithArgTask<Boolean> activeTask;
    private SystemRunWithArgTask<Boolean> inactiveTask;

    @Autowired
    private SchedulerPool schedulerPool;

    public MaintenanceEventRT(MaintenanceEventVO vo) {
        this.vo = vo;
    }

    public MaintenanceEventVO getVo() {
        return vo;
    }

    private void raiseEvent(long time) {
        if (!eventActive) {
            eventType.fire(time, getMessage());
            eventActive = true;
        }
    }

    private void returnToNormal(long time) {
        if (eventActive) {
            eventType.clearAlarm(time);
            eventActive = false;
        }
    }

    public LocalizableMessage getMessage() {
        return new LocalizableMessageImpl("event.maintenance.active", vo.getDescription());
    }

    public boolean isEventActive() {
        return eventActive;
    }

    public boolean toggle() {
        run(!eventActive, System.currentTimeMillis());
        return eventActive;
    }

    @Override
    synchronized public void run(Boolean active, long fireTime) {
        if (active) {
            raiseEvent(fireTime);
        } else {
            returnToNormal(fireTime);
        }
    }

    //
    //
    // Lifecycle interface
    //
    public void initialize() {
        eventType = new MaintenanceEventType(vo);

        if (vo.getScheduleType() != MaintenanceEventKey.MANUAL) {
            // Schedule the active event.
            final CronExpression activeTrigger = createTrigger(true);
            activeTask = new SystemRunWithArgTask<>(activeTrigger, this, true);
            schedulerPool.schedule(activeTask);

            // Schedule the inactive event
            final CronExpression inactiveTrigger = createTrigger(false);
            inactiveTask = new SystemRunWithArgTask<>(inactiveTrigger, this, false);
            schedulerPool.schedule(inactiveTask);

            if (vo.getScheduleType() != MaintenanceEventKey.ONCE) {
                // Check if we are currently active.
                if (inactiveTask.getNextScheduledExecutionTime() < activeTask.getNextScheduledExecutionTime()) {
                    raiseEvent(System.currentTimeMillis());
                }
            }
        }
    }

    public void terminate() {
        if (activeTask != null) {
            activeTask.cancel();
        }
        if (inactiveTask != null) {
            inactiveTask.cancel();
        }

        if (eventActive) {
            eventType.disableAlarm();
        }
    }

    public void joinTermination() {
        // no op
    }

    public CronExpression createTrigger(boolean activeTrigger) {
        final int month = activeTrigger ? vo.getActiveMonth() : vo.getInactiveMonth();
        final int day = activeTrigger ? vo.getActiveDay() : vo.getInactiveDay();
        final int hour = activeTrigger ? vo.getActiveHour() : vo.getInactiveHour();
        final int minute = activeTrigger ? vo.getActiveMinute() : vo.getInactiveMinute();
        final int second = activeTrigger ? vo.getActiveSecond() : vo.getInactiveSecond();
        switch (vo.getScheduleType()) {
            case MANUAL:
                return null;

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
                throw new ImplementMeException();
            case WEEKLY:
                throw new ImplementMeException();
            case MONTHLY:
                throw new ImplementMeException();
            default:
                throw new RuntimeException();
        }
    }
}
