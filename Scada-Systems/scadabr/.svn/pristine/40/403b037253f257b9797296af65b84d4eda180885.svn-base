/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.timer.cron;

import java.util.Calendar;
import java.util.GregorianCalendar;

/**
 *
 * @author aploese
 */
public class CronCalendar {

    //needed to figure out thew leap year
    private final static GregorianCalendar gc = new GregorianCalendar();

    private int millisecond;
    private int second;
    private int minute;
    private int hourOfDay;
    private int dayOfMonth;
    private int month;
    private int year;

    public CronCalendar() {
    }

    public void setCurrentTime(Calendar c) {
        millisecond = c.get(Calendar.MILLISECOND);
        second = c.get(Calendar.SECOND);
        minute = c.get(Calendar.MINUTE);
        hourOfDay = c.get(Calendar.HOUR_OF_DAY);
        dayOfMonth = c.get(Calendar.DAY_OF_MONTH);
        month = c.get(Calendar.MONTH) + 1;
        year = c.get(Calendar.YEAR);
    }

    /**
     * @return the millisecond
     */
    public int getMillisecond() {
        return millisecond;
    }

    /**
     * @return the second
     */
    public int getSecond() {
        return second;
    }

    /**
     * @return the minute
     */
    public int getMinute() {
        return minute;
    }

    /**
     * @return the hourOfDay
     */
    public int getHourOfDay() {
        return hourOfDay;
    }

    /**
     * @return the dayOfMonth
     */
    public int getDayOfMonth() {
        return dayOfMonth;
    }

    /**
     * @return the month
     */
    public int getMonth() {
        return month;
    }

    /**
     * @return the year
     */
    public int getYear() {
        return year;
    }

    boolean setMilliseconds(int value) {
        if (millisecond == value) {
            return false;
        }
        if (value < millisecond) {
            incSecond();
        }
        millisecond = value;
        return true;
    }

    boolean setMilliseconds(boolean includeNow, int start, int end, int increment) {
        if (millisecond < start) {
            millisecond = start;
            return true;
        }

        if (millisecond > end) {
            millisecond = start;
            incSecond();
            return true;
        }

        final int modulo = (millisecond - start) % increment;
        if (modulo > 0) {
            millisecond += increment - modulo;
            if (millisecond > end) {
                millisecond = start;
                incSecond();
            }
            return true;
        } else if (!includeNow) {
            millisecond += increment;
            if (millisecond > end) {
                millisecond = start;
                incSecond();
            }
            return true;
        } else {
            return false;
        }

    }

    void incMillisecond() {
        millisecond++;
        if (millisecond > 999) {
            millisecond = 0;
            incSecond();
        }
    }

    boolean setSeconds(int value) {
        if (second == value) {
            return false;
        }
        if (value < second) {
            incMinute();
        }
        second = value;
        return true;
    }

    boolean setSeconds(boolean includeNow, int start, int end, int increment) {
        if (second < start) {
            second = start;
            return true;
        }

        if (second > end) {
            second = start;
            incMinute();
            return true;
        }

        int modulo = (second - start) % increment;
        if (modulo > 0) {
            second += increment - modulo;
            if (second > end) {
                second = start;
                incMinute();
            }
            return true;
        } else if (!includeNow) {
            second += increment;
            if (second > end) {
                second = start;
                incMinute();
            }
            return true;
        } else {
            return false;
        }
    }

    void incSecond() {
        second++;
        if (second > 59) {
            second = 0;
            incMinute();
        }
    }

    boolean setMinutes(int value) {
        if (minute == value) {
            return false;
        }
        if (value < minute) {
            incHourOfDay();
        }
        minute = value;
        return true;
    }

    boolean setMinutes(boolean includeNow, int start, int end, int increment) {
        if (minute < start) {
            minute = start;
            return true;
        }

        if (minute > end) {
            minute = start;
            incHourOfDay();
            return true;
        }

        int modulo = (minute - start) % increment;
        if (modulo > 0) {
            minute += increment - modulo;
            if (minute > end) {
                minute = start;
                incHourOfDay();
            }
            return true;
        } else if (!includeNow) {
            minute += increment;
            if (minute > end) {
                minute = start;
                incHourOfDay();
            }
            return true;
        } else {
            return false;
        }
    }

    void incMinute() {
        minute++;
        if (minute > 59) {
            minute = 0;
            incHourOfDay();
        }
    }

    boolean setHourOfDay(int value) {
        if (hourOfDay == value) {
            return false;
        }
        if (value < hourOfDay) {
            incDayOfMonth();
        }
        hourOfDay = value;
        return true;
    }

    boolean setHourOfDay(boolean includeNow, int start, int end, int increment) {
        if (hourOfDay < start) {
            hourOfDay = start;
            return true;
        }

        if (hourOfDay > end) {
            hourOfDay = start;
            incDayOfMonth();
            return true;
        }

        int modulo = (hourOfDay - start) % increment;
        if (modulo > 0) {
            hourOfDay += increment - modulo;
            if (hourOfDay > end) {
                hourOfDay = start;
                incDayOfMonth();
            }
            return true;
        } else if (!includeNow) {
            hourOfDay += increment;
            if (hourOfDay > end) {
                hourOfDay = start;
                incDayOfMonth();
            }
            return true;
        } else {
            return false;
        }
    }

    void incHourOfDay() {
        hourOfDay++;
        if (hourOfDay > 23) {
            hourOfDay = 0;
            incDayOfMonth();
        }
    }

    boolean setDayOfMonth(int value) {
        if (dayOfMonth == value) {
            return false;
        }
        if (value < dayOfMonth) {
            incMonth();
        }
        dayOfMonth = value;
        fixupMonth();
        return true;
    }

    private void fixupMonth() {
        switch (month) {
            case 2:
                if (dayOfMonth == 29) {
                    if (!isLeapYear()) {
                        incMonth();
                    }
                } else if (dayOfMonth > 29) {
                    incMonth();
                }
                break;
            case 4:
            case 6:
            case 9:
            case 11:
                if (dayOfMonth > 30) {
                    incMonth();
                }
                break;
            default:
        }
    }

    private void fixupMonthAndDay(int startDay) {
        switch (month) {
            case 2:
                if (dayOfMonth == 29) {
                    if (!isLeapYear()) {
                        incMonth();
                        dayOfMonth = startDay;
                    }
                } else if (dayOfMonth > 29) {
                    incMonth();
                    dayOfMonth = startDay;
                }
                break;
            case 4:
            case 6:
            case 9:
            case 11:
                if (dayOfMonth > 30) {
                    incMonth();
                    dayOfMonth = startDay;
                }
                break;
            default:
        }
    }

    boolean setDayOfMonth(boolean includeNow, int start, int end, int increment) {
        if (dayOfMonth < start) {
            dayOfMonth = start;
            return true;
        }

        if (dayOfMonth > end) {
            dayOfMonth = start;
            incMonth();
            fixupMonth();
            return true;
        }

        int modulo = (dayOfMonth - start) % increment;
        if (modulo > 0) {
            dayOfMonth += increment - modulo;
            if (dayOfMonth > end) {
                dayOfMonth = start;
                incMonth();
            }
            fixupMonthAndDay(start);
            return true;
        } else if (!includeNow) {
            dayOfMonth += increment;
            if (dayOfMonth > end) {
                dayOfMonth = start;
                incMonth();
            }
            fixupMonthAndDay(start);
            return true;
        } else {
            return false;
        }
    }

    void incDayOfMonth() {
        dayOfMonth++;
        fixupMonth();
    }

    boolean setMonth(int value) throws NoLeapYearException, AfterLastDayOfMonthException {
        if (month == value) {
            return false;
        }
        if (value < month) {
            incYear();
        }
        month = value;
        switch (month) {
            case 2:
                if (dayOfMonth == 29) {
                    if (isLeapYear()) {
                        return true;
                    } else {
                        throw new NoLeapYearException();
                    }

                } else if (dayOfMonth > 29) {
                    throw new AfterLastDayOfMonthException();
                }
                break;
            case 4:
            case 6:
            case 9:
            case 11:
                if (dayOfMonth == 31) {
                    throw new AfterLastDayOfMonthException();
                }
                break;
        }
        return true;
    }

    boolean setMonth(boolean includeNow, int start, int end, int increment) {
        if (month < start) {
            month = start;
            return true;
        }

        if (month > end) {
            month = start;
            incYear();
            return true;
        }

        int modulo = (month - start) % increment;
        if (modulo > 0) {
            month += increment - modulo;
            if (month > end) {
                month = start;
                incYear();
            }
            return true;
        } else if (!includeNow) {
            month += increment;
            if (month > end) {
                month = start;
                incYear();
            }
            return true;
        } else {
            return false;
        }
    }

    void incMonth() {
        month++;
        if (month > 12) {
            month = 1;
            incYear();
        }
        fixupMonth();
    }

    boolean setYear(int value) throws NoLeapYearException, TimeInPastException {
        if (year == value) {
            return false;
        }
        if (value < year) {
            throw new TimeInPastException();
        }
        year = value;
        if (month == 2 && dayOfMonth == 29) {
            if (isLeapYear()) {
                return true;
            } else {
                throw new NoLeapYearException();
            }
        }
        return true;
    }

    //TODO fix LeapYear
    boolean setYear(boolean includeNow, int start, int end, int increment) throws NoLeapYearException, TimeInPastException {
        if (year < start) {
            year = start;
            return true;
        }

        if (year > end) {
            year = start;
            throw new TimeInPastException();
        }

        int modulo = (year - start) % increment;
        if (modulo > 0) {
            year += increment - modulo;
            if (year > end) {
                throw new TimeInPastException();
            }
            return true;
        } else if (!includeNow) {
            year += increment;
            if (year > end) {
                throw new TimeInPastException();
            }
            if (dayOfMonth == 29 && month == 2 && !isLeapYear()) {
                throw new NoLeapYearException();
            }
            return true;
        } else {
            return false;
        }
    }

    void incYear() {
        year++;
    }

    boolean isLeapYear() {
        return gc.isLeapYear(year);
    }

    /**
     * Set the current time and date to the calendar.
     * @param c 
     */
    void setCalendarToTimestamp(Calendar c) {
        c.set(year, month-1, dayOfMonth, hourOfDay, minute, second);
        c.set(Calendar.MILLISECOND, millisecond);
    }

    static class NoLeapYearException extends Exception {
    }

    static class AfterLastDayOfMonthException extends Exception {
    }

    static class TimeInPastException extends Exception {
    }

}
