package com.serotonin.mango.vo.event;

import br.org.scadabr.ScadaBrConstants;
import java.util.List;

import org.joda.time.DateTime;

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.timer.cron.CronExpression;
import br.org.scadabr.timer.cron.CronParser;
import com.serotonin.mango.rt.event.type.AuditEventType;
import br.org.scadabr.util.ChangeComparable;
import br.org.scadabr.util.StringUtils;
import br.org.scadabr.vo.event.AlarmLevel;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.event.type.MaintenanceEventKey;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.serotonin.mango.rt.event.maintenance.MaintenanceEventRT;
import com.serotonin.mango.rt.event.type.MaintenanceEventType;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import org.springframework.validation.Errors;
import org.springframework.validation.Validator;

public class MaintenanceEventVO implements ChangeComparable<MaintenanceEventVO> {

    public static class MaintenenanceEventVoValidator implements Validator {

        @Override
        public boolean supports(Class<?> clazz) {
            return DataSourceVO.class.isAssignableFrom(clazz);
        }

        @Override
        public void validate(Object target, Errors errors) {
            final MaintenanceEventVO vo = (MaintenanceEventVO) target;
            if (vo.alias.length() > 50) {
                errors.rejectValue("alias", "maintenanceEvents.validate.aliasTooLong");
            }

            if (vo.dataSourceId <= 0) {
                errors.rejectValue("dataSourceId", "validate.invalidValue");
            }

            // Check that cron patterns are ok.
            if (vo.scheduleType == MaintenanceEventKey.CRON) {
                try {
                    new CronParser().parse(vo.activeCron, CronExpression.TIMEZONE_UTC);
                } catch (Exception e) {
                    errors.rejectValue("activeCron", "maintenanceEvents.validate.activeCron", new Object[]{e}, "maintenanceEvents.validate.activeCron");
                }

                try {
                    new CronParser().parse(vo.inactiveCron, CronExpression.TIMEZONE_UTC);
                } catch (Exception e) {
                    errors.rejectValue("inactiveCron", "maintenanceEvents.validate.inactiveCron", new Object[]{e}, "maintenanceEvents.validate.inactiveCron");
                }
            }

            // Test that the triggers can be created.
            MaintenanceEventRT rt = new MaintenanceEventRT(vo);
            try {
                rt.createTrigger(true);
            } catch (RuntimeException e) {
                errors.rejectValue("activeCron", "maintenanceEvents.validate.activeTrigger", new Object[]{e}, "maintenanceEvents.validate.activeTrigger");
            }

            try {
                rt.createTrigger(false);
            } catch (RuntimeException e) {
                errors.rejectValue("inactiveCron", "maintenanceEvents.validate.inactiveTrigger", new Object[]{e}, "maintenanceEvents.validate.inactiveTrigger");
            }

            // If the event is once, make sure the active time is earlier than the inactive time.
            if (vo.scheduleType == MaintenanceEventKey.ONCE) {
                DateTime adt = new DateTime(vo.activeYear, vo.activeMonth, vo.activeDay, vo.activeHour, vo.activeMinute, vo.activeSecond, 0);
                DateTime idt = new DateTime(vo.inactiveYear, vo.inactiveMonth, vo.inactiveDay, vo.inactiveHour, vo.inactiveMinute, vo.inactiveSecond, 0);
                if (idt.getMillis() <= adt.getMillis()) {
                    errors.rejectValue("scheduleType", "maintenanceEvents.validate.invalidRtn");
                }
            }
        }

    }

    public static final String XID_PREFIX = "ME_";

    private Integer id;
    private String xid;
    private int dataSourceId;

    private String alias;
    private AlarmLevel alarmLevel = AlarmLevel.NONE;
    private MaintenanceEventKey scheduleType = MaintenanceEventKey.MANUAL;

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

    //
    //
    // Convenience data from data source
    //
    private int dataSourceTypeId;
    private String dataSourceName;
    private String dataSourceXid;
    private MaintenanceEventType maintenanceEventType;

    public synchronized MaintenanceEventType getEventType() {
        if (maintenanceEventType == null) {
            maintenanceEventType = new MaintenanceEventType(this);
        }
        return maintenanceEventType;
    }

    @JsonIgnore
    public boolean isNew() {
        return id == null;
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

    public int getDataSourceId() {
        return dataSourceId;
    }

    public void setDataSourceId(int dataSourceId) {
        this.dataSourceId = dataSourceId;
    }

    public String getAlias() {
        return alias;
    }

    public void setAlias(String alias) {
        this.alias = alias;
    }

    public AlarmLevel getAlarmLevel() {
        return alarmLevel;
    }

    public void setAlarmLevel(AlarmLevel alarmLevel) {
        this.alarmLevel = alarmLevel;
    }

    public MaintenanceEventKey getScheduleType() {
        return scheduleType;
    }

    public void setScheduleType(MaintenanceEventKey scheduleType) {
        this.scheduleType = scheduleType;
    }

    public boolean isDisabled() {
        return disabled;
    }

    public void setDisabled(boolean disabled) {
        this.disabled = disabled;
    }

    public int getActiveYear() {
        return activeYear;
    }

    public void setActiveYear(int activeYear) {
        this.activeYear = activeYear;
    }

    public int getActiveMonth() {
        return activeMonth;
    }

    public void setActiveMonth(int activeMonth) {
        this.activeMonth = activeMonth;
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

    public int getActiveSecond() {
        return activeSecond;
    }

    public void setActiveSecond(int activeSecond) {
        this.activeSecond = activeSecond;
    }

    public String getActiveCron() {
        return activeCron;
    }

    public void setActiveCron(String activeCron) {
        this.activeCron = activeCron;
    }

    public int getInactiveYear() {
        return inactiveYear;
    }

    public void setInactiveYear(int inactiveYear) {
        this.inactiveYear = inactiveYear;
    }

    public int getInactiveMonth() {
        return inactiveMonth;
    }

    public void setInactiveMonth(int inactiveMonth) {
        this.inactiveMonth = inactiveMonth;
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

    public int getInactiveSecond() {
        return inactiveSecond;
    }

    public void setInactiveSecond(int inactiveSecond) {
        this.inactiveSecond = inactiveSecond;
    }

    public String getInactiveCron() {
        return inactiveCron;
    }

    public void setInactiveCron(String inactiveCron) {
        this.inactiveCron = inactiveCron;
    }

    public int getDataSourceTypeId() {
        return dataSourceTypeId;
    }

    public void setDataSourceTypeId(int dataSourceTypeId) {
        this.dataSourceTypeId = dataSourceTypeId;
    }

    public String getDataSourceName() {
        return dataSourceName;
    }

    public void setDataSourceName(String dataSourceName) {
        this.dataSourceName = dataSourceName;
    }

    public String getDataSourceXid() {
        return dataSourceXid;
    }

    public void setDataSourceXid(String dataSourceXid) {
        this.dataSourceXid = dataSourceXid;
    }

    public LocalizableMessage getDescription() {
        if (alias != null) {
            return new LocalizableMessageImpl("common.default", alias);
        }
        switch (scheduleType) {
            case MANUAL:
                return new LocalizableMessageImpl("maintenanceEvents.schedule.manual", dataSourceName);
            case ONCE:
                return new LocalizableMessageImpl("maintenanceEvents.schedule.onceUntil", dataSourceName, new DateTime(activeYear, activeMonth, activeDay, activeHour, activeMinute, activeSecond, 0).toDate(),
                        new DateTime(inactiveYear, inactiveMonth, inactiveDay, inactiveHour, inactiveMinute, inactiveSecond, 0).toDate());
            case HOURLY:
                String activeTime = StringUtils.pad(Integer.toString(activeMinute), '0', 2) + ":"
                        + StringUtils.pad(Integer.toString(activeSecond), '0', 2);
                return new LocalizableMessageImpl("maintenanceEvents.schedule.hoursUntil", dataSourceName, activeTime,
                        StringUtils.pad(Integer.toString(inactiveMinute), '0', 2) + ":"
                        + StringUtils.pad(Integer.toString(inactiveSecond), '0', 2));
            case DAILY:
                return new LocalizableMessageImpl("maintenanceEvents.schedule.dailyUntil", dataSourceName, activeTime(), inactiveTime());
            case WEEKLY:
                return new LocalizableMessageImpl("maintenanceEvents.schedule.weeklyUntil", dataSourceName, weekday(true), activeTime(), weekday(false), inactiveTime());
            case MONTHLY:
                return new LocalizableMessageImpl("maintenanceEvents.schedule.monthlyUntil", dataSourceName, monthday(true), activeTime(), monthday(false), inactiveTime());
            case YEARLY:
                return new LocalizableMessageImpl("maintenanceEvents.schedule.yearlyUntil", dataSourceName, monthday(true), month(true), activeTime(), monthday(false), month(false), inactiveTime());
            case CRON:
                return new LocalizableMessageImpl("maintenanceEvents.schedule.cronUntil", dataSourceName, activeCron, inactiveCron);
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
        return "event.audit.maintenanceEvent";
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "common.xid", xid);
        AuditEventType.addPropertyMessage(list, "maintenanceEvents.dataSource", dataSourceId);
        AuditEventType.addPropertyMessage(list, "maintenanceEvents.alias", alias);
        AuditEventType.addPropertyMessage(list, "common.alarmLevel", alarmLevel.getI18nKey());
        AuditEventType.addPropertyMessage(list, "maintenanceEvents.type", scheduleType);
        AuditEventType.addPropertyMessage(list, "common.disabled", disabled);
        AuditEventType.addPropertyMessage(list, "common.configuration", getDescription());
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, MaintenanceEventVO from) {
        AuditEventType.maybeAddPropertyChangeMessage(list, "common.xid", from.xid, xid);
        AuditEventType.maybeAddPropertyChangeMessage(list, "maintenanceEvents.dataSource", from.dataSourceId,
                dataSourceId);
        AuditEventType.maybeAddPropertyChangeMessage(list, "maintenanceEvents.alias", from.alias, alias);
        AuditEventType.maybeAddPropertyChangeMessage(list, "common.alarmLevel", from.alarmLevel, alarmLevel);
        if (from.scheduleType != scheduleType) {
            AuditEventType.addPropertyChangeMessage(list, "maintenanceEvents.type", from.scheduleType, scheduleType);
        }
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

}
