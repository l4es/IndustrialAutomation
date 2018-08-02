/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib.dojo;

import java.util.HashMap;
import java.util.Map;

/**
 *
 * @author aploese
 */
public class DataDojoProps {

    private Map<String, Object> props;

    public boolean containsKey(String key) {
        return props == null ? false : props.containsKey(key);
    }

    public void put(String key, Object value) {
        if (props == null) {
            props = new HashMap<>();
        }
        props.put(key, value);
    }

    public String getString() {
        if (props == null) {
            return null; 
        }
        StringBuilder sb = new StringBuilder();
        boolean firstProp = true;
            for (String prop : props.keySet()) {
                if (firstProp) {
                    firstProp = false;
                } else {
                    sb.append(", ");
                }
                sb.append(prop);
                sb.append(": ");
                final Object propValue = props.get(prop);
                if (propValue instanceof Boolean) {
                    sb.append(propValue.toString());
                } else if (propValue instanceof Number) {
                    sb.append(propValue.toString());
                } else {
                    sb.append('\'');
                    sb.append(propValue.toString());
                    sb.append('\'');
                }
            }
        return sb.toString();
    }

    public boolean isEmpty() {
        return props == null ? true : props.isEmpty();
    }

}
