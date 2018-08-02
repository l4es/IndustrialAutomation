/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.mvc;

/**
 *
 * @author aploese
 */
public class JsonConstraintViolation {
    private final String path;
    private final String msg;
    private final Object rejectedValue;

    public JsonConstraintViolation(String path, Object rejectedValue, String msg) {
        this.path = path;
        this.rejectedValue = rejectedValue;
        this.msg = msg;
    }

    /**
     * @return the msg
     */
    public String getMsg() {
        return msg;
    }

    /**
     * @return the rejectedValue
     */
    public Object getRejectedValue() {
        return rejectedValue;
    }

    /**
     * @return the path
     */
    public String getPath() {
        return path;
    }
    
}
