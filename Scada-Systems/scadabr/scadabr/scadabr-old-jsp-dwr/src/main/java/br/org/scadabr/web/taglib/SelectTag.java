/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib;

import java.io.IOException;
import javax.servlet.jsp.JspException;
import javax.servlet.jsp.JspTagException;
import javax.servlet.jsp.JspWriter;
import javax.servlet.jsp.tagext.TagSupport;
import static br.org.scadabr.web.taglib.Functions.printAttribute;

/**
 *
 * @author aploese
 */
public class SelectTag extends TagSupport {

    private String name;
    private String value;
    private String size;
    private String styleClass;
    private String onchange;
    private String onclick;
    private String onblur;
    private String onmouseover;
    private String onmouseout;
    private String style;

    public void setName(String name) {
        this.name = name;
    }

    public void setValue(String value) {
        this.value = value;
    }

    public void setSize(String size) {
        this.size = size;
    }

    public void setOnchange(String onchange) {
        this.onchange = onchange;
    }

    public void setOnblur(String onblur) {
        this.onblur = onblur;
    }

    public void setOnclick(String onclick) {
        this.onclick = onclick;
    }

    public void setOnmouseover(String onmouseover) {
        this.onmouseover = onmouseover;
    }

    public void setOnmouseout(String onmouseout) {
        this.onmouseout = onmouseout;
    }

    public void setStyleClass(String styleClass) {
        this.styleClass = styleClass;
    }

    public void setStyle(String style) {
        this.style = style;
    }

    @Override
    public void release() {
        super.release();
        id = null;
        name = null;
        value = null;
        size = null;
        styleClass = null;
        onchange = null;
        onclick = null;
        onblur = null;
        onmouseover = null;
        onmouseout = null;
        style = null;
    }

    @Override
    public int doStartTag() throws JspException {
        try {
            JspWriter out = pageContext.getOut();

            out.print("<select");
            printAttribute(out, "id", id);
            printAttribute(out, "name", name);
            printAttribute(out, "size", size);
            printAttribute(out, "class", styleClass);
            printAttribute(out, "onchange", onchange);
            printAttribute(out, "onclick", onclick);
            printAttribute(out, "onblur", onblur);
            printAttribute(out, "onmouseover", onmouseover);
            printAttribute(out, "onmouseout", onmouseout);
            printAttribute(out, "style", style);
            out.println(">");
        } catch (IOException ex) {
            throw new JspTagException(ex.getMessage());
        }
        return EVAL_BODY_INCLUDE;
    }

    @Override
    public int doEndTag()
            throws JspException {
        try {
            JspWriter out = pageContext.getOut();
            out.print("</select>");
        } catch (IOException ex) {
            throw new JspTagException(ex.getMessage());
        }
        return EVAL_PAGE;
    }

    public String getValue() {
        return value;
    }
}
