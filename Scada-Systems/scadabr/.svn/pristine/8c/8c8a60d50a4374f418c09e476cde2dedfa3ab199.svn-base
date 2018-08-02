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
import javax.servlet.ServletContext;

/**
 *
 * @author aploese
 */
public class ListSortTag extends PaginationUrlTag {

    private String label;
    private String labelKey;
    private String field;
    private String styleClass;
    private String upImageSrc;
    private String upImageWidth;
    private String upImageHeight;
    private String upImageAlt;
    private String upImageAltKey;
    private String downImageSrc;
    private String downImageWidth;
    private String downImageHeight;
    private String downImageAlt;
    private String downImageAltKey;
    private String imageAlign;

    public void setField(String field) {
        this.field = field;
    }

    public void setLabel(String label) {
        this.label = label;
    }

    public void setLabelKey(String labelKey) {
        this.labelKey = labelKey;
    }

    public void setStyleClass(String styleClass) {
        this.styleClass = styleClass;
    }

    public void setDownImageAlt(String downImageAlt) {
        this.downImageAlt = downImageAlt;
    }

    public void setDownImageAltKey(String downImageAltKey) {
        this.downImageAltKey = downImageAltKey;
    }

    public void setDownImageHeight(String downImageHeight) {
        this.downImageHeight = downImageHeight;
    }

    public void setDownImageSrc(String downImageSrc) {
        this.downImageSrc = downImageSrc;
    }

    public void setDownImageWidth(String downImageWidth) {
        this.downImageWidth = downImageWidth;
    }

    public void setUpImageAlt(String upImageAlt) {
        this.upImageAlt = upImageAlt;
    }

    public void setUpImageAltKey(String upImageAltKey) {
        this.upImageAltKey = upImageAltKey;
    }

    public void setUpImageHeight(String upImageHeight) {
        this.upImageHeight = upImageHeight;
    }

    public void setUpImageSrc(String upImageSrc) {
        this.upImageSrc = upImageSrc;
    }

    public void setUpImageWidth(String upImageWidth) {
        this.upImageWidth = upImageWidth;
    }

    public void setImageAlign(String imageAlign) {
        this.imageAlign = imageAlign;
    }

    @Override
    public int doStartTag() throws JspException {
        setDefaults();
        JspWriter out = pageContext.getOut();
        List<String> excludeList = new ArrayList<>();
        excludeList.add(prefix + "sortField");
        excludeList.add(prefix + "sortDesc");
        addExcludeParams(excludeList);
        try {
            out.write("<a href=\"");
            out.write(getBaseHref(excludeList));
            out.write(prefix);
            out.write("sortField=");
            out.write(field);
            out.write("&amp;");
            out.write(prefix);
            out.write("sortDesc=");
            if ((field != null) && (field.equals(paging.getSortField()))) {
                out.write(Boolean.toString(!paging.getSortDesc()));
            } else {
                out.write(Boolean.toString(false));
            }
            out.write("\"");

            printAttribute(out, "class", styleClass);

            out.write(">");
            if (labelKey.isEmpty()) {
                out.write(label);
            } else {
                out.write(AbstractLocalizer.localizeI18nKey(labelKey, I18NUtils.getBundle(pageContext)));
            }
            out.write("</a>&nbsp;");
            if ((field != null) && (field.equals(paging.getSortField()))) {
                out.write("<img");
                if (paging.getSortDesc()) {
                    printAttribute(out, "src", downImageSrc);
                    printAttribute(out, "width", downImageWidth);
                    printAttribute(out, "height", downImageHeight);
                    if (downImageAltKey != null) {
                        printAttribute(out, "alt", AbstractLocalizer.localizeI18nKey(downImageAltKey, I18NUtils.getBundle(pageContext)));
                    } else {
                        printAttribute(out, "alt", downImageAlt);
                    }
                } else {
                    printAttribute(out, "src", upImageSrc);
                    printAttribute(out, "width", upImageWidth);
                    printAttribute(out, "height", upImageHeight);
                    if (upImageAltKey != null) {
                        printAttribute(out, "alt", AbstractLocalizer.localizeI18nKey(upImageAltKey, I18NUtils.getBundle(pageContext)));
                    } else {
                        printAttribute(out, "alt", upImageAlt);
                    }
                }
                printAttribute(out, "align", imageAlign);
                printAttribute(out, "border", "0");
                out.write("/>");
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
        label = null;
        labelKey = null;
        field = null;
        styleClass = null;
        upImageSrc = null;
        upImageWidth = null;
        upImageHeight = null;
        upImageAlt = null;
        downImageSrc = null;
        downImageWidth = null;
        downImageHeight = null;
        downImageAlt = null;
        imageAlign = null;
    }

    private void setDefaults() {
        ServletContext ctx = pageContext.getServletContext();
        if (styleClass == null) {
            styleClass = ctx.getInitParameter("br.org.scadabr.web.taglib.ListSortTag.styleClass");
        }
        if (upImageSrc == null) {
            upImageSrc = ctx.getInitParameter("br.org.scadabr.web.taglib.ListSortTag.upImage.src");
        }
        if (upImageWidth == null) {
            upImageWidth = ctx.getInitParameter("br.org.scadabr.web.taglib.ListSortTag.upImage.width");
        }
        if (upImageHeight == null) {
            upImageHeight = ctx.getInitParameter("br.org.scadabr.web.taglib.ListSortTag.upImage.height");
        }
        if (upImageAlt == null) {
            upImageAlt = ctx.getInitParameter("br.org.scadabr.web.taglib.ListSortTag.upImage.alt");
        }
        if (upImageAltKey == null) {
            upImageAltKey = ctx.getInitParameter("br.org.scadabr.web.taglib.ListSortTag.upImage.altKey");
        }
        if (downImageSrc == null) {
            downImageSrc = ctx.getInitParameter("br.org.scadabr.web.taglib.ListSortTag.downImage.src");
        }
        if (downImageWidth == null) {
            downImageWidth = ctx.getInitParameter("br.org.scadabr.web.taglib.ListSortTag.downImage.width");
        }
        if (downImageHeight == null) {
            downImageHeight = ctx.getInitParameter("br.org.scadabr.web.taglib.ListSortTag.downImage.height");
        }
        if (downImageAlt == null) {
            downImageAlt = ctx.getInitParameter("br.org.scadabr.web.taglib.ListSortTag.downImage.alt");
        }
        if (downImageAltKey == null) {
            downImageAltKey = ctx.getInitParameter("br.org.scadabr.web.taglib.ListSortTag.downImage.altKey");
        }
        if (imageAlign == null) {
            imageAlign = ctx.getInitParameter("br.org.scadabr.web.taglib.ListSortTag.imageAlign");
        }
    }
}
