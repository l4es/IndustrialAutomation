/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib.dojox;

import br.org.scadabr.web.taglib.DojoTag;
import javax.servlet.jsp.JspException;
import org.springframework.web.servlet.tags.form.TagWriter;

/**
 *
 * @author aploese
 */
public class TableContainerTag extends DojoTag {

    public TableContainerTag() {
        super("div", "dojox/layout/TableContainer");
    }

    private String style;

    @Override
    protected void writeAttributes(TagWriter tagWriter) throws JspException {
        tagWriter.writeAttribute("style", style);
    }

    public void setCols(int cols) {
        putDataDojoProp("cols", cols);
    }

    /**
     * @param style the style to set
     */
    public void setStyle(String style) {
        this.style = style;
    }
}
