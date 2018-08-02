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
public class ButtonTag extends DojoTag {

    public ButtonTag() {
        super("button", "dijit/form/Button");
    }

    private String name;
    private String value;
    private String i18nLabel;
    private String i18nTitle;
    private String type;
    private boolean disabled;
    private TagWriter tagWriter;

    @Override
    protected void writeAttributes(TagWriter tagWriter) throws JspException {
        tagWriter.writeOptionalAttributeValue("type", type);
        tagWriter.writeOptionalAttributeValue("name", name);
        if (i18nTitle != null) {
            tagWriter.writeAttribute("title", getRequestContext().getMessage(i18nTitle));
        }
        if (disabled) {
            tagWriter.writeAttribute("disabled", "true");
        }
    }

    @Override
    public int writeTagContent(TagWriter tagWriter) throws JspException {
        // make Button as wide as image...
        if (i18nLabel == null && !containsPropsKey("showLabel") && containsPropsKey("iconClass")) {
            putDataDojoProp("showLabel", false);
        }
        super.writeTagContent(tagWriter);
        if (i18nLabel != null) {
            tagWriter.appendValue(getRequestContext().getMessage(i18nLabel));
        }
        return EVAL_BODY_INCLUDE;
    }

    /**
     * @param name the name to set
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     * @param value the value to set
     */
    public void setValue(String value) {
        this.value = value;
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

    /**
     * @param iconClass the iconClass to set
     */
    public void setIconClass(String iconClass) {
        putDataDojoProp("iconClass", iconClass);
    }

    /**
     * @param disabled the disabled to set
     */
    public void setDisabled(boolean disabled) {
        this.disabled = disabled;
    }

}
