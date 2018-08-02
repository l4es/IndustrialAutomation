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
package com.serotonin.mango.web.dwr;

import br.org.scadabr.l10n.AbstractLocalizer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.ResourceBundle;
import java.util.Set;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.serotonin.mango.Common;
import com.serotonin.mango.db.dao.MailingListDao;
import com.serotonin.mango.rt.maint.work.EmailWorkItem;
import com.serotonin.mango.vo.mailingList.EmailRecipient;
import com.serotonin.mango.vo.mailingList.MailingList;
import com.serotonin.mango.vo.event.RecipientListEntryBean;
import com.serotonin.mango.web.email.MangoEmailContent;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.l10n.Localizer;
import javax.inject.Inject;

public class MailingListsDwr extends BaseDwr {

    private final Log log = LogFactory.getLog(MailingListsDwr.class);

    @Inject
    private MailingListDao mailingListDao;
    
    public DwrResponseI18n init() {
        DwrResponseI18n response = new DwrResponseI18n();
        response.addData("lists", mailingListDao.getMailingLists());
        response.addData("users", userDao.getUsers());
        return response;
    }

    public MailingList getMailingList(int id) {
        if (id == Common.NEW_ID) {
            MailingList ml = new MailingList();
            ml.setId(Common.NEW_ID);
            ml.setXid(mailingListDao.generateUniqueXid());
            ml.setEntries(new LinkedList<EmailRecipient>());
            return ml;
        }
        return mailingListDao.getMailingList(id);
    }

    public DwrResponseI18n saveMailingList(int id, String xid, String name, List<RecipientListEntryBean> entryBeans,
            List<Integer> inactiveIntervals) {
        DwrResponseI18n response = new DwrResponseI18n();

        // Validate the given information. If there is a problem, return an appropriate error message.
        MailingList ml = createMailingList(id, xid, name, entryBeans);
        ml.getInactiveIntervals().addAll(inactiveIntervals);

        if (xid.isEmpty()) {
            response.addContextual("xid", "validate.required");
        } else if (!mailingListDao.isXidUnique(xid, id)) {
            response.addContextual("xid", "validate.xidUsed");
        }

        ml.validate(response);

        if (response.isEmpty()) {
            // Save the mailing list
            mailingListDao.saveMailingList(ml);
            response.addData("mlId", ml.getId());
        }

        return response;
    }

    public void deleteMailingList(int mlId) {
        mailingListDao.deleteMailingList(mlId);
    }

    public DwrResponseI18n sendTestEmail(int id, String name, List<RecipientListEntryBean> entryBeans) {
        DwrResponseI18n response = new DwrResponseI18n();

        MailingList ml = createMailingList(id, null, name, entryBeans);
        mailingListDao.populateEntrySubclasses(ml.getEntries());

        Set<String> addresses = new HashSet<>();
        ml.appendAddresses(addresses, null);
        String[] toAddrs = addresses.toArray(new String[0]);

        try {
            ResourceBundle bundle = Common.getBundle();
            Map<String, Object> model = new HashMap<>();
            model.put("message", new LocalizableMessageImpl("ftl.userTestEmail", ml.getName()));
            MangoEmailContent cnt = new MangoEmailContent("ftl.testEmail", model, bundle, AbstractLocalizer.localizeI18nKey("ftl.testEmail", bundle), Common.UTF8);
            EmailWorkItem.queueEmail(toAddrs, cnt);
        } catch (Exception e) {
            response.addGeneric("mailingLists.testerror", e);
            log.warn("", e);
        }

        return response;
    }

    //
    // /
    // / Private helper methods
    // /
    //
    private MailingList createMailingList(int id, String xid, String name, List<RecipientListEntryBean> entryBeans) {
        // Convert the incoming information into more useful types.
        MailingList ml = new MailingList();
        ml.setId(id);
        ml.setXid(xid);
        ml.setName(name);

        List<EmailRecipient> entries = new ArrayList<>(entryBeans.size());
        for (RecipientListEntryBean bean : entryBeans) {
            entries.add(bean.createEmailRecipient());
        }
        ml.setEntries(entries);

        return ml;
    }

    /**
     * @return the mailingListDao
     */
    public MailingListDao getMailingListDao() {
        return mailingListDao;
    }

    /**
     * @param mailingListDao the mailingListDao to set
     */
    public void setMailingListDao(MailingListDao mailingListDao) {
        this.mailingListDao = mailingListDao;
    }

}
