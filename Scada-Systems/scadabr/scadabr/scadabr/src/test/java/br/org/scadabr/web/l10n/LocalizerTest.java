/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package br.org.scadabr.web.l10n;

import br.org.scadabr.l10n.AbstractLocalizer;
import br.org.scadabr.l10n.Localizer;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import java.text.MessageFormat;
import java.util.Locale;
import java.util.ResourceBundle;
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
public class LocalizerTest {
    
    public LocalizerTest() {
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
     * Test of localizeI18nKey method, of class Localizer.
     */
    @Test
    @Ignore
    public void testLocalizeI18nKey() {
        System.out.println("localizeI18nKey");
        String i18nKey = "";
        ResourceBundle bundle = null;
        Object[] args = null;
        String expResult = "";
        String result = AbstractLocalizer.localizeI18nKey(i18nKey, bundle, args);
        assertEquals(expResult, result);
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }

    /**
     * Test of localizeMessage method, of class Localizer.
     */
    @Test
    @Ignore
    public void testLocalizeMessage() {
        System.out.println("localizeMessage");
        LocalizableMessage i18nMessage = null;
        ResourceBundle bundle = null;
        String expResult = "";
        String result = AbstractLocalizer.localizeMessage(i18nMessage, bundle);
        assertEquals(expResult, result);
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }

    /**
     * Test of localizeDateTime method, of class Localizer.
     */
    @Test
    @Ignore
    public void testLocalizeDateTime() {
        System.out.println("localizeDateTime");
        long ts = System.currentTimeMillis();
        Locale locale = Locale.CANADA;
        String expResult = "";
        String result = AbstractLocalizer.localizeTimeStamp(ts, true, locale);
        assertEquals(expResult, result);
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }

    /**
     * Test of localizeDate method, of class Localizer.
     */
    @Test
    @Ignore
    public void testLocalizeDate() {
        System.out.println("localizeDate");
        Locale locale = null;
        long ts = 0L;
        String expResult = "";
        String result = AbstractLocalizer.localizeDate(locale, ts);
        assertEquals(expResult, result);
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }

    /**
     * Test of localizeTime method, of class Localizer.
     */
    @Test
    @Ignore
    public void testLocalizeTime() {
        System.out.println("localizeTime");
        Locale locale = null;
        long ts = 0L;
        String expResult = "";
        String result = AbstractLocalizer.localizeTime(locale, ts);
        assertEquals(expResult, result);
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }
    
    @Test
    public void testLocalizeDouble() {
        System.out.println("localizeDouble");
        Locale locale = Locale.GERMAN;
        String expResult = "179,76931E306";
        String result = new MessageFormat("{0,number,##0.#####E0}", locale).format(new Object[] {Double.MAX_VALUE});
        assertEquals(expResult, result);
    }
    
    
}
