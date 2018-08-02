/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo.event.type;

import br.org.scadabr.rt.event.type.DuplicateHandling;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.vo.event.AlarmLevel;

/**
 *
 * @author aploese
 */
public enum MaintenanceEventKey implements EventKey<MaintenanceEventKey> {

    MANUAL(1, "maintenanceEvents.type.manual"),
    HOURLY(2, "maintenanceEvents.type.hour"),
    DAILY(3, "maintenanceEvents.type.day"),
    WEEKLY(4, "maintenanceEvents.type.week"),
    MONTHLY(5, "maintenanceEvents.type.month"),
    YEARLY(6, "maintenanceEvents.type.year"),
    ONCE(7, "maintenanceEvents.type.once"),
    CRON(8, "maintenanceEvents.type.cron");

    public static MaintenanceEventKey fromId(int id) {
        switch (id) {
            case 1:
                return MANUAL;
            case 2:
                return HOURLY;
            case 3:
                return DAILY;
            case 4:
                return WEEKLY;
            case 5:
                return MONTHLY;
            case 6:
                return YEARLY;
            case 7:
                return ONCE;
            case 8:
                return CRON;
            default:
                throw new IndexOutOfBoundsException("Cant get MaintenanceEventKey from id: " + id);
        }
    }

    private final int id;
    private final String i18nKey;

    private MaintenanceEventKey(int id, String i18nKey) {
        this.id = id;
        this.i18nKey = i18nKey;
    }

    @Override
    public int getId() {
        return id;
    }

    @Override
    public DuplicateHandling getDuplicateHandling() {
        return DuplicateHandling.IGNORE;
    }

    @Override
    public AlarmLevel getDefaultAlarmLevel() {
        throw new ImplementMeException();
    }

    @Override
    public boolean isStateful() {
        throw new ImplementMeException();
    }

    @Override
    public String getI18nKey() {
        return i18nKey;
    }

    @Override
    public Object[] getArgs() {
        return null;
    }

    @Override
    public String getName() {
        return name();
    }

}
