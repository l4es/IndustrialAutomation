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
import javax.servlet.jsp.tagext.BodyTagSupport;
import static br.org.scadabr.web.taglib.Functions.printAttribute;

/**
 *
 * @author aploese
 */
public class OptionTag extends BodyTagSupport {

    private String value;

    public String getValue() {
        return value;
    }

    public void setValue(String value) {
        this.value = value;
    }

    @Override
    public int doStartTag() {
        if (getBodyContent() != null) {
            getBodyContent().clearBody();
        }
        return EVAL_BODY_AGAIN;
    }

    @Override
    public int doEndTag() throws JspException {
        try {
            String content = getBodyContent() != null ? getBodyContent().getString() : null;

            JspWriter out = pageContext.getOut();

            out.print("<option");
            printAttribute(out, "value", value);

            SelectTag selectTag = (SelectTag) findAncestorWithClass(this, SelectTag.class);
            if (selectTag != null) {
                if (value != null) {
                    if (value.equals(selectTag.getValue())) {
                        out.print(" selected=\"selected\"");
                    }
                } else if ((content != null)
                        && (content.equals(selectTag.getValue()))) {
                    out.print(" selected=\"selected\"");
                }
            }
            out.print(">");
            if (content != null) {
                out.print(content);
            }
            out.print("</option>");
        } catch (IOException ex) {
            throw new JspTagException(ex.getMessage());
        }
        return EVAL_PAGE;
    }

    @Override
    public void release() {
        super.release();
        value = null;
    }
}
