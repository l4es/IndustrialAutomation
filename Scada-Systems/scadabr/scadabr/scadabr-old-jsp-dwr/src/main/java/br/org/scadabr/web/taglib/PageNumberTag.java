/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib;

import br.org.scadabr.l10n.AbstractLocalizer;
import br.org.scadabr.web.i18n.I18NUtils;
import br.org.scadabr.l10n.Localizer;
import br.org.scadabr.web.util.PagingDataForm;
import java.io.IOException;
import javax.servlet.jsp.JspException;
import javax.servlet.jsp.JspWriter;
import javax.servlet.jsp.tagext.TagSupport;

/**
 *
 * @author aploese
 */
public class PageNumberTag extends TagSupport {

    private PagingDataForm paging;
    private boolean rows = true;
    private String pageLabelKey;
    private String ofLabelKey;
    private String rowsLabelKey;
    private String noRowsLabelKey;

    public void setRows(boolean rows) {
        this.rows = rows;
    }

    public void setPaging(PagingDataForm paging) {
        this.paging = paging;
    }

    public void setPageLabelKey(String pageLabelKey) {
        this.pageLabelKey = pageLabelKey;
    }

    public void setOfLabelKey(String ofLabelKey) {
        this.ofLabelKey = ofLabelKey;
    }

    public void setRowsLabelKey(String rowsLabelKey) {
        this.rowsLabelKey = rowsLabelKey;
    }

    public void setNoRowsLabelKey(String noRowsLabelKey) {
        this.noRowsLabelKey = noRowsLabelKey;
    }

    @Override
    public int doStartTag() throws JspException {
        try {
            final JspWriter out = pageContext.getOut();
            if (paging.getNumberOfItems() > 0) {
                out.write(AbstractLocalizer.localizeI18nKey(pageLabelKey, I18NUtils.getBundle(pageContext)));
                out.write(" ");
                out.write(Integer.toString(paging.getPage() + 1));
                out.write(" ");
                out.write(AbstractLocalizer.localizeI18nKey(ofLabelKey, I18NUtils.getBundle(pageContext)));
                out.write(" ");
                out.write(Integer.toString(paging.getNumberOfPages()));
                if (rows) {
                    out.write(" (");
                    out.write(Integer.toString(paging.getOffset() + 1));
                    out.write(" - ");
                    if (paging.getNumberOfItems() < paging.getOffset() + paging.getItemsPerPage()) {
                        out.write(Integer.toString(paging.getNumberOfItems()));
                    } else {
                        out.write(Integer.toString(paging.getOffset() + paging.getItemsPerPage()));
                    }
                    out.write(" ");
                    out.write(AbstractLocalizer.localizeI18nKey(ofLabelKey, I18NUtils.getBundle(pageContext)));
                    out.write(" ");
                    out.write(Integer.toString(paging.getNumberOfItems()));
                    out.write(" ");
                    out.write(AbstractLocalizer.localizeI18nKey(rowsLabelKey, I18NUtils.getBundle(pageContext)));
                    out.write(")");
                }
                out.flush();
            } else if (rows) {
                out.write(AbstractLocalizer.localizeI18nKey(noRowsLabelKey, I18NUtils.getBundle(pageContext)));
            }
        } catch (IOException e) {
            throw new JspException("Error writing page info", e);
        }
        return SKIP_BODY;
    }

    @Override
    public void release() {
        super.release();
        paging = null;
        rows = true;
    }
}
