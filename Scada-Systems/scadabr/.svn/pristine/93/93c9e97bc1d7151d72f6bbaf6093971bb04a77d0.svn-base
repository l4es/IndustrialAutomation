/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib;

import java.util.List;
import javax.servlet.jsp.JspException;
import javax.servlet.jsp.tagext.TagSupport;

/**
 *
 * @author aploese
 */
public class ListEntryTag extends TagSupport {

    private Object value;
    private String listVar;

    public void setValue(Object value) {
        this.value = value;
    }

    public void setListVar(String listVar) {
        this.listVar = listVar;
    }

    @Override
    public int doStartTag() throws JspException {
        if (listVar.isEmpty()) {
            ListTag listTag = (ListTag) findAncestorWithClass(this, ListTag.class);
            if (listTag == null) {
                throw new JspException("ListEntry tags must be used within a list tag");
            }
            listTag.addListEntry(value);
        } else {
            List list = (List) pageContext.getRequest().getAttribute(listVar);
            if (list == null) {
                throw new JspException("A list with the var name '" + listVar + "' was not found");
            }
            list.add(value);
        }
        return EVAL_BODY_INCLUDE;
    }

    @Override
    public void release() {
        super.release();
        value = null;
    }
}
