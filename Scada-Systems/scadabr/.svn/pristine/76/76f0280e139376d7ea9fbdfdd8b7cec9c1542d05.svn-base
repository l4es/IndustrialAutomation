/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.dwr;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 *
 * @author aploese
 */
public class DwrResponse {

    private List<DwrMessage> messages = new ArrayList<>();
    private Map<String, Object> data = new HashMap<>();

    public boolean getHasMessages() {
        return (messages != null) && (messages.size() > 0);
    }

    public void addGeneric(String genericMessage) {
        addMessage(new DwrMessage(genericMessage));
    }

    public void addContext(String contextKey, String contextualMessage) {
        addMessage(new DwrMessage(contextKey, contextualMessage));
    }

    public void addMessage(DwrMessage message) {
        messages.add(message);
    }

    public List<DwrMessage> getMessages() {
        return messages;
    }

    public void setMessages(List<DwrMessage> messages) {
        this.messages = messages;
    }

    public void addData(String key, Object value) {
        data.put(key, value);
    }

    public Map<String, Object> getData() {
        return data;
    }

    public void setData(Map<String, Object> data) {
        this.data = data;
    }



}
