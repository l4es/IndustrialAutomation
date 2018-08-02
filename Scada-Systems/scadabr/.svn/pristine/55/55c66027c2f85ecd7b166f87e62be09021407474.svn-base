/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib.dijit;

import javax.servlet.jsp.JspException;
import org.springframework.validation.FieldError;
import org.springframework.web.servlet.tags.form.AbstractHtmlInputElementTag;
import org.springframework.web.servlet.tags.form.TagWriter;

/**
 *
 * @author aploese
 */
public class LoginTextBoxTag extends AbstractHtmlInputElementTag {

    private String i18nLabel;
    private String i18nTitle;
    private String type;

    @Override
    public void release() {
        super.release();
        i18nLabel = null;
        i18nTitle = null;
        type = null;
    }

    @Override
    protected int writeTagContent(TagWriter tagWriter) throws JspException {
        tagWriter.startTag("input");

        writeDefaultAttributes(tagWriter);
        tagWriter.writeOptionalAttributeValue("type", type);

        final String value = getValue();
        tagWriter.writeAttribute("value", value);

        // custom optional attributes
        tagWriter.writeAttribute("label", getRequestContext().getMessage(i18nLabel) + ":");
        tagWriter.writeAttribute("title", getRequestContext().getMessage(i18nTitle != null ? i18nTitle : i18nLabel));
        if (getBindStatus().getErrors().hasFieldErrors(getBindStatus().getExpression())) {
            tagWriter.writeAttribute("data-dojo-type", "dijit/form/ValidationTextBox");
            StringBuilder sb = new StringBuilder();
            sb.append("_isEmpty: function(value) {return false;},\n"); //TODO Workaround to show error message see ValidationTextBox.validate
            sb.append("validator : function(value, constraints){return ");
            sb.append(getBindStatus().getErrors().hasErrors() ? (value == null ? "''" : "'" + value + "'") + " != value" : "true");
            sb.append(";},\n invalidMessage:'");
            for (FieldError fe : getBindStatus().getErrors().getFieldErrors(getBindStatus().getExpression())) {
                sb.append(getRequestContext().getMessage(fe, true));
            }
            sb.append("'");
            tagWriter.writeAttribute("data-dojo-props", sb.toString());
        } else {
            tagWriter.writeAttribute("data-dojo-type", "dijit/form/TextBox");
        }

        tagWriter.endTag();
        return SKIP_BODY;
    }

    /**
     * Writes the '{@code value}' attribute to the supplied {@link TagWriter}.
     * Subclasses may choose to override this implementation to control exactly
     * when the value is written.
     * @return 
     * @throws javax.servlet.jsp.JspException
     */
    protected String getValue() throws JspException {
        String value = getDisplayString(getBoundValue(), getPropertyEditor());
        return processFieldValue(getName(), value, "text");
    }

    /**
     * @param i18nLabel the i18nLabel to set
     */
    public void setI18nLabel(String i18nLabel) {
        this.i18nLabel = i18nLabel;
    }

    /**
     * @param i18nTitle the i18nTitle to set
     */
    public void setI18nTitle(String i18nTitle) {
        this.i18nTitle = i18nTitle;
    }

    /**
     * @param type the type to set
     */
    public void setType(String type) {
        this.type = type;
    }

}
