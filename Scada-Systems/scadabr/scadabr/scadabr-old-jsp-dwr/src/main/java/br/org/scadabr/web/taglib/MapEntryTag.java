/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib;

import javax.servlet.jsp.JspException;
import javax.servlet.jsp.tagext.TagSupport;

/**
 *
 * @author aploese
 */
public class MapEntryTag extends TagSupport {

    private String key;
    private Object value;

    public void setKey(String key) {
        this.key = key;
    }

    public void setValue(Object value) {
        this.value = value;
    }

    @Override
    public int doStartTag() throws JspException {
        MapTag mapTag = (MapTag) findAncestorWithClass(this, MapTag.class);
        if (mapTag == null) {
            throw new JspException("MapEntry tags must be used within a map tag");
        }
        mapTag.addMapEntry(key, value);

        return EVAL_BODY_INCLUDE;
    }

    @Override
    public void release() {
        super.release();
        key = null;
        value = null;
    }
}
