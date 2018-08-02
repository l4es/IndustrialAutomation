/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.utils.i18n;

/**
 *
 * @author aploese
 */
public class LocalizableMessageImpl implements LocalizableMessage {

    private final String i18nKey;
    private final Object[] args;

    public LocalizableMessageImpl(String i18nKey, Object... args) {
        this.i18nKey = i18nKey;
        this.args = args;
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
