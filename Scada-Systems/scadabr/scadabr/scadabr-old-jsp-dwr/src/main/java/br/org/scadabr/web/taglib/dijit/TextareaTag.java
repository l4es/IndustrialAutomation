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
public class TextareaTag extends DojoInputTag {

    private String text;
    
    public TextareaTag() {
        super("dijit/form/Textarea", "text");
    }
    
    
    
    public void setText(String text) {
        this.text = text;
    }

    @Override
    protected String getValue0() {
        return text;
    }

}
