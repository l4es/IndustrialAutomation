package br.org.scadabr.l10n;

import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import java.io.Serializable;
import java.text.DateFormat;
import java.text.MessageFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;
import java.util.ResourceBundle;
import java.util.logging.Logger;

/**
 * This class localizes I18N keys and messages with the default #ResourceBundle
 * or an given one.
 *
 */
public abstract class AbstractLocalizer implements Serializable, Localizer {

    /**
     * the logger to use.
     */
    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_CORE);

    //TODO APL move this map out to a Singleton bean?
    private final Map<Locale, Map<String, MessageFormat>> pvtMessageFormatMap = new HashMap<>();

    protected MessageFormat getMessageRender(String messagePattern, final Locale locale) {
        synchronized (pvtMessageFormatMap) {
            Map<String, MessageFormat> localizedMap = pvtMessageFormatMap.get(locale);
            if (localizedMap == null) {
                localizedMap = new HashMap<>();
                pvtMessageFormatMap.put(getLocale(), localizedMap);
            }
            MessageFormat result = localizedMap.get(messagePattern);
            if (result == null) {
                result = new MessageFormat(messagePattern, locale);
                localizedMap.put(messagePattern, result);
            }
            return result;
        }
    }

    /**
     * Localize the message.
     *
     * @param i18nKey the I18N key
     * @param bundle the resource bundle to use.
     * @param args the parameter to pass.
     * @return the localizes I18n message.
     */
    @Deprecated // use this instance as bean
    public static String localizeI18nKey(String i18nKey, ResourceBundle bundle, Object... args) {
        Object[] localizedArgs = new Object[args.length];
        for (int i = 0; i < args.length; i++) {
            if (args[i] instanceof LocalizableMessage) {
                final LocalizableMessage localizableMessage = (LocalizableMessage) args[i];
                localizedArgs[i] = localizeMessage(localizableMessage, bundle);
            } else {
                localizedArgs[i] = args[i];
            }
        }
        return new MessageFormat(bundle.getString(i18nKey), bundle.getLocale()).format(localizedArgs);
    }

    /**
     * Localize the message.
     *
     * @param i18nMessage the I18N message.
     * @param bundle the bundle to use
     * @return the localized I18N message.
     */
    @Deprecated // use this instance as bean
    public static String localizeMessage(LocalizableMessage i18nMessage, ResourceBundle bundle) {
        return localizeI18nKey(i18nMessage.getI18nKey(), bundle, ((LocalizableMessage) i18nMessage).getArgs());
    }

    //TODO set TimeZone ???
    @Deprecated // use this instance as bean
    public static String localizeTimeStamp(long ts, boolean hideDateOfToday, Locale locale) {
        if (hideDateOfToday && (System.currentTimeMillis() - ts) < 86400000) {
            return DateFormat.getTimeInstance(DateFormat.DEFAULT, locale).format(new Date(ts));
        } else {
            return DateFormat.getDateTimeInstance(DateFormat.DEFAULT, DateFormat.DEFAULT, locale).format(new Date(ts));
        }
    }

    @Deprecated // use this instance as bean
    public static String localizeDate(Locale locale, long ts) {
        return DateFormat.getDateInstance(DateFormat.DEFAULT, locale).format(new Date(ts));
    }

    @Deprecated // use this instance as bean
    public static String localizeTime(Locale locale, long ts) {
        return DateFormat.getTimeInstance(DateFormat.DEFAULT, locale).format(new Date(ts));
    }

}
