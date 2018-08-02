/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib;

import java.util.ArrayList;
import java.util.List;
import javax.servlet.jsp.tagext.TagSupport;

/**
 *
 * @author aploese
 */
public class ListTag extends TagSupport {

    private String var;
    private List list;

    public void setVar(String var) {
        this.var = var;
    }

    public void addListEntry(Object value) {
        list.add(value);
    }

    @Override
    public int doStartTag() {
        list = new ArrayList();
        pageContext.getRequest().setAttribute(var, list);
        return EVAL_BODY_INCLUDE;
    }

    @Override
    public void release() {
        super.release();
        var = null;
    }
}
