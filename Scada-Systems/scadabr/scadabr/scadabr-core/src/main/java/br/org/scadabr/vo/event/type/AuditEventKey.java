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
public enum AuditEventKey implements EventKey<AuditEventKey>{

    DATA_SOURCE(1, "event.audit.dataSource"),
    DATA_POINT(2, "event.audit.dataPoint"),
    POINT_EVENT_DETECTOR(3, "event.audit.pointEventDetector"),
    COMPOUND_EVENT_DETECTOR(4, "event.audit.compoundEventDetector"),
    SCHEDULED_EVENT(5, "event.audit.scheduledEvent"),
    EVENT_HANDLER(6, "event.audit.eventHandler"),
    POINT_LINK(7, "event.audit.pointLink"),
    MAINTENANCE_EVENT(8, "event.audit.maintenanceEvent");
    private final String i18nKey;
    private final int id;
    private AlarmLevel alarmLevel;

    private AuditEventKey(int id, String i18nKey) {
        this.i18nKey = i18nKey;
        this.id = id;
        this.alarmLevel = getDefaultAlarmLevel();
    }

    @Override
    public String getName() {
        return name();
    }
    
    @Override
    public String getI18nKey() {
        return i18nKey;
    }
    
    @Override
    public int getId() {
        return id;
    }

    public static AuditEventKey fromId(int id) {
        switch (id) {
            case 1:
                return DATA_SOURCE;
            case 2:
                return DATA_POINT;
            case 3:
                return POINT_EVENT_DETECTOR;
            case 4:
                return COMPOUND_EVENT_DETECTOR;
            case 5:
                return SCHEDULED_EVENT;
            case 6:
                return EVENT_HANDLER;
            case 7:
                return POINT_LINK;
            case 8:
                return MAINTENANCE_EVENT;
            default:
                throw new IndexOutOfBoundsException();
        }
    }

    @Override
    public Object[] getArgs() {
        return null;
    }
    
    /**
     * @return the defaultAlarmLevel
     */
    @Override
    public AlarmLevel getDefaultAlarmLevel() {
        return AlarmLevel.INFORMATION;
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
        return getDefaultAlarmLevel() == alarmLevel;
    }

    @Override
    public DuplicateHandling getDuplicateHandling() {
        return DuplicateHandling.IGNORE;
    }

    @Override
    public boolean isStateful() {
        return false;
    }


}
