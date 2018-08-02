/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib.dijit;

import br.org.scadabr.web.taglib.DojoInputTag;
import javax.servlet.jsp.JspException;
import org.springframework.web.servlet.tags.form.TagWriter;

/**
 *
 * @author aploese
 */
public class CheckBoxTag extends DojoInputTag {

    private boolean checked;
    
    public CheckBoxTag() {
        super("dijit/form/CheckBox", "checkbox");
    }

    @Override
    protected void writeAttributes(TagWriter tagWriter) throws JspException {
        super.writeAttributes(tagWriter);
        if (checked) {
            tagWriter.writeAttribute("checked", Boolean.toString(checked));
        }
    }
    
    
    @Override
    protected String getValue0() {
        return Boolean.toString(checked);
    }
    
    
    public void setChecked(boolean checked) {
        this.checked = checked;
    }
    
    
}
