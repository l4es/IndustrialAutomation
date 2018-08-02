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
package com.serotonin.mango.rt.event.handlers;

import br.org.scadabr.dao.MailingListDao;
import br.org.scadabr.dao.SystemSettingsDao;
import br.org.scadabr.l10n.AbstractLocalizer;
import br.org.scadabr.rt.SchedulerPool;
import br.org.scadabr.timer.cron.CronExpression;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.event.type.SystemEventKey;
import br.org.scadabr.web.email.EmailInline;
import com.serotonin.mango.Common;
import com.serotonin.mango.rt.event.EventInstance;
import com.serotonin.mango.rt.maint.work.EmailWorkItem;
import com.serotonin.mango.util.timeout.EventRunWithArgTask;
import com.serotonin.mango.util.timeout.RunWithArgClient;
import com.serotonin.mango.vo.event.EventHandlerVO;
import com.serotonin.mango.web.email.MangoEmailContent;
import com.serotonin.mango.web.email.UsedImagesDirective;
import freemarker.template.TemplateException;
import java.io.File;
import java.io.IOException;
import java.util.GregorianCalendar;
import java.util.HashMap;
import java.util.Map;
import java.util.ResourceBundle;
import java.util.Set;
import javax.mail.internet.AddressException;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.joda.time.DateTime;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

@Configurable
public class EmailHandlerRT extends EventHandlerRT implements RunWithArgClient<EventInstance> {

    @Autowired
    private MailingListDao mailingListDao;
    @Autowired
    private SystemSettingsDao systemSettingsDao;
    @Autowired
    private SchedulerPool schedulerPool;

    private static final Log LOG = LogFactory.getLog(EmailHandlerRT.class);

    private EventRunWithArgTask<EventInstance> escalationTask;

    private Set<String> activeRecipients;

    private enum NotificationType {

        ACTIVE("active", "ftl.subject.active"), //
        ESCALATION("escalation", "ftl.subject.escalation"), //
        INACTIVE("inactive", "ftl.subject.inactive");

        String file;
        String key;

        private NotificationType(String file, String key) {
            this.file = file;
            this.key = key;
        }

        public String getFile() {
            return file;
        }

        public String getKey() {
            return key;
        }
    }

    /**
     * The list of all of the recipients - active and escalation - for sending
     * upon inactive if configured to do so.
     */
    private Set<String> inactiveRecipients;

    public EmailHandlerRT(EventHandlerVO vo) {
        this.vo = vo;
    }

    public Set<String> getActiveRecipients() {
        return activeRecipients;
    }

    @Override
    public void eventRaised(EventInstance evt) {
        // Get the email addresses to send to
        activeRecipients = mailingListDao.getRecipientAddresses(vo.getActiveRecipients(),
                new DateTime(evt.getFireTimestamp()));

        // Send an email to the active recipients.
        sendEmail(evt, NotificationType.ACTIVE, activeRecipients);

        // If an inactive notification is to be sent, save the active recipients.
        if (vo.isSendInactive()) {
            if (vo.isInactiveOverride()) {
                inactiveRecipients = mailingListDao.getRecipientAddresses(vo.getInactiveRecipients(),
                        new DateTime(evt.getFireTimestamp()));
            } else {
                inactiveRecipients = activeRecipients;
            }
        }

        // If an escalation is to be sent, set up timeout to trigger it.
        if (vo.isSendEscalation()) {
            GregorianCalendar c = new GregorianCalendar();
            c.setTimeInMillis(vo.getEscalationDelayType().getMillis(vo.getEscalationDelay()));
            escalationTask = new EventRunWithArgTask<>(new CronExpression(c), this, evt);
            schedulerPool.schedule(escalationTask);
        }
    }

    //
    // TimeoutClient
    //
    @Override
    synchronized public void run(EventInstance evt, long fireTime) {
        // Get the email addresses to send to
        Set<String> addresses = mailingListDao.getRecipientAddresses(vo.getEscalationRecipients(), new DateTime(
                fireTime));

        // Send the escalation.
        sendEmail(evt, NotificationType.ESCALATION, addresses);

        // If an inactive notification is to be sent, save the escalation recipients, but only if inactive recipients
        // have not been overridden.
        if (vo.isSendInactive() && !vo.isInactiveOverride()) {
            inactiveRecipients.addAll(addresses);
        }
    }

    @Override
    synchronized public void eventInactive(EventInstance evt) {
        // Cancel the escalation job in case it's there
        if (escalationTask != null) {
            escalationTask.cancel();
        }

        if (inactiveRecipients != null && inactiveRecipients.size() > 0) // Send an email to the inactive recipients.
        {
            sendEmail(evt, NotificationType.INACTIVE, inactiveRecipients);
        }
    }

    public void sendActiveEmail(EventInstance evt, Set<String> addresses) {
        sendEmail(evt, NotificationType.ACTIVE, addresses, null);
    }

    private void sendEmail(EventInstance evt, NotificationType notificationType, Set<String> addresses) {
        sendEmail(evt, notificationType, addresses, vo.getAlias());
    }

    private void sendEmail(EventInstance evt, NotificationType notificationType, Set<String> addresses,
            String alias) {
        if (evt.getEventType().isSystemMessage()) {
            if (evt.getEventType().getEventKey()== SystemEventKey.EMAIL_SEND_FAILURE) {
                // Don't send email notifications about email send failures.
                LOG.info("Not sending email for event raised due to email failure");
                return;
            }
        }

        ResourceBundle bundle = Common.getBundle();

        // Determine the subject to use.
        LocalizableMessage subjectMsg;
        LocalizableMessage notifTypeMsg = new LocalizableMessageImpl(notificationType.getKey());
        if (alias == null) {
            if (evt.isNew()) {
                subjectMsg = new LocalizableMessageImpl("ftl.subject.default", notifTypeMsg);
            } else {
                subjectMsg = new LocalizableMessageImpl("ftl.subject.default.id", notifTypeMsg, evt.getId());
            }
        } else {
            if (evt.isNew()) {
                subjectMsg = new LocalizableMessageImpl("ftl.subject.alias", alias, notifTypeMsg);
            } else {
                subjectMsg = new LocalizableMessageImpl("ftl.subject.alias.id", alias, notifTypeMsg, evt.getId());
            }
        }

        String subject = AbstractLocalizer.localizeMessage(subjectMsg, bundle);

        try {
            String[] toAddrs = addresses.toArray(new String[0]);
            UsedImagesDirective inlineImages = new UsedImagesDirective();

            // Send the email.
            Map<String, Object> model = new HashMap<>();
            model.put("evt", evt);
            if (evt.getContext() != null) {
                model.putAll(evt.getContext());
            }
            model.put("img", inlineImages);
            model.put("instanceDescription", systemSettingsDao.getInstanceDescription());
            MangoEmailContent content = new MangoEmailContent(notificationType.getFile(), model, bundle, subject);

            for (String s : inlineImages.getImageList()) {
                content.addInline(new EmailInline.FileInline(s, new File(Common.ctx.getServletContext().getRealPath(s))));
            }

            new EmailWorkItem().queueEmail(toAddrs, content);
        } catch (TemplateException | IOException | AddressException | RuntimeException e) {
            LOG.error("", e);
        }
    }
}
