/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib;

import br.org.scadabr.l10n.AbstractLocalizer;
import br.org.scadabr.web.i18n.I18NUtils;
import br.org.scadabr.l10n.Localizer;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import javax.servlet.jsp.JspException;
import javax.servlet.jsp.JspWriter;
import static br.org.scadabr.web.taglib.Functions.printAttribute;

/**
 *
 * @author aploese
 */
public class PaginationTag extends PaginationUrlTag {

    private String delimeter = "&nbsp;";
    private boolean indices = true;
    private String previousLabelKey;
    private String nextLabelKey;
    private String styleClass;

    public void setDelimeter(String delimeter) {
        this.delimeter = delimeter;
    }

    public void setStyleClass(String styleClass) {
        this.styleClass = styleClass;
    }

    public void setIndices(boolean indices) {
        this.indices = indices;
    }

    public void setPreviousLabelKey(String previousLabelKey) {
        this.previousLabelKey = previousLabelKey;
    }

    public void setNextLabelKey(String nextLabelKey) {
        this.nextLabelKey = nextLabelKey;
    }

    @Override
    public int doStartTag() throws JspException {
        setDefaults();
        JspWriter out = pageContext.getOut();

        List<String> excludeList = new ArrayList<>();
        excludeList.add(prefix + "page");
        addExcludeParams(excludeList);

        String baseHref = getBaseHref(excludeList);
        try {
            boolean prev = paging.getPage() > 0;
            boolean next = paging.getNumberOfPages() > paging.getPage() + 1;
            if (prev) {
                out.write("<a");
                printAttribute(out, "href", baseHref + prefix + "page=" + Integer.toString(paging.getPage() - 1));

                printAttribute(out, "class", styleClass);
                out.write(">");
                out.write(AbstractLocalizer.localizeI18nKey(previousLabelKey, I18NUtils.getBundle(pageContext)));
                out.write("</a>");
            }
            if ((indices) && (paging.getNumberOfItems() > 0)) {
                if (prev) {
                    out.write(delimeter);
                }
                int start = paging.getPage() - 5;
                if (start < 0) {
                    start = 0;
                }
                int end = paging.getPage() + 5;
                if (end >= paging.getNumberOfPages()) {
                    end = paging.getNumberOfPages() - 1;
                }
                for (int i = start; i <= end; i++) {
                    if (i > start) {
                        out.write(delimeter);
                    }
                    if (i == paging.getPage()) {
                        out.write("<strong>");
                        out.write(Integer.toString(i + 1));
                        out.write("</strong>");
                    } else {
                        out.write("<a");
                        printAttribute(out, "href", baseHref + prefix + "page=" + Integer.toString(i));
                        printAttribute(out, "class", styleClass);
                        out.write(">");
                        out.write(Integer.toString(i + 1));
                        out.write("</a>");
                    }
                }
                if (next) {
                    out.write(delimeter);
                }
            } else if ((prev) && (next)) {
                out.write(delimeter);
            }
            if (next) {
                out.write("<a");
                printAttribute(out, "href", baseHref + prefix + "page=" + Integer.toString(paging.getPage() + 1));

                printAttribute(out, "class", styleClass);
                out.write(">");
                out.write(AbstractLocalizer.localizeI18nKey(nextLabelKey, I18NUtils.getBundle(pageContext)));
                out.write("</a>");
            }
            out.flush();
        } catch (IOException e) {
            throw new JspException("Error writing page info", e);
        }
        return SKIP_BODY;
    }

    @Override
    public void release() {
        super.release();
        delimeter = "&nbsp;";
        indices = true;
        styleClass = null;
    }

    private void setDefaults() {
        if (styleClass == null) {
            styleClass = pageContext.getServletContext().getInitParameter("br.org.scadabr.web.taglib.PaginationTag.styleClass");
        }
    }
}
