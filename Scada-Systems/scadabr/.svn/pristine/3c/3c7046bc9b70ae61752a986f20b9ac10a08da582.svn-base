/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib.dijit;

import br.org.scadabr.web.taglib.DojoInputTag;
import java.util.Date;

/**
 *
 * @author aploese
 */
public class DateTextBoxTag extends DojoInputTag {

    public DateTextBoxTag() {
        super("dijit/form/DateTextBox", "date");
    }

    private Object timestamp;

    @Override
    protected String getValue0() {
        if (timestamp instanceof Long) {
            return String.format("%tF", new Date((Long) timestamp));
        } else if (timestamp instanceof Date) {
            return String.format("%tF", (Date) timestamp);
        }
        throw new IllegalArgumentException("Cant handle: " + timestamp);
    }

    public void setTimestamp(Object value) {
        this.timestamp = value;
    }

}
