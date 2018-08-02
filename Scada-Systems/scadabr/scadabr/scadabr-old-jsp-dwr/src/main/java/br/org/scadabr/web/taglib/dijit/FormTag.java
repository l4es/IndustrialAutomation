/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib.dijit;

import br.org.scadabr.web.taglib.DojoTag;
import javax.servlet.jsp.JspException;
import org.springframework.web.servlet.tags.form.TagWriter;

/**
 *
 * @author aploese
 */
public class FormTag extends DojoTag {

    public FormTag() {
        super("form", "dijit/form/Form");
    }

    private String action;
    private String method;

    @Override
    public void release() {
        super.release();
        id = null;
    }

    @Override
    protected void writeAttributes(TagWriter tagWriter) throws JspException {
        tagWriter.writeAttribute("action", action);
        tagWriter.writeAttribute("method", method);
    }

    /**
     * @param action the action to set
     */
    public void setAction(String action) {
        this.action = action;
    }

    /**
     * @param method the method to set
     */
    public void setMethod(String method) {
        this.method = method;
    }

}
