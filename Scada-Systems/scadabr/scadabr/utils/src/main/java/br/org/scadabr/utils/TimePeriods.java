/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.utils;

import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import org.joda.time.DateTime;
import org.joda.time.Period;

/**
 *
 * @author aploese
 */
public enum TimePeriods implements LocalizableMessage {

    MILLISECONDS(8, "common.tp.milliseconds"),
    SECONDS(1, "common.tp.seconds"),
    MINUTES(2, "common.tp.minutes"),
    HOURS(3, "common.tp.hours"),
    DAYS(4, "common.tp.days"),
    WEEKS(5, "common.tp.weeks"),
    MONTHS(6, "common.tp.months"),
    YEARS(7, "common.tp.years");
    private final String i18nKey;
    private final int id;

    private TimePeriods(int id, String i18nKey) {
        this.id = id;
        this.i18nKey = i18nKey;
    }

    public static TimePeriods fromId(int id) {
        switch (id) {
            case 8:
                return TimePeriods.MILLISECONDS;
            case 1:
                return TimePeriods.SECONDS;
            case 2:
                return TimePeriods.MINUTES;
            case 3:
                return TimePeriods.HOURS;
            case 4:
                return TimePeriods.DAYS;
            case 5:
                return TimePeriods.WEEKS;
            case 6:
                return TimePeriods.MONTHS;
            case 7:
                return TimePeriods.YEARS;
            default:
                throw new RuntimeException("Unknown TimePeriod: " + id);
        }
    }

    public int getId() {
        return id;
    }

    /**
     * Returns the length of time in milliseconds that the
     *
     * @param periods
     * @return
     */
    public long getMillis(int periods) {
        return getPeriod(periods).toDurationFrom(null).getMillis();
    }

    public Period getPeriod(int periods) {
        switch (this) {
            case MILLISECONDS:
                return Period.millis(periods);
            case SECONDS:
                return Period.seconds(periods);
            case MINUTES:
                return Period.minutes(periods);
            case HOURS:
                return Period.hours(periods);
            case DAYS:
                return Period.days(periods);
            case WEEKS:
                return Period.weeks(periods);
            case MONTHS:
                return Period.months(periods);
            case YEARS:
                return Period.years(periods);
            default:
                throw new RuntimeException("Unsupported time period: "
                        + this);
        }
    }

    public LocalizableMessage getPeriodDescription(int periods) {
        return new LocalizableMessageImpl("common.tp.description", periods, this);
    }

    @Override
    public String getI18nKey() {
        return i18nKey;
    }

    @Override
    public Object[] getArgs() {
        return null;
    }

    public long minus(long time, int periods) {
        return minus(new DateTime(time), periods).getMillis();
    }

    public DateTime minus(DateTime time, int periods) {
        return time.minus(getPeriod(periods));
    }

    public long truncate(long time) {
        return truncateDateTime(new DateTime(time)).getMillis();
    }

    public DateTime truncateDateTime(DateTime time) {
        switch (this) {
            case SECONDS:
                time = time.minus(time.getMillisOfSecond());
                break;
            case MINUTES:
                time = time.minus(time.getMillisOfSecond());
                time = time.minus(TimePeriods.SECONDS.getPeriod(time.getSecondOfMinute()));
                break;
            case HOURS:
                time = time.minus(time.getMillisOfSecond());
                time = time.minus(TimePeriods.SECONDS.getPeriod(time.getSecondOfMinute()));
                time = time.minus(TimePeriods.MINUTES.getPeriod(time.getMinuteOfHour()));
                break;
            case DAYS:
                time = time.minus(time.getMillisOfDay());
                break;
            case WEEKS:
                time = time.minus(time.getMillisOfDay());
                time = time.minus(TimePeriods.DAYS.getPeriod(time.getDayOfWeek() - 1));
                break;
            case MONTHS:
                time = time.minus(time.getMillisOfDay());
                time = time.minus(TimePeriods.DAYS.getPeriod(time.getDayOfMonth() - 1));
                break;
            case YEARS:
                time = time.minus(time.getMillisOfDay());
                time = time.minus(TimePeriods.DAYS.getPeriod(time.getDayOfYear() - 1));
                break;
            default:
                throw new RuntimeException("TimePeriod unknown" + this);
        }
        return time;
    }

    public long next(long time) {
        return minus(truncateDateTime(new DateTime(time)), -1).getMillis();
    }

    public static LocalizableMessage getDuration(long duration) {
        if (duration < 1000) {
            return new LocalizableMessageImpl("common.duration.millis", duration);
        }

        if (duration < 10000) {
            String s = "" + (duration / 1000) + '.';
            s += (int) (((double) (duration % 1000)) / 10 + 0.5);
            return new LocalizableMessageImpl("common.duration.seconds", s);
        }

        if (duration < 60000) {
            String s = "" + (duration / 1000) + '.';
            s += (int) (((double) (duration % 1000)) / 100 + 0.5);
            return new LocalizableMessageImpl("common.duration.seconds", s);
        }

        // Convert to seconds
        duration /= 1000;

        if (duration < 600) {
            return new LocalizableMessageImpl("common.duration.minSec", duration / 60, duration % 60);
        }

        // Convert to minutes
        duration /= 60;

        if (duration < 60) {
            return new LocalizableMessageImpl("common.duration.minutes", duration);
        }

        if (duration < 1440) {
            return new LocalizableMessageImpl("common.duration.hourMin", duration / 60, duration % 60);
        }

        // Convert to hours
        duration /= 60;

        return new LocalizableMessageImpl("common.duration.hours", duration);
    }

}
