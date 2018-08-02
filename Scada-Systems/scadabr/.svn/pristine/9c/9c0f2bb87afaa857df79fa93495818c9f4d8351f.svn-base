/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.i18n;

import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import java.text.MessageFormat;
import java.util.Locale;
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
public class LocalizableMessageImplTest {

    public LocalizableMessageImplTest() {
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

    @Test
    public void testBoolArg() throws Exception {
        System.out.println("testBoolArg");
        LocalizableMessage instance = new LocalizableMessageImpl("i18nKey", true, false, Boolean.TRUE, Boolean.FALSE);
        String expResult = "i18nKey|\\Ztrue|\\Zfalse|\\Ztrue|\\Zfalse|";
        //expResult = new br.org.scadabr.i18n.LocalizableMessage(instance.getI18nKey(), instance.getArgs()).serialize();
        String serialized = I18NUtils.serialize(instance);
        assertEquals(expResult, serialized);
        LocalizableMessage deserialized = I18NUtils.deserialize(serialized);
        assertEquals(instance.getI18nKey(), deserialized.getI18nKey());
        assertArrayEquals(instance.getArgs(), deserialized.getArgs());
    }

    @Test
    public void testNumberArg() throws Exception {
        System.out.println("testBoolArg");
        LocalizableMessage instance = new LocalizableMessageImpl("i18nKey", "\\B127", Byte.MAX_VALUE, Short.MAX_VALUE, Integer.MAX_VALUE, Long.MAX_VALUE, Float.MAX_VALUE, Double.MAX_VALUE);
        String expResult = "i18nKey|\\\\B127|\\B127|\\S32767|\\I2147483647|\\J9223372036854775807|\\F3.4028235E38|\\D1.7976931348623157E308|";
        //expResult = new br.org.scadabr.i18n.LocalizableMessage(instance.getI18nKey(), instance.getArgs()).serialize();
        String serialized = I18NUtils.serialize(instance);
        assertEquals(expResult, serialized);
        LocalizableMessage deserialized = I18NUtils.deserialize(serialized);
        assertEquals(instance.getI18nKey(), deserialized.getI18nKey());
        assertArrayEquals(instance.getArgs(), deserialized.getArgs());
    }

    @Test
    public void testSpecialChars() throws Exception {
        System.out.println("testSpecialChars");
        LocalizableMessage instance = new LocalizableMessageImpl("i18nKey", "\\", "|", "[", "]", "|[dummykey]");
        String expResult = "i18nKey|\\\\|\\||\\[|\\]|\\|\\[dummykey\\]|";
        //expResult = new br.org.scadabr.i18n.LocalizableMessage(instance.getI18nKey(), instance.getArgs()).serialize(); // This handles a "\" not correctly, we do ;-)
        String serialized = I18NUtils.serialize(instance);
        assertEquals(expResult, serialized);
        LocalizableMessage deserialized = I18NUtils.deserialize(serialized);
        assertEquals(instance.getI18nKey(), deserialized.getI18nKey());
        assertEquals(expResult, I18NUtils.serialize(deserialized));
    }

    /**
     * Test of getI18nKey method, of class LocalizableMessageImpl.
     */
    @Test
    public void testNested() throws Exception {
        System.out.println("testNested");
        LocalizableMessage instance = new LocalizableMessageImpl("i18nKey", null, new LocalizableMessageImpl("someKey"));
        String expResult = "i18nKey||[someKey|]|";
        //expResult = new br.org.scadabr.i18n.LocalizableMessage("i18nKey", null, new br.org.scadabr.i18n.LocalizableMessage("someKey")).serialize(); // Orig after a Localizable MSG no | was written
        String serialized = I18NUtils.serialize(instance);
        assertEquals(expResult, serialized);
        LocalizableMessage deserialized = I18NUtils.deserialize(serialized);
        assertEquals(expResult, I18NUtils.serialize(deserialized));
    }
    
    @Test
    public void testAuditEventWithDouble() throws Exception {
        System.out.println("testAuditEventWithDouble");
        String data = "event.audit.added|admin (1)|[event.audit.dataPoint|]|\\I4|[event.audit.propertyList.15|[event.audit.property|[common.xid|]|DP_593013|]|[event.audit.property|[dsEdit.points.name|]|aa|]|[event.audit.property|[common.enabled|]|[common.false|]|]|[event.audit.property|[pointEdit.logging.type|]|[pointEdit.logging.type.change|]|]|[event.audit.property|[pointEdit.logging.period|]|[common.tp.description|\\I15|[common.tp.minutes|]|]|]|[event.audit.property|[pointEdit.logging.valueType|]|[pointEdit.logging.valueType.instant|]|]|[event.audit.property|[pointEdit.logging.tolerance|]|\\D0.0|]|[event.audit.property|[pointEdit.logging.purge|]|[common.tp.description|\\I1|[common.tp.years|]|]|]|[event.audit.property|[pointEdit.logging.defaultCache|]|\\I1|]|[event.audit.property|[pointEdit.logging.discard|]|[common.false|]|]|[event.audit.property|[pointEdit.logging.discardLow|]|\\D-1.7976931348623157E308|]|[event.audit.property|[pointEdit.logging.discardHigh|]|\\D1.7976931348623157E308|]|[event.audit.property|[pointEdit.logging.engineeringUnits|]|\\I95|]|[event.audit.property|[pointEdit.props.chartColour|]||]|[event.audit.property|[dsEdit.vmstat.attribute|]|MEMORY_BUFF|]|]|";
        LocalizableMessage lm = I18NUtils.deserialize(data);
        assertEquals(data, I18NUtils.serialize(lm));
    }
    
    @Test
    public void testMessageFormat() throws Exception {
        MessageFormat mf = new MessageFormat("{0}={1,number,##0.#####E0}", Locale.GERMANY);
        String result = mf.format(new Object[] {"NAME", 31415.12});
        assertEquals("NAME=31,41512E3", result);
    }
}
