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
package com.serotonin.mango.vo.event;

import br.org.scadabr.ScadaBrConstants;
import java.util.List;

import org.joda.time.DateTime;

import br.org.scadabr.ShouldNeverHappenException;

import br.org.scadabr.timer.cron.CronExpression;
import br.org.scadabr.timer.cron.CronParser;
import com.serotonin.mango.rt.event.schedule.ScheduledEventRT;
import com.serotonin.mango.rt.event.type.AuditEventType;
import br.org.scadabr.util.ChangeComparable;
import br.org.scadabr.util.StringUtils;
import br.org.scadabr.vo.event.AlarmLevel;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.event.type.ScheduledEventKey;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.serotonin.mango.rt.event.type.ScheduledEventType;
import java.text.ParseException;
import org.springframework.validation.Errors;
import org.springframework.validation.Validator;

/**
 * @author Matthew Lohbihler
 *
 */
public class ScheduledEventVO implements EventDetectorVO, ChangeComparable<ScheduledEventVO> {

    public static class ScheduledEventVoValidator implements Validator {

        @Override
        public boolean supports(Class<?> clazz) {
            return ScheduledEventVO.class.isAssignableFrom(clazz);
        }

        @Override
        public void validate(Object target, Errors errors) {
            final ScheduledEventVO vo = (ScheduledEventVO) target;
            if (vo.alias.length() > 50) {
                errors.rejectValue("alias", "scheduledEvents.validate.aliasTooLong");
            }

            // Check that cron patterns are ok.
            if (vo.scheduleType == ScheduledEventKey.CRON) {
                try {
                    new CronParser().parse(vo.activeCron, CronExpression.TIMEZONE_UTC);
                } catch (ParseException e) {
                    errors.rejectValue("activeCron", "scheduledEvents.validate.activeCron", new Object[]{e}, "scheduledEvents.validate.activeCron");
                }

                if (vo.stateful) {
                    try {
                        new CronParser().parse(vo.inactiveCron, CronExpression.TIMEZONE_UTC);
                    } catch (ParseException e) {
                        errors.rejectValue("inactiveCron", "scheduledEvents.validate.inactiveCron", new Object[]{e}, "scheduledEvents.validate.inactiveCron");
                    }
                }
            }

            // Test that the triggers can be created.
            ScheduledEventRT rt = vo.createRuntime();
            try {
                rt.createTrigger(true);
            } catch (RuntimeException e) {
                errors.rejectValue("activeCron", "scheduledEvents.validate.activeTrigger", e.getMessage());
            }

            if (vo.stateful) {
                try {
                    rt.createTrigger(false);
                } catch (RuntimeException e) {
                    errors.rejectValue("inactiveCron", "scheduledEvents.validate.inactiveTrigger",
                            e.getMessage());
                }
            }

            // If the event is once, make sure the active time is earlier than the inactive time.
            if (vo.scheduleType == ScheduledEventKey.ONCE && vo.stateful) {
                DateTime adt = new DateTime(vo.activeYear, vo.activeMonth, vo.activeDay, vo.activeHour, vo.activeMinute, vo.activeSecond, 0);
                DateTime idt = new DateTime(vo.inactiveYear, vo.inactiveMonth, vo.inactiveDay, vo.inactiveHour, vo.inactiveMinute, vo.inactiveSecond, 0);
                if (idt.getMillis() <= adt.getMillis()) {
                    errors.rejectValue("scheduleType", "scheduledEvents.validate.invalidRtn");
                }
            }
        }

    }

    public static final String XID_PREFIX = "SE_";

    public static String getEventDetectorKey(int id) {
        return EventDetectorVO.SCHEDULED_EVENT_PREFIX + id;
    }

    @JsonIgnore
    public boolean isNew() {
        return id == null;
    }

    private Integer id;
    private String xid;

    private String alias;
    private AlarmLevel alarmLevel = AlarmLevel.NONE;
    private ScheduledEventKey scheduleType = ScheduledEventKey.DAILY;

    private boolean stateful = true;

    private boolean disabled = false;

    private int activeYear;

    private int activeMonth;

    private int activeDay;

    private int activeHour;

    private int activeMinute;

    private int activeSecond;

    private String activeCron;

    private int inactiveYear;

    private int inactiveMonth;

    private int inactiveDay;

    private int inactiveHour;

    private int inactiveMinute;

    private int inactiveSecond;

    private String inactiveCron;

    private ScheduledEventType scheduledEventType;

    public synchronized ScheduledEventType getEventType() {
        if (scheduledEventType == null) {
            scheduledEventType = new ScheduledEventType(this);
        }
        return scheduledEventType;
    }

    public ScheduledEventRT createRuntime() {
        return new ScheduledEventRT(this);
    }

    @Override
    public String getEventDetectorKey() {
        return getEventDetectorKey(id);
    }

    public LocalizableMessage getDescription() {
        if (!alias.isEmpty()) {
            return new LocalizableMessageImpl("common.default", alias);
        }
        switch (scheduleType) {
            case ONCE:
                if (stateful) {
                    return new LocalizableMessageImpl("event.schedule.onceUntil", new DateTime(activeYear, activeMonth, activeDay, activeHour, activeMinute, activeSecond, 0).toDate(),
                            new DateTime(inactiveYear, inactiveMonth, inactiveDay, inactiveHour, inactiveMinute, inactiveSecond, 0).toDate());
                } else {
                    return new LocalizableMessageImpl("event.schedule.onceAt", new DateTime(activeYear, activeMonth, activeDay, activeHour, activeMinute, activeSecond, 0).toDate());
                }
            case HOURLY:
                String activeTime = StringUtils.pad(Integer.toString(activeMinute), '0', 2) + ":"
                        + StringUtils.pad(Integer.toString(activeSecond), '0', 2);
                if (stateful) {
                    return new LocalizableMessageImpl("event.schedule.hoursUntil", activeTime, StringUtils.pad(
                            Integer.toString(inactiveMinute), '0', 2)
                            + ":" + StringUtils.pad(Integer.toString(inactiveSecond), '0', 2));
                } else {
                    return new LocalizableMessageImpl("event.schedule.hoursAt", activeTime);
                }
            case DAILY:
                if (stateful) {
                    return new LocalizableMessageImpl("event.schedule.dailyUntil", activeTime(), inactiveTime());
                } else {
                    return new LocalizableMessageImpl("event.schedule.dailyAt", activeTime());
                }
            case WEEKLY:
                if (stateful) {
                    return new LocalizableMessageImpl("event.schedule.weeklyUntil", weekday(true), activeTime(),
                            weekday(false), inactiveTime());
                } else {
                    return new LocalizableMessageImpl("event.schedule.weeklyAt", weekday(true), activeTime());
                }
            case MONTHLY:
                if (stateful) {
                    return new LocalizableMessageImpl("event.schedule.monthlyUntil", monthday(true), activeTime(),
                            monthday(false), inactiveTime());
                } else {
                    return new LocalizableMessageImpl("event.schedule.monthlyAt", monthday(true), activeTime());
                }
            case YEARLY:
                if (stateful) {
                    return new LocalizableMessageImpl("event.schedule.yearlyUntil", monthday(true), month(true),
                            activeTime(), monthday(false), month(false), inactiveTime());
                } else {
                    return new LocalizableMessageImpl("event.schedule.yearlyAt", monthday(true), month(true), activeTime());
                }
            case CRON:
                if (stateful) {
                    return new LocalizableMessageImpl("event.schedule.cronUntil", activeCron, inactiveCron);
                } else {
                    return new LocalizableMessageImpl("event.schedule.cronAt", activeCron);
                }
            default:
                throw new ShouldNeverHappenException("Unknown schedule type: " + scheduleType);
        }
    }

    private String activeTime() {
        return StringUtils.pad(Integer.toString(activeHour), '0', 2) + ":"
                + StringUtils.pad(Integer.toString(activeMinute), '0', 2) + ":"
                + StringUtils.pad(Integer.toString(activeSecond), '0', 2);
    }

    private String inactiveTime() {
        return StringUtils.pad(Integer.toString(inactiveHour), '0', 2) + ":"
                + StringUtils.pad(Integer.toString(inactiveMinute), '0', 2) + ":"
                + StringUtils.pad(Integer.toString(inactiveSecond), '0', 2);
    }

    private static final String[] weekdays = {"", "common.day.mon", "common.day.tue", "common.day.wed",
        "common.day.thu", "common.day.fri", "common.day.sat", "common.day.sun"};

    private LocalizableMessage weekday(boolean active) {
        int day = activeDay;
        if (!active) {
            day = inactiveDay;
        }
        return new LocalizableMessageImpl(weekdays[day]);
    }

    private LocalizableMessage monthday(boolean active) {
        int day = activeDay;

        if (!active) {
            day = inactiveDay;
        }

        if (day == -3) {
            return new LocalizableMessageImpl("common.day.thirdLast");
        }
        if (day == -2) {
            return new LocalizableMessageImpl("common.day.secondLastLast");
        }
        if (day == -1) {
            return new LocalizableMessageImpl("common.day.last");
        }
        if (day != 11 && day % 10 == 1) {
            return new LocalizableMessageImpl("common.counting.st", Integer.toString(day));
        }
        if (day != 12 && day % 10 == 2) {
            return new LocalizableMessageImpl("common.counting.nd", Integer.toString(day));
        }
        if (day != 13 && day % 10 == 3) {
            return new LocalizableMessageImpl("common.counting.rd", Integer.toString(day));
        }
        return new LocalizableMessageImpl("common.counting.th", Integer.toString(day));
    }

    private static final String[] months = {"", "common.month.jan", "common.month.feb", "common.month.mar",
        "common.month.apr", "common.month.may", "common.month.jun", "common.month.jul", "common.month.aug",
        "common.month.sep", "common.month.oct", "common.month.nov", "common.month.dec"};

    private LocalizableMessage month(boolean active) {
        int day = activeDay;
        if (!active) {
            day = inactiveDay;
        }
        return new LocalizableMessageImpl(months[day]);
    }

    @Override
    public String getTypeKey() {
        return "event.audit.scheduledEvent";
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "common.xid", xid);
        AuditEventType.addPropertyMessage(list, "scheduledEvents.alias", alias);
        AuditEventType.addPropertyMessage(list, "common.alarmLevel", alarmLevel.getI18nKey());
        AuditEventType.addPropertyMessage(list, "scheduledEvents.type", scheduleType);
        AuditEventType.addPropertyMessage(list, "common.rtn", stateful);
        AuditEventType.addPropertyMessage(list, "common.disabled", disabled);
        AuditEventType.addPropertyMessage(list, "common.configuration", getDescription());
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, ScheduledEventVO from) {
        AuditEventType.maybeAddPropertyChangeMessage(list, "common.xid", from.xid, xid);
        AuditEventType.maybeAddPropertyChangeMessage(list, "scheduledEvents.alias", from.alias, alias);
        AuditEventType.maybeAddPropertyChangeMessage(list, "common.alarmLevel", from.alarmLevel, alarmLevel);
        if (from.scheduleType != scheduleType) {
            AuditEventType.addPropertyChangeMessage(list, "scheduledEvents.type", from.scheduleType,
                    scheduleType);
        }
        AuditEventType.maybeAddPropertyChangeMessage(list, "common.rtn", from.stateful, stateful);
        AuditEventType.maybeAddPropertyChangeMessage(list, "common.disabled", from.disabled, disabled);
        if (from.activeYear != activeYear || from.activeMonth != activeMonth || from.activeDay != activeDay
                || from.activeHour != activeHour || from.activeMinute != activeMinute
                || from.activeSecond != activeSecond || (from.activeCron == null ? activeCron != null : !from.activeCron.equals(activeCron))
                || from.inactiveYear != inactiveYear || from.inactiveMonth != inactiveMonth
                || from.inactiveDay != inactiveDay || from.inactiveHour != inactiveHour
                || from.inactiveMinute != inactiveMinute || from.inactiveSecond != inactiveSecond
                || (from.inactiveCron == null ? inactiveCron != null : !from.inactiveCron.equals(inactiveCron))) {
            AuditEventType.maybeAddPropertyChangeMessage(list, "common.configuration", from.getDescription(),
                    getDescription());
        }
    }

    @Override
    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public String getXid() {
        return xid;
    }

    public void setXid(String xid) {
        this.xid = xid;
    }

    public String getAlias() {
        return alias;
    }

    public void setAlias(String alias) {
        this.alias = alias;
    }

    public int getActiveDay() {
        return activeDay;
    }

    public void setActiveDay(int activeDay) {
        this.activeDay = activeDay;
    }

    public int getActiveHour() {
        return activeHour;
    }

    public void setActiveHour(int activeHour) {
        this.activeHour = activeHour;
    }

    public int getActiveMinute() {
        return activeMinute;
    }

    public void setActiveMinute(int activeMinute) {
        this.activeMinute = activeMinute;
    }

    public int getActiveMonth() {
        return activeMonth;
    }

    public void setActiveMonth(int activeMonth) {
        this.activeMonth = activeMonth;
    }

    public int getActiveSecond() {
        return activeSecond;
    }

    public void setActiveSecond(int activeSecond) {
        this.activeSecond = activeSecond;
    }

    public int getActiveYear() {
        return activeYear;
    }

    public void setActiveYear(int activeYear) {
        this.activeYear = activeYear;
    }

    public AlarmLevel getAlarmLevel() {
        return alarmLevel;
    }

    public void setAlarmLevel(AlarmLevel alarmLevel) {
        this.alarmLevel = alarmLevel;
    }

    public int getInactiveDay() {
        return inactiveDay;
    }

    public void setInactiveDay(int inactiveDay) {
        this.inactiveDay = inactiveDay;
    }

    public int getInactiveHour() {
        return inactiveHour;
    }

    public void setInactiveHour(int inactiveHour) {
        this.inactiveHour = inactiveHour;
    }

    public int getInactiveMinute() {
        return inactiveMinute;
    }

    public void setInactiveMinute(int inactiveMinute) {
        this.inactiveMinute = inactiveMinute;
    }

    public int getInactiveMonth() {
        return inactiveMonth;
    }

    public void setInactiveMonth(int inactiveMonth) {
        this.inactiveMonth = inactiveMonth;
    }

    public int getInactiveSecond() {
        return inactiveSecond;
    }

    public void setInactiveSecond(int inactiveSecond) {
        this.inactiveSecond = inactiveSecond;
    }

    public int getInactiveYear() {
        return inactiveYear;
    }

    public void setInactiveYear(int inactiveYear) {
        this.inactiveYear = inactiveYear;
    }

    public boolean isStateful() {
        return stateful;
    }

    public void setStateful(boolean stateful) {
        this.stateful = stateful;
    }

    public ScheduledEventKey getScheduleType() {
        return scheduleType;
    }

    public void setScheduleType(ScheduledEventKey scheduleType) {
        this.scheduleType = scheduleType;
    }

    public String getActiveCron() {
        return activeCron;
    }

    public void setActiveCron(String activeCron) {
        this.activeCron = activeCron;
    }

    public String getInactiveCron() {
        return inactiveCron;
    }

    public void setInactiveCron(String inactiveCron) {
        this.inactiveCron = inactiveCron;
    }

    public boolean isDisabled() {
        return disabled;
    }

    public void setDisabled(boolean disabled) {
        this.disabled = disabled;
    }

}
