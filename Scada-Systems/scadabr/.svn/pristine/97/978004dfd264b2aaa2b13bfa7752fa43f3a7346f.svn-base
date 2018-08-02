/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package br.org.scadabr.timer.cron;

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
public class CronParserTest {
    
    private CronParser parser;
    
    public CronParserTest() {
    }
    
    @BeforeClass
    public static void setUpClass() {
    }
    
    @AfterClass
    public static void tearDownClass() {
    }
    
    @Before
    public void setUp() {
        parser = new CronParser();
    }
    
    @After
    public void tearDown() {
        parser = null;
    }

    /**
     * Test of parse method, of class CronParser.
     */
    @Test
    public void testParseANY() throws Exception {
        System.out.println("parse");
        String cron = "* * * * * * * *";
        CronExpression result = parser.parse(cron, CronExpression.TIMEZONE_UTC);
        assertEquals(cron, result.toString());
    }
    
    /**
     * Test of parse method, of class CronParser.
     */
    @Test
    public void testParse_Fixed() throws Exception {
        System.out.println("parse");
        String cron = "0 * * * * * * *";
        CronExpression result = parser.parse(cron, CronExpression.TIMEZONE_UTC);
        assertEquals(cron, result.toString());
    }

    /**
     * Test of parse method, of class CronParser.
     */
    @Test(expected = RuntimeException.class)
    public void testParse_Too_Long() throws Exception {
        System.out.println("parse");
        String cron = "* * * * * * * * *";
        CronExpression result = parser.parse(cron, CronExpression.TIMEZONE_UTC);
    }


    /**
     * Test of parse method, of class CronParser.
     */
    @Test(expected = RuntimeException.class)
    public void testParse_Too_Short() throws Exception {
        System.out.println("parse");
        String cron = "* * * * * *";
        CronExpression result = parser.parse(cron, CronExpression.TIMEZONE_UTC);
    }




    /**
     * Test of parse method, of class CronParser.
     */
    @Test
    public void testParse_Fixed_TWO() throws Exception {
        System.out.println("parse");
        String cron = "0,500 * * * * * * *";
        CronExpression result = parser.parse(cron, CronExpression.TIMEZONE_UTC);
        assertEquals(cron, result.toString());
    }


    /**
     * Test of parse method, of class CronParser.
     */
    @Test
    public void testParse_MS_Range() throws Exception {
        System.out.println("parse");
        String cron = "100-200 * * * * * * *";
        CronExpression result = parser.parse(cron, CronExpression.TIMEZONE_UTC);
        assertEquals(cron, result.toString());
    }

    /**
     * Test of parse method, of class CronParser.
     */
    @Test
    public void testParse_MS_Range_Increment() throws Exception {
        System.out.println("parse");
        String cron = "100-600/17 * * * * * * *";
        CronExpression result = parser.parse(cron, CronExpression.TIMEZONE_UTC);
        assertEquals(cron, result.toString());
    }

    /**
     * Test of parse method, of class CronParser.
     */
    @Test
    public void testParse_MS_ANY_Increment() throws Exception {
        System.out.println("parse");
        String cron = "*/200 * * * * * * *";
        CronExpression result = parser.parse(cron, CronExpression.TIMEZONE_UTC);
        assertEquals(cron, result.toString());
    }

    /**
     * Test of parse method, of class CronParser.
     */
    @Test
    public void testParse_MS_Range_Increment_Range() throws Exception {
        System.out.println("parse");
        String cron = "0-499/100,500-999/50 * * * * * * *";
        CronExpression result = parser.parse(cron, CronExpression.TIMEZONE_UTC);
        assertEquals(cron, result.toString());
    }

}
