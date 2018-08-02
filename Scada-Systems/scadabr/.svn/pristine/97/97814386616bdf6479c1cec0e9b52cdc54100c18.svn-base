/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

import java.util.Date;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.Callable;
import java.util.concurrent.Future;
import java.util.concurrent.ScheduledThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.logging.Logger;
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
public class SchedulerTest {
    
    final static Logger LOG = Logger.getLogger("SCHEDULER TEST");
    
    class MyFuture extends TimerTask {

        @Override
        public void run() {
            LOG.info("Hello: " + super.toString()  + new Date(scheduledExecutionTime()));
        }
        
    }
    
    Timer t;
    
    public SchedulerTest() {
    }
    
    @BeforeClass
    public static void setUpClass() {
    }
    
    @AfterClass
    public static void tearDownClass() {
    }
    
    @Before
    public void setUp() {
        t = new Timer();
    }
    
    @After
    public void tearDown() {
        t.cancel();
        t = null;
    }

     @Test
     @Ignore // takes 10 sec ....
     public void hello() throws Exception {
         Date d = new Date();
         d .setHours(d.getHours() -1);
         t.scheduleAtFixedRate(new MyFuture(), d, 1000);
         Thread.sleep(10000);
     }
}
