/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package br.org.scadabr.timer.cron;

import java.util.Calendar;

/**
 *
 * @author aploese
 */
public enum CronFieldType {
    MILLISECOND(Calendar.MILLISECOND, 0, 999), 
    SECOND(Calendar.SECOND, 0, 59), 
    MINUTE(Calendar.MINUTE, 0, 59), 
    HOUR_OF_DAY(Calendar.HOUR_OF_DAY, 0, 23), 
    DAY_OF_MONTH(Calendar.DAY_OF_MONTH, 1, 31), 
    MONTH(Calendar.MONTH, 1, 12), // Calendar fields!! january == 1 !!!
    DAY_OF_WEEK(Calendar.DAY_OF_WEEK, 0, 6), // Calendar fields!! start is sunday == 0 !!!
    YEAR(Calendar.YEAR, 1970, 2099);
    
    public final int floor;
    public final int ceil;
    public final int calendarField;
    
    private CronFieldType(int calendarField, int floor, int ceil) {
        this.calendarField = calendarField;
        this.floor = floor;
        this.ceil = ceil;
    }
    
    public boolean isValid(int value) {
        return value >= floor ? value <= ceil : false;
    }

}
