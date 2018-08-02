/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib;

import br.org.scadabr.l10n.AbstractLocalizer;
import br.org.scadabr.web.i18n.I18NUtils;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import java.io.IOException;
import javax.servlet.jsp.JspException;
import javax.servlet.jsp.tagext.TagSupport;
import static br.org.scadabr.web.taglib.Functions.escapeAllQuotes;
import static br.org.scadabr.web.taglib.Functions.escapeDoubleQuote;
import static br.org.scadabr.web.taglib.Functions.escapeSingleQuote;

/**
 *
 * @author aploese
 */
public class LocalizableMessageTag extends TagSupport {

    private LocalizableMessage message;
    private String key;
    private boolean escapeQuotes;
    private boolean escapeDQuotes;

    public void setMessage(LocalizableMessage message) {
        this.message = message;
    }

    public void setKey(String key) {
        this.key = key;
    }

    public void setEscapeQuotes(boolean escapeQuotes) {
        this.escapeQuotes = escapeQuotes;
    }

    public void setEscapeDQuotes(boolean escapeDQuotes) {
        this.escapeDQuotes = escapeDQuotes;
    }

    @Override
    public int doEndTag() throws JspException {
        String s = null;
        if (message != null) {
            s = AbstractLocalizer.localizeMessage(message, I18NUtils.getBundle(pageContext));
        } else if (key != null) {
            s = AbstractLocalizer.localizeI18nKey(key, I18NUtils.getBundle(pageContext));
        }
        if (s != null) {
            if (escapeQuotes && escapeDQuotes) {
                s = escapeAllQuotes(s);
            } else if (escapeQuotes) {
                s = escapeSingleQuote(s);
            } else if (escapeDQuotes){
                s = escapeDoubleQuote(s);
            }

            try {
                pageContext.getOut().write(s);
            } catch (IOException e) {
                throw new JspException(e);
            }
        }
        return EVAL_PAGE;
    }
}
