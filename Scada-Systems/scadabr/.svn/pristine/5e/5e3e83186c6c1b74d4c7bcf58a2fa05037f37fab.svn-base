/*
 Mango - Open Source M2M - http://mango.serotoninsoftware.com
 Copyright (C) 2006-2011 Serotonin Software Technologies Inc.
 @author Matthew Lohbihler
    
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
package com.serotonin.mango.web.email;

import br.org.scadabr.l10n.AbstractLocalizer;
import java.io.IOException;
import java.util.Map;
import java.util.ResourceBundle;

import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.l10n.Localizer;

import freemarker.core.Environment;
import freemarker.ext.beans.BeanModel;
import freemarker.template.TemplateDirectiveBody;
import freemarker.template.TemplateDirectiveModel;
import freemarker.template.TemplateException;
import freemarker.template.TemplateModel;
import freemarker.template.TemplateModelException;
import freemarker.template.TemplateNumberModel;
import freemarker.template.TemplateScalarModel;
import java.util.Locale;

/**
 * @author Matthew Lohbihler
 */
public class MessageFormatDirective implements TemplateDirectiveModel {

    private final ResourceBundle bundle;

    public MessageFormatDirective(ResourceBundle bundle) {
        this.bundle = bundle;
    }

    @Override
    public void execute(Environment env, Map params, TemplateModel[] loopVars,
            TemplateDirectiveBody body) throws TemplateException, IOException {

        if (params.containsKey("key")) {
            try {
                final TemplateScalarModel key = (TemplateScalarModel) params.get("key");
                env.getOut().write(AbstractLocalizer.localizeI18nKey(((TemplateScalarModel) key).getAsString(), bundle));
                return;
            } catch (ClassCastException e) {
                throw new TemplateModelException("key must be a string");
            }
        }

        if (params.containsKey("message")) {
            final BeanModel model = (BeanModel) params.get("message");
            if (model == null) {
                // The parameter is there, but the value is null.
                return;
            }
            LocalizableMessage message = (LocalizableMessage) model.getWrappedObject();
            if (message == null) {
                return;
            } else {
                env.getOut().write(AbstractLocalizer.localizeMessage(message, bundle));
                return;
            }
        }

        if (params.containsKey("timestamp")) {
            try {
                final TemplateNumberModel timestamp = (TemplateNumberModel) params.get("timestamp");
                if (timestamp == null) {
                    // The parameter is there, but the value is null.
                    return;
                }
                env.getOut().write(AbstractLocalizer.localizeTimeStamp(timestamp.getAsNumber().longValue(), false, bundle.getLocale()));
                return;
            } catch (ClassCastException e) {
                throw new TemplateModelException("timestamp must be a long");

            }
        }
        
        if (params.containsKey("duration")) {
            try {
                final TemplateNumberModel duration = (TemplateNumberModel) params.get("duration");
                if (duration == null) {
                    // The parameter is there, but the value is null.
                    return;
                }
                env.getOut().write(Long.toString(duration.getAsNumber().longValue() / 1000));
                env.getOut().write(" s ");
                env.getOut().write(Long.toString(duration.getAsNumber().longValue() % 1000));
                env.getOut().write(" ms");
                return;
            } catch (ClassCastException e) {
                throw new TemplateModelException("timestamp must be a long");

            }
        }
        

        // The parameter wasn't given
        throw new TemplateModelException("One of key or message must be provided");

    }
}
