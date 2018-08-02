/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.timer.cron;

import java.text.DateFormat;
import java.text.FieldPosition;
import java.text.ParsePosition;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.Locale;
import java.util.TimeZone;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;
import org.junit.Ignore;

/**
 *
 * @author aploese
 */
public class CronExpressionTest {

    private static TimeZone tz;
    private CronParser cp;
    private static DateFormat df;

    public CronExpressionTest() {
    }

    @BeforeClass
    public static void setUpClass() {
        tz = TimeZone.getTimeZone("UTC");
        df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS");
        df.setTimeZone(tz);
    }

    @AfterClass
    public static void tearDownClass() {
    }

    @Before
    public void setUp() {
        cp = new CronParser();
    }

    @After
    public void tearDown() {
        cp = null;
    }

    private Calendar getCalendar() {
        Calendar result = Calendar.getInstance(tz);
        result.set(2014, Calendar.JANUARY, 1, 0, 0, 0);
        result.set(Calendar.MILLISECOND, 0);
        return result;
    }

    private String formatDate(Calendar c) {
        return df.format(c.getTime());
    }

    /**
     * Test of calcNextValidTime method, of class CronExpression.
     */
    @Test
    public void testCalcNextValidTime_ANY() throws Exception {
        System.out.println("calcNextValidTime");
        CronExpression instance = cp.parse("* * * * * * * *", CronExpression.TIMEZONE_UTC);
        Calendar c = getCalendar();

        instance.calcNextValidTimeIncludingThis(c.getTimeInMillis());
        assertEquals("2014-01-01 00:00:00.000 UTC", instance.getNextTimestampAsString());
        instance.calcNextValidTimeAfter();
        assertEquals("2014-01-01 00:00:00.001 UTC", instance.getNextTimestampAsString());
        instance.calcNextValidTimeAfter();
        assertEquals("2014-01-01 00:00:00.002 UTC", instance.getNextTimestampAsString());

        c.set(Calendar.MILLISECOND, 999);
        instance.calcNextValidTimeAfter(c.getTimeInMillis());
        assertEquals("2014-01-01 00:00:01.000 UTC", instance.getNextTimestampAsString());
    }

    @Test
    public void testCalcNextValidTime_Sec() throws Exception {
        System.out.println("calcNextValidTime");
        CronExpression instance = cp.parse("0 5-25/7 * * * * * *", CronExpression.TIMEZONE_UTC);
        Calendar c = getCalendar();

        instance.calcNextValidTimeIncludingThis(c.getTimeInMillis());
        assertEquals("2014-01-01 00:00:05.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeIncludingThis(instance.getNextTimestampinMillis());
        assertEquals("2014-01-01 00:00:05.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-01-01 00:00:12.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-01-01 00:00:19.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-01-01 00:01:05.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-01-01 00:01:12.000 UTC", instance.getNextTimestampAsString());
    }

    @Test
    public void testCalc_30() throws Exception {
        System.out.println("calcNextValidTime");
        CronExpression instance = cp.parse("0 0 0 12 30 * * *", CronExpression.TIMEZONE_UTC);
        Calendar c = getCalendar();

        instance.calcNextValidTimeIncludingThis(c.getTimeInMillis());
        assertEquals("2014-01-30 12:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeIncludingThis(instance.getNextTimestampinMillis());
        assertEquals("2014-01-30 12:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-03-30 12:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-04-30 12:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-05-30 12:00:00.000 UTC", instance.getNextTimestampAsString());
    }

    @Test
    public void testCalc_31() throws Exception {
        System.out.println("calcNextValidTime");
        CronExpression instance = cp.parse("0 0 0 15 31 * * *", CronExpression.TIMEZONE_UTC);
        Calendar c = getCalendar();

        instance.calcNextValidTimeIncludingThis(c.getTimeInMillis());
        assertEquals("2014-01-31 15:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeIncludingThis(instance.getNextTimestampinMillis());
        assertEquals("2014-01-31 15:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-03-31 15:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-05-31 15:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-07-31 15:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-08-31 15:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-10-31 15:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-12-31 15:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2015-01-31 15:00:00.000 UTC", instance.getNextTimestampAsString());
    }

    @Test
    public void testCalc_29_FEB() throws Exception {
        System.out.println("calcNextValidTime");
        CronExpression instance = cp.parse("0 0 0 12 29 FEB * *", CronExpression.TIMEZONE_UTC);
        Calendar c = getCalendar();

        instance.calcNextValidTimeIncludingThis(c.getTimeInMillis());
        assertEquals("2016-02-29 12:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeIncludingThis(instance.getNextTimestampinMillis());
        assertEquals("2016-02-29 12:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2020-02-29 12:00:00.000 UTC", instance.getNextTimestampAsString());
    }

    @Test
    public void testCalc_Range_28_TO_31() throws Exception {
        System.out.println("calcNextValidTime");
        CronExpression instance = cp.parse("0 0 0 21 28-31 * * *", CronExpression.TIMEZONE_UTC);
        Calendar c = getCalendar();

        instance.calcNextValidTimeIncludingThis(c.getTimeInMillis());
        assertEquals("2014-01-28 21:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeIncludingThis(instance.getNextTimestampinMillis());
        assertEquals("2014-01-28 21:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-01-29 21:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-01-30 21:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-01-31 21:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-02-28 21:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-03-28 21:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-03-29 21:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-03-30 21:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-03-31 21:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-04-28 21:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-04-29 21:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-04-30 21:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-05-28 21:00:00.000 UTC", instance.getNextTimestampAsString());
    }
    
    @Test
    @Ignore("Currently No ranges in day, month and year")
    public void testCalc_Range_28_29_30_31() throws Exception {
        CronExpression instance = cp.parse("0 0 0 21 28,29,30,31 * * *", CronExpression.TIMEZONE_UTC);
        Calendar c = getCalendar();

        instance.calcNextValidTimeIncludingThis(c.getTimeInMillis());
        assertEquals("2016-29-02 12:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeIncludingThis(instance.getNextTimestampinMillis());
        assertEquals("2016-29-02 12:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2020-29-02 12:00:00.000 UTC", instance.getNextTimestampAsString());

        fail();
    }

    @Test
    public void testCalc_Range_28_TO_29_FEB() throws Exception {
        System.out.println("calcNextValidTime");
        CronExpression instance = cp.parse("0 0 0 21 28-29 FEB * *", CronExpression.TIMEZONE_UTC);
        Calendar c = getCalendar();

        instance.calcNextValidTimeIncludingThis(c.getTimeInMillis());
        assertEquals("2014-02-28 21:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeIncludingThis(instance.getNextTimestampinMillis());
        assertEquals("2014-02-28 21:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2015-02-28 21:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2016-02-28 21:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2016-02-29 21:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2017-02-28 21:00:00.000 UTC", instance.getNextTimestampAsString());
    }
    
    @Test
    @Ignore("No ranges in day, month and year")
    public void testCalc_Range_28_29_FEB() throws Exception {
        CronExpression instance = cp.parse("0 0 0 21 28,29 FEB * *", CronExpression.TIMEZONE_UTC);
        Calendar c = getCalendar();

        instance.calcNextValidTimeIncludingThis(c.getTimeInMillis());
        assertEquals("2016-29-02 12:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeIncludingThis(instance.getNextTimestampinMillis());
        assertEquals("2016-29-02 12:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2020-29-02 12:00:00.000 UTC", instance.getNextTimestampAsString());

        fail();
    }


    @Test
    @Ignore
    public void testCalc_Last_Day_Of_Month() throws Exception {
        System.out.println("calcNextValidTime");
        CronExpression instance = cp.parse("0 0 0 12 L * * *", CronExpression.TIMEZONE_UTC);
        Calendar c = getCalendar();

        instance.calcNextValidTimeIncludingThis(c.getTimeInMillis());
        assertEquals("2014-01-01 00:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeIncludingThis(instance.getNextTimestampinMillis());
        assertEquals("2014-01-01 00:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-01-01 00:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-01-01 00:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-01-01 00:00:00.000 UTC", instance.getNextTimestampAsString());

        fail();
    }

    @Test
    @Ignore
    public void testCalc_WeekDay() throws Exception {
        System.out.println("calcNextValidTime");
        CronExpression instance = cp.parse("0 0 0 12 * * SUN *", CronExpression.TIMEZONE_UTC);
        Calendar c = getCalendar();

        instance.calcNextValidTimeIncludingThis(c.getTimeInMillis());
        assertEquals("2014-01-01 00:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeIncludingThis(instance.getNextTimestampinMillis());
        assertEquals("2014-01-01 00:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-01-01 00:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-01-01 00:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-01-01 00:00:00.000 UTC", instance.getNextTimestampAsString());

        fail();
    }

    @Test
    @Ignore
    public void testCalc_WorkDay() throws Exception {
        //cron # ???
        System.out.println("calcNextValidTime");
        CronExpression instance = cp.parse("0 0 0 12 29W * * *", CronExpression.TIMEZONE_UTC);
        Calendar c = getCalendar();

        instance.calcNextValidTimeIncludingThis(c.getTimeInMillis());
        assertEquals("2014-01-01 00:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeIncludingThis(instance.getNextTimestampinMillis());
        assertEquals("2014-01-01 00:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-01-01 00:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-01-01 00:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-01-01 00:00:00.000 UTC", instance.getNextTimestampAsString());

        fail();
    }

    @Test
    public void testCalc_Ranges() throws Exception {
        System.out.println("calcNextValidTime");
        CronExpression instance = cp.parse("0 0 0-29/3,30-59/5 * * * * *", CronExpression.TIMEZONE_UTC);
        Calendar c = getCalendar();

        instance.calcNextValidTimeIncludingThis(c.getTimeInMillis());
        assertEquals("2014-01-01 00:00:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-01-01 00:03:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-01-01 00:06:00.000 UTC", instance.getNextTimestampAsString());

        c.set(Calendar.MINUTE, 27);
        instance.calcNextValidTimeAfter(c.getTimeInMillis());
        assertEquals("2014-01-01 00:30:00.000 UTC", instance.getNextTimestampAsString());

        instance.calcNextValidTimeAfter();
        assertEquals("2014-01-01 00:35:00.000 UTC", instance.getNextTimestampAsString());
    }

}
