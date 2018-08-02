/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.timer;

import br.org.scadabr.timer.cron.CronExpression;
import java.text.ParseException;
import java.util.TimeZone;
import java.util.concurrent.TimeUnit;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author aploese
 */
public class CronTimerPoolTest {

    class TestCronTask extends CronTask {

        public TestCronTask(String cronPattern, TimeZone tz) throws ParseException {
            super(cronPattern, tz);
        }

        int i = 0;

        @Override
        protected void run(long scheduledExecutionTime) {
            i++;
        }

        @Override
        protected boolean overrunDetected(long lastExecutionTime, long thisExecutionTime) {
            throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
        }

    }

    public CronTimerPoolTest() {
    }

    @BeforeClass
    public static void setUpClass() {
    }

    @AfterClass
    public static void tearDownClass() {
    }

    @Before
    public void setUp() {
    }

    @After
    public void tearDown() {
    }

    /**
     * Test of schedule method, of class CronTimerPool.
     */
    @Test
    public void testSchedule() throws Exception {
        System.out.println("schedule");
        TestCronTask task = new TestCronTask("0 * * * * * * *", CronExpression.TIMEZONE_UTC);
        CronTimerPool instance = new CronTimerPool(1, 10, 10, TimeUnit.SECONDS);
        instance.schedule(task);
        Thread.sleep(5000);
        instance.shutdown();
        assertTrue("times called:  " + task.i, task.i <= 6);
        assertTrue("times called:  " + task.i, task.i >= 4);
    }

}
