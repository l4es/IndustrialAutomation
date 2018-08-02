/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.i18n;

import java.util.ResourceBundle;
import javax.servlet.ServletRequest;
import javax.servlet.jsp.PageContext;
import javax.servlet.jsp.jstl.core.Config;
import javax.servlet.jsp.jstl.fmt.LocalizationContext;

/**
 *
 * @author aploese
 */
@Deprecated
public class I18NUtils {

    @Deprecated // USE RequestContextAwareTag
    public static ResourceBundle getBundle(ServletRequest request) {
        LocalizationContext lc = (LocalizationContext) Config.get(request, Config.FMT_LOCALIZATION_CONTEXT);
        if (lc != null) {
            return lc.getResourceBundle();
        }
        return null;
    }

    @Deprecated // USE RequestContextAwareTag
    public static ResourceBundle getBundle(PageContext pc) {
        LocalizationContext lc = (LocalizationContext) Config.find(pc, Config.FMT_LOCALIZATION_CONTEXT);
        if (lc != null) {
            return lc.getResourceBundle();
        }
        return null;
    }

}
