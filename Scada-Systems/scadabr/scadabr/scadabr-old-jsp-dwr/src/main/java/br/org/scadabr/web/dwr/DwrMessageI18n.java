/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.dwr;

import br.org.scadabr.utils.i18n.LocalizableMessage;

/**
 * @TODO rename this properly
 *
 * @author aploese
 */
public class DwrMessageI18n implements LocalizableMessage {

    private final String contextKey;
    private final String i18nKey;
    private final Object[] args;

    public DwrMessageI18n(String contextKey, String i18nKey, Object... args) {
        this.contextKey = contextKey;
        this.i18nKey = i18nKey;
        this.args = args;
    }

    public boolean isInContext() {
        return contextKey != null;
    }

    public String getContextKey() {
        return contextKey;
    }

    @Override
    public String getI18nKey() {
        return i18nKey;
    }

    @Override
    public Object[] getArgs() {
        return args;
    }

}
