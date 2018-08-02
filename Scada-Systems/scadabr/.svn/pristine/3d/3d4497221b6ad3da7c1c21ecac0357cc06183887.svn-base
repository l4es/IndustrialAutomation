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
package com.serotonin.mango.rt.maint.work;

import br.org.scadabr.dao.SystemSettingsDao;
import br.org.scadabr.rt.SchedulerPool;
import br.org.scadabr.timer.cron.SystemRunnable;
import javax.mail.internet.AddressException;
import javax.mail.internet.InternetAddress;

import com.serotonin.mango.rt.event.type.SystemEventType;
import com.serotonin.mango.web.email.MangoEmailContent;
import br.org.scadabr.web.email.EmailContent;
import br.org.scadabr.web.email.EmailSender;
import br.org.scadabr.vo.event.type.SystemEventKey;
import java.io.UnsupportedEncodingException;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

/**
 * @author Matthew Lohbihler
 *
 */
@Configurable
public class EmailWorkItem implements SystemRunnable {

    @Autowired
    private SystemSettingsDao  systemSettingsDao;
    @Autowired
    private SchedulerPool schedulerPool;
    
    /*
    @Override
    public int getPriority() {
        return WorkItem.PRIORITY_MEDIUM;
    }
*/
    public void queueEmail(String toAddr, MangoEmailContent content) throws AddressException {
        queueEmail(new String[]{toAddr}, content);
    }

    public void queueEmail(String[] toAddrs, MangoEmailContent content) throws AddressException {
        queueEmail(toAddrs, content, null);
    }

    public void queueEmail(String[] toAddrs, MangoEmailContent content, Runnable[] postSendExecution)
            throws AddressException {
        queueEmail(toAddrs, content.getSubject(), content, postSendExecution);
    }

    public void queueEmail(String[] toAddrs, String subject, EmailContent content, Runnable[] postSendExecution)
            throws AddressException {

        this.toAddresses = new InternetAddress[toAddrs.length];
        for (int i = 0; i < toAddrs.length; i++) {
            this.toAddresses[i] = new InternetAddress(toAddrs[i]);
        }

        this.subject = subject;
        this.content = content;
        this.postSendExecution = postSendExecution;

        schedulerPool.execute(this);
    }

    private InternetAddress fromAddress;
    private InternetAddress[] toAddresses;
    private String subject;
    private EmailContent content;
    private Runnable[] postSendExecution;

    @Override
    public void run() {
        try {
            final SystemSettingsDao.Email mailSetting = systemSettingsDao.getEmail();
            if (fromAddress == null) {
                String addr = mailSetting.getFromAddress();
                String pretty = mailSetting.getFromName();
                fromAddress = new InternetAddress(addr, pretty);
            }

            EmailSender emailSender = new EmailSender(mailSetting.getSmtpHost(),
                    mailSetting.getSmtpPort(),
                    mailSetting.isAuthorization(),
                    mailSetting.getSmtpUsername(),
                    mailSetting.getSmtpPassword(),
                    mailSetting.isTls());

            emailSender.send(fromAddress, toAddresses, subject, content);
        } catch (UnsupportedEncodingException e) {
            String to = "";
            for (InternetAddress addr : toAddresses) {
                if (to.length() > 0) {
                    to += ", ";
                }
                to += addr.getAddress();
            }
            new SystemEventType(SystemEventKey.EMAIL_SEND_FAILURE).fire("event.email.failure", subject, to, e.getMessage());
        } finally {
            if (postSendExecution != null) {
                for (Runnable runnable : postSendExecution) {
                    runnable.run();
                }
            }
        }
    }
}
