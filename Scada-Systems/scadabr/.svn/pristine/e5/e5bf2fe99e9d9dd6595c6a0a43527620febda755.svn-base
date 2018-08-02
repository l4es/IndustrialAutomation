/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.l10n;

import br.org.scadabr.l10n.AbstractLocalizer;
import br.org.scadabr.web.i18n.LocaleResolver;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.i18n.MessageSource;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.vo.DataPointVO;
import java.text.DateFormat;
import java.util.Date;
import java.util.Locale;
import javax.inject.Inject;
import javax.inject.Named;
import javax.servlet.http.HttpServletRequest;
import org.springframework.context.annotation.Scope;

/**
 *
 * @author aploese
 */
@Named
@Scope("session")
public class RequestContextAwareLocalizer extends AbstractLocalizer {

    @Inject
    private transient HttpServletRequest request;
    @Inject
    private transient MessageSource messageSource;
    @Inject
    private transient LocaleResolver localeResolver;


    public RequestContextAwareLocalizer() {
    }

    @Override
    public String localizeTimeStamp(long ts, boolean hideDateOfToday) {
        DateFormat df;
        if (hideDateOfToday && (System.currentTimeMillis() - ts) < 86400000) {
            df = DateFormat.getTimeInstance(DateFormat.DEFAULT, getLocale());
        } else {
            df = DateFormat.getDateTimeInstance(DateFormat.DEFAULT, DateFormat.DEFAULT, getLocale());
        }
        df.setTimeZone(localeResolver.resolveTimeZone(request));
        return df.format(new Date(ts));
    }

    @Override
    public String localizeDate(long ts) {
        final DateFormat df = DateFormat.getDateInstance(DateFormat.DEFAULT, getLocale());
        df.setTimeZone(localeResolver.resolveTimeZone(request));
        return df.format(new Date(ts));
    }

    @Override
    public String localizeTime(long ts) {
        final DateFormat df = DateFormat.getTimeInstance(DateFormat.DEFAULT, getLocale());
        df.setTimeZone(localeResolver.resolveTimeZone(request));
        return df.format(new Date(ts));
    }

    @Override
    public String getMessage(String code, Object... args) {
        return messageSource.getMessage(code, args, getLocale());
    }

    @Override
    public String getMessage(LocalizableMessage localizableMessage) {
        return messageSource.getMessage(localizableMessage.getI18nKey(), localizableMessage.getArgs(), getLocale());
    }

    @Override
    public Locale getLocale() {
        return localeResolver.resolveLocale(request);
    }

    @Override
    public <T extends PointValueTime> String formatValueAndUnit(DataPointVO<?, T> dpVo, T mv) {
        return getMessageRender(dpVo.getValueAndUnitPattern(mv), getLocale()).format(new Object[]{mv.getValue(), dpVo.getUnit()});
    }


    @Override
    public <T extends PointValueTime> String formatValue(DataPointVO<?, T> dpVo, T mv) {
        return getMessageRender(dpVo.getValuePattern(mv), getLocale()).format(mv.getValue());
    }

}
