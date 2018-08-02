/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib;

import java.util.HashMap;
import java.util.Map;
import javax.servlet.jsp.JspException;
import javax.servlet.jsp.tagext.TagSupport;

/**
 *
 * @author aploese
 */
public class MapTag extends TagSupport {

    private String var;
    private Map<String, Object> map;

    public void setVar(String var) {
        this.var = var;
    }

    public void addMapEntry(String key, Object value) {
        map.put(key, value);
    }

    @Override
    public int doStartTag() throws JspException {
        map = new HashMap<>();
        if (var.isEmpty()) {
            ListTag listTag = (ListTag) findAncestorWithClass(this, ListTag.class);
            if (listTag == null) {
                throw new JspException("If no 'var' attribute is given, this Map must have a ListEntry tags ancestor");
            }
            listTag.addListEntry(map);
        } else {
            pageContext.getRequest().setAttribute(var, map);
        }
        return EVAL_BODY_INCLUDE;
    }

    @Override
    public void release() {
        super.release();
        var = null;
        map = null;
    }
}
