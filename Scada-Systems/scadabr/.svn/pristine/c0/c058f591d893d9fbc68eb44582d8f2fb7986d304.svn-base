/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.dwr;

import br.org.scadabr.utils.i18n.LocalizableException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * @TODO rename this properly
 * @author aploese
 */
@Deprecated
public class DwrResponseI18n {

    private List<DwrMessageI18n> messages = new ArrayList<>();
    private Map<String, Object> data = new HashMap<>();

    public void add(DwrMessageI18n message) {
        messages.add(message);
    }

    public void addContextual(String contexKey, String i18nKey, Object... args) {
        add(new DwrMessageI18n(contexKey, i18nKey, args));
    }

    public void addContextual(String contexKey, LocalizableException e) {
        add(new DwrMessageI18n(contexKey, e.getI18nKey(), e.getArgs()));
    }

    public void addContextual(String contexKey, String i18nKey, Throwable t) {
        add(new DwrMessageI18n(contexKey, i18nKey, t.getMessage()));
    }

    public void addGeneric(String i18nKey, Object... args) {
        add(new DwrMessageI18n(null, i18nKey, args));
    }

    public void addGeneric(LocalizableException e) {
        add(new DwrMessageI18n(null, e.getI18nKey(), e.getArgs()));
    }

    public void addGeneric(String i18nKey, Throwable t) {
        add(new DwrMessageI18n(null, i18nKey, t.getMessage()));
    }

    public void setMessages(List<DwrMessageI18n> messages) {
        this.messages = messages;
    }

    //Todo change to hasMessages???
    public boolean isEmpty() {
        return messages.isEmpty();
    }

    public Iterable<DwrMessageI18n> getMessages() {
        return messages;
    }

    public void addData(String i18nKey, Object value) {
        data.put(i18nKey, value);
    }

    public Map<String, Object> getData() {
        return data;
    }
}
