/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib;

import br.org.scadabr.web.taglib.dojo.DataDojoProps;
import javax.servlet.jsp.JspException;
import org.springframework.web.servlet.tags.form.AbstractFormTag;
import org.springframework.web.servlet.tags.form.TagWriter;

/**
 *
 * @author aploese
 */
public class DojoTag extends AbstractFormTag {

    private final DataDojoProps dataDojoProps = new DataDojoProps();
    private final String dojoDataType;
    private final String tag;
    private TagWriter tagWriter;
    private final int tagReturn;

    public DojoTag(String tag, String dojoDataType) {
        this.tag = tag;
        this.dojoDataType = dojoDataType;
        this.tagReturn = EVAL_BODY_INCLUDE;
    }

    public DojoTag(int tagReturn, String tag, String dojoDataType) {
        this.tag = tag;
        this.dojoDataType = dojoDataType;
        this.tagReturn = tagReturn;
    }

    @Override
    protected int writeTagContent(TagWriter tagWriter) throws JspException {
        this.tagWriter = tagWriter;
        tagWriter.startTag(tag);
        if (getId() != null) {
            tagWriter.writeAttribute("id", getId());
        }
        tagWriter.writeAttribute("data-dojo-type", dojoDataType);
        writeAttributes(tagWriter);
        if (!dataDojoProps.isEmpty()) {
            tagWriter.writeAttribute("data-dojo-props", dataDojoProps.getString());
        }

        tagWriter.forceBlock();

        return tagReturn;
    }

    protected void writeAttributes(TagWriter tagWriter) throws JspException {
    }

    protected boolean containsPropsKey(String key) {
        return dataDojoProps.containsKey(key);
    }

    protected void putDataDojoProp(String key, Object value) {
        dataDojoProps.put(key, value);
    }

    /**
     * Closes the '{@code div}' block tag.
     *
     * @return
     * @throws javax.servlet.jsp.JspException
     */
    @Override
    public int doEndTag() throws JspException {
        this.tagWriter.endTag();
        return EVAL_PAGE;
    }

}
