package br.org.scadabr.i18n;

import br.org.scadabr.utils.i18n.LocalizableMessage;
import java.util.Locale;
import java.util.MissingResourceException;
import java.util.ResourceBundle;

public class MessageSource extends org.springframework.context.support.ResourceBundleMessageSource {

    public MessageSource() {
        super();
    }
    
    public MessageSource(String basename) {
        super();
        this.setBasename(basename);
    }
    
    @Override
    protected ResourceBundle doGetBundle(String basename, Locale locale) throws MissingResourceException {
        return ResourceBundle.getBundle(basename, locale, getBundleClassLoader());
    }

    /**
     * Take care of Localizable Message in Args
     *
     * @param code
     * @param args
     * @param locale
     * @return
     */
    @Override
    protected String getMessageInternal(String code, Object[] args, Locale locale) {
        if (args == null || args.length == 0) {
            return super.getMessageInternal(code, args, locale);
        }
        Object[] localizedArgs = new Object[args.length];
        for (int i = 0; i < args.length; i++) {
            if (args[i] instanceof LocalizableMessage) {
                final LocalizableMessage localizableMessage = (LocalizableMessage) args[i];
                localizedArgs[i] = getMessage(localizableMessage, locale);
            } else {
                localizedArgs[i] = args[i];
            }
        }
        return super.getMessageInternal(code, localizedArgs, locale);
    }

    public String getMessage(String code, Locale locale) {
        return super.getMessage(code, null, locale);
    }

    public String getMessage(LocalizableMessage i18nMessage, Locale locale) {
        return getMessage(i18nMessage.getI18nKey(), ((LocalizableMessage) i18nMessage).getArgs(), locale);
    }

}
