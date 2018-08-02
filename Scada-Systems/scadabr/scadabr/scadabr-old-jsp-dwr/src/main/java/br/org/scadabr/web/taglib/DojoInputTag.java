/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib;

import javax.servlet.jsp.JspException;
import org.springframework.web.servlet.tags.form.TagWriter;

/**
 *
 * @author aploese
 */
public abstract class DojoInputTag extends DojoTag {

    public DojoInputTag(String dojoDataType, String type) {
        super(SKIP_BODY, "input", dojoDataType);
        this.type = type;
    }

    private String i18nLabel;
    private String i18nTitle;
    private String type;
    private String name;

    @Override
    protected void writeAttributes(TagWriter tagWriter) throws JspException {
        tagWriter.writeOptionalAttributeValue("type", type);
        tagWriter.writeOptionalAttributeValue("name", name);
        tagWriter.writeAttribute("label", getRequestContext().getMessage(i18nLabel) + ":");
        tagWriter.writeAttribute("title", getRequestContext().getMessage(i18nTitle != null ? i18nTitle : i18nLabel));
        tagWriter.writeOptionalAttributeValue("value", getValue0());
    }

    protected abstract String getValue0();

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

    /**
     * @param name the name to set
     */
    public void setName(String name) {
        this.name = name;
    }

}
