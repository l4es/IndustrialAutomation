/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.propertyEditor;

import java.beans.PropertyEditorSupport;
import java.text.DecimalFormat;
import java.text.ParseException;

/**
 *
 * @author aploese
 */
public class IntegerFormatEditor extends PropertyEditorSupport {

    private final DecimalFormat format;
    private final boolean hideZero;

    public IntegerFormatEditor(DecimalFormat format, boolean hideZero) {
        this.format = format;
        this.hideZero = hideZero;
    }

    @Override
    public void setAsText(String text) throws IllegalArgumentException {
            try {
                setValue(format.parse(text).intValue());
            } catch (ParseException e) {
                throw new IllegalArgumentException(e);
            }
    }
    
    @Override
    public String getAsText() {
        final String result = format.format(getValue());
        if (hideZero && "0".equals(result)) {
            return "";
        } else {
            return result;
        }
    }

}
