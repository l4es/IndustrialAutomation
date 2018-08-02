/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib;

import java.io.IOException;
import java.util.Objects;
import javax.servlet.jsp.JspException;
import javax.servlet.jsp.JspTagException;
import javax.servlet.jsp.JspWriter;
import javax.servlet.jsp.tagext.TagSupport;
import static br.org.scadabr.web.taglib.Functions.printAttribute;

/**
 *
 * @author aploese
 */
public class CheckboxTag extends TagSupport {

    private String name;
    private String value = Boolean.TRUE.toString();
    private String selectedValue;
    private String styleClass;
    private String style;
    private String onclick;

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getOnclick() {
        return onclick;
    }

    public void setOnclick(String onclick) {
        this.onclick = onclick;
    }

    public String getSelectedValue() {
        return selectedValue;
    }

    public void setSelectedValue(String selectedValue) {
        this.selectedValue = selectedValue;
    }

    public String getStyle() {
        return style;
    }

    public void setStyle(String style) {
        this.style = style;
    }

    public String getStyleClass() {
        return styleClass;
    }

    public void setStyleClass(String styleClass) {
        this.styleClass = styleClass;
    }

    public String getValue() {
        return value;
    }

    public void setValue(String value) {
        this.value = value;
    }

    @Override
    public void release() {
        super.release();
        name = null;
        value = null;
        selectedValue = null;
        styleClass = null;
        onclick = null;
        style = null;
    }

    @Override
    public int doEndTag() throws JspException {
        try {
            final JspWriter out = pageContext.getOut();

            out.print("<input");
            printAttribute(out, "type", "checkbox");
            printAttribute(out, "id", id);
            printAttribute(out, "name", name);
            printAttribute(out, "value", value);
            printAttribute(out, "class", styleClass);
            printAttribute(out, "onclick", onclick);
            printAttribute(out, "style", style);
            if (Objects.equals(value, selectedValue)) {
                printAttribute(out, "checked", "checked");
            }
            out.println("/>");
        } catch (IOException ex) {
            throw new JspTagException(ex.getMessage());
        }
        return EVAL_PAGE;
    }
}
