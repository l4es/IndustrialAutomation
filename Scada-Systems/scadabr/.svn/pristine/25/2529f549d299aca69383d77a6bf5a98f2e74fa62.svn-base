/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.dwr;

import br.org.scadabr.l10n.AbstractLocalizer;
import br.org.scadabr.web.i18n.I18NUtils;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.l10n.Localizer;
import org.directwebremoting.WebContext;
import org.directwebremoting.WebContextFactory;
import org.directwebremoting.convert.StringConverter;
import org.directwebremoting.extend.MarshallException;
import org.directwebremoting.extend.OutboundContext;
import org.directwebremoting.extend.OutboundVariable;

/**
 *
 * @author aploese
 */
public class LocalizableMessageConverter extends StringConverter {

    @Override
    public OutboundVariable convertOutbound(Object data, OutboundContext outctx) throws MarshallException {
        WebContext webctx = WebContextFactory.get();
        LocalizableMessage lm = (LocalizableMessage) data;
        String s = AbstractLocalizer.localizeMessage(lm, I18NUtils.getBundle(webctx.getHttpServletRequest()));
        return super.convertOutbound(s, outctx);
    }

}
