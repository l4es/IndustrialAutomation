/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib.dijit;

import br.org.scadabr.web.taglib.DojoInputTag;

/**
 *
 * @author aploese
 */
public class NumberSpinnerTag extends DojoInputTag {

    private Number number;
    
    public NumberSpinnerTag() {
        super("dijit/form/NumberSpinner", "number");
    }
    
    @Override
    protected String getValue0() {
        return number.toString();
    }

    
    public void setNumber(Number number) {
        this.number = number;
    } 

}
