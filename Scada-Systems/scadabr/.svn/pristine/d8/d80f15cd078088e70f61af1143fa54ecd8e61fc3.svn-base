/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package br.org.scadabr.web.dwr;

/**
 *
 * @author aploese
 */
public class DwrMessage {
    private final String contextKey;
    private final String message;

    /**
     * Creates a contextual message
     * @param contextKey
     * @param message 
     */
    public DwrMessage(String contextKey, String message) {
        this.contextKey = contextKey;
        this.message = message;
    }

    /**
     * Creates a generic message
     * @param message 
     */
    public DwrMessage(String message) {
        this.contextKey = null;
        this.message = message;
    }

    public boolean isInContext(){
        return contextKey != null;
    }
    
    public String getContextKey() {
        return contextKey;
    }

    public String getMessage() {
        return message;
    }

    @Override
    public String toString() {
        return isInContext() ? contextKey + " --> " + message : message;
    }
}
