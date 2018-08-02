/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.taglib;

import br.org.scadabr.l10n.AbstractLocalizer;
import br.org.scadabr.l10n.Localizer;
import java.io.IOException;
import java.text.DateFormat;
import java.util.Date;
import java.util.Locale;
import javax.servlet.jsp.JspException;
import javax.servlet.jsp.jstl.core.Config;
import javax.servlet.jsp.jstl.fmt.LocalizationContext;
import javax.servlet.jsp.tagext.TagSupport;

/**
 *
 * @author aploese
 */
public class LocalizableTimeStampTag extends TagSupport {

    private long timestamp;
    private boolean hideDateOfToday;

    //TODO get proper date from ??? Usersettings???? - so currently all that is older 24 hours from now 
    @Override
    public int doEndTag() throws JspException {
        Locale l = ((LocalizationContext) Config.find(pageContext, Config.FMT_LOCALIZATION_CONTEXT)).getLocale();
        try {
            pageContext.getOut().write(AbstractLocalizer.localizeTimeStamp(timestamp, hideDateOfToday, l));
        } catch (IOException e) {
            throw new JspException(e);
        }
        return EVAL_PAGE;
    }

    /**
     * @param timestamp the timestamp to set
     */
    public void setTimestamp(long timestamp) {
        this.timestamp = timestamp;
    }

    /**
     * @param hideDateOfToday the hideDateOfToday to set
     */
    public void setHideDateOfToday(boolean hideDateOfToday) {
        this.hideDateOfToday = hideDateOfToday;
    }

}
