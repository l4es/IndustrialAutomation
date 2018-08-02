/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.util;

import br.org.scadabr.web.dwr.DwrMessage;
import br.org.scadabr.web.dwr.DwrMessageI18n;
import br.org.scadabr.web.dwr.DwrResponse;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import org.springframework.validation.BindException;

/**
 *
 * @author aploese
 */
public class ValidationUtils {

    public static void reject(BindException errors, String errorCode, Object... args) {
        errors.reject(errorCode, args, "???" + errorCode + "(10)???");
    }

    public static void rejectValue(BindException errors, String field, String errorCode, Object... args) {
        errors.rejectValue(field, errorCode, args, "???" + errorCode + "(11)???");
    }

    public static void reject(BindException errors, String fieldPrefix, DwrResponse response) {
        for (DwrMessage m : response.getMessages()) {
            if (m.isInContext() ) {
                rejectValue(errors, fieldPrefix + m.getContextKey(), m.getMessage(), new Object[0]);
            } else {
                reject(errors, m.getMessage(), new Object[0]);
            }
        }
    }

    public static void reject(BindException errors, String fieldPrefix, DwrResponseI18n response) {
        for (DwrMessageI18n m : response.getMessages()) {
            if (m.isInContext()) {
                rejectValue(errors, fieldPrefix + m.getContextKey(), m.getI18nKey(), m.getArgs());
            } else {
                reject(errors, m.getI18nKey(), m.getArgs());
            }
        }
    }

}
