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
public enum ScheduledEventKey implements EventKey<ScheduledEventKey> {

    HOURLY(1, "scheduledEvents.type.hour"),
    DAILY(2, "scheduledEvents.type.day"),
    WEEKLY(3, "scheduledEvents.type.week"),
    MONTHLY(4, "scheduledEvents.type.month"),
    YEARLY(5, "scheduledEvents.type.year"),
    ONCE(6, "scheduledEvents.type.once"),
    CRON(7, "scheduledEvents.type.cron");

    public static ScheduledEventKey fromId(int id) {
        switch (id) {
            case 1:
                return HOURLY;
            case 2:
                return DAILY;
            case 3:
                return WEEKLY;
            case 4:
                return YEARLY;
            case 5:
                return ONCE;
            case 6:
                return CRON;
            default:
                throw new IndexOutOfBoundsException("Cant get ScheduledEventKey from id: " + id);
        }
    }

    private final int id;
    private final String i18nKey;

    private ScheduledEventKey(int id, String i18nKey) {
        this.id = id;
        this.i18nKey = i18nKey;
    }

    @Override
    public int getId() {
        return id;
    }

    @Override
    public DuplicateHandling getDuplicateHandling() {
        throw new ImplementMeException();
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
