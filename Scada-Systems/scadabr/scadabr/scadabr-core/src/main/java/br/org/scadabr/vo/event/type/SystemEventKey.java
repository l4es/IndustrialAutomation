/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo.event.type;

import br.org.scadabr.rt.event.type.DuplicateHandling;
import br.org.scadabr.vo.event.AlarmLevel;

/**
 *
 * @author aploese
 */
public enum SystemEventKey implements EventKey<SystemEventKey> {

    SYSTEM_STARTUP(1, "event.system.startup", AlarmLevel.INFORMATION, false),
    SYSTEM_SHUTDOWN(2, "event.system.shutdown", AlarmLevel.INFORMATION, false),
    MAX_ALARM_LEVEL_CHANGED(3, "event.system.maxAlarmChanged", AlarmLevel.NONE, true),
    USER_LOGIN(4, "event.system.userLogin", AlarmLevel.INFORMATION, true),
    VERSION_CHECK(5, "event.system.versionCheck", AlarmLevel.INFORMATION, false) {
                @Override
                public DuplicateHandling getDuplicateHandling() {
                    return DuplicateHandling.IGNORE_SAME_MESSAGE;
                }
            },
    COMPOUND_DETECTOR_FAILURE(6, "event.system.compound", AlarmLevel.URGENT, true),
    SET_POINT_HANDLER_FAILURE(7, "event.system.setPoint", AlarmLevel.URGENT, true),
    EMAIL_SEND_FAILURE(8, "event.system.email", AlarmLevel.INFORMATION, true),
    POINT_LINK_FAILURE(9, "event.system.pointLink", AlarmLevel.URGENT, true) {
                @Override
                public DuplicateHandling getDuplicateHandling() {
                    return DuplicateHandling.IGNORE_SAME_MESSAGE;
                }
            },
    PROCESS_FAILURE(10, "event.system.process", AlarmLevel.URGENT, true);
    private final String i18nKey;
    private final int id;
    private final AlarmLevel defaultAlarmLevel;
    private AlarmLevel alarmLevel;
    private final boolean stateful;

    private SystemEventKey(int id, String i18nKey, AlarmLevel defaultAlarmLevel, boolean stateful) {
        this.i18nKey = i18nKey;
        this.id = id;
        this.defaultAlarmLevel = defaultAlarmLevel;
        this.alarmLevel = defaultAlarmLevel;
        this.stateful = stateful;
    }

    @Override
    public String getName() {
        return name();
    }

    @Override
    public String getI18nKey() {
        return i18nKey;
    }

    public static SystemEventKey fromId(int ordinal) {
        switch (ordinal) {
            case 1:
                return SYSTEM_STARTUP;
            case 2:
                return SYSTEM_SHUTDOWN;
            case 3:
                return MAX_ALARM_LEVEL_CHANGED;
            case 4:
                return USER_LOGIN;
            case 5:
                return VERSION_CHECK;
            case 6:
                return COMPOUND_DETECTOR_FAILURE;
            case 7:
                return SET_POINT_HANDLER_FAILURE;
            case 8:
                return EMAIL_SEND_FAILURE;
            case 9:
                return POINT_LINK_FAILURE;
            case 10:
                return PROCESS_FAILURE;
            default:
                throw new IndexOutOfBoundsException();
        }
    }

    @Override
    public Object[] getArgs() {
        return null;
    }

    /**
     * @return the id
     */
    @Override
    public int getId() {
        return id;
    }

    /**
     * @return the defaultAlarmLevel
     */
    public AlarmLevel getDefaultAlarmLevel() {
        return defaultAlarmLevel;
    }

    /**
     * @return the alarmLevel
     */
    public AlarmLevel getAlarmLevel() {
        return alarmLevel;
    }

    /**
     * @param alarmLevel the alarmLevel to set
     */
    public void setAlarmLevel(AlarmLevel alarmLevel) {
        this.alarmLevel = alarmLevel;
    }

    public boolean isDefaultAlarmlevel() {
        return defaultAlarmLevel == alarmLevel;
    }

    @Override
    public boolean isStateful() {
        return stateful;
    }

    @Override
    public DuplicateHandling getDuplicateHandling() {
        return DuplicateHandling.ALLOW;
    }
}
