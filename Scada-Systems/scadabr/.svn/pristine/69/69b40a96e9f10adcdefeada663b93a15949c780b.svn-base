/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.i18n;

import java.util.Calendar;
import java.util.TimeZone;
import javax.servlet.http.HttpServletRequest;
import org.springframework.web.servlet.i18n.SessionLocaleResolver;
import org.springframework.web.util.WebUtils;

/**
 *
 * @author aploese
 */
public class LocaleResolver extends SessionLocaleResolver {

    @Override
    protected TimeZone determineDefaultTimeZone(HttpServletRequest request) {
        return Calendar.getInstance(resolveLocale(request)).getTimeZone();
    }

    public TimeZone resolveTimeZone(HttpServletRequest request) {
        TimeZone timeZone = (TimeZone) WebUtils.getSessionAttribute(request, TIME_ZONE_SESSION_ATTRIBUTE_NAME);
        if (timeZone == null) {
            timeZone = determineDefaultTimeZone(request);
        }
        return timeZone;
    }

}
