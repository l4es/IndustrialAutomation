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

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.Reader;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.ResourceBundle;

import javax.servlet.http.HttpServletRequest;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.directwebremoting.WebContext;
import org.directwebremoting.WebContextFactory;
import org.springframework.beans.propertyeditors.LocaleEditor;
import org.springframework.web.servlet.LocaleResolver;
import org.springframework.web.servlet.i18n.SessionLocaleResolver;

import br.org.scadabr.io.StreamUtils;
import br.org.scadabr.l10n.AbstractLocalizer;
import com.serotonin.mango.Common;
import com.serotonin.mango.db.dao.MailingListDao;
import com.serotonin.mango.rt.EventManager;
import com.serotonin.mango.rt.maint.work.EmailWorkItem;
import com.serotonin.mango.util.DocumentationItem;
import com.serotonin.mango.util.DocumentationManifest;
import com.serotonin.mango.vo.User;
import com.serotonin.mango.vo.event.RecipientListEntryBean;
import com.serotonin.mango.web.email.MangoEmailContent;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.web.dwr.MethodFilter;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import javax.inject.Inject;

public class MiscDwr extends BaseDwr {

    public static final Log LOG = LogFactory.getLog(MiscDwr.class);
    private static final String LONG_POLL_DATA_KEY = "LONG_POLL_DATA";
    private static final String LONG_POLL_DATA_TIMEOUT_KEY = "LONG_POLL_DATA_TIMEOUT";

    @Inject
    protected WatchListDwr watchListDwr;
    @Inject
    protected DataPointDetailsDwr dataPointDetailsDwr;
    @Inject
    protected CustomViewDwr customViewDwr;
    @Inject
    private MailingListDao mailingListDao;
    @Inject
    private EventManager eventManager;

    public boolean toggleUserMuted() {
        User user = Common.getUser();
        if (user != null) {
            user.setMuted(!user.isMuted());
            return user.isMuted();
        }
        return false;
    }

    @Deprecated //APL use static content???
    public Map<String, Object> getDocumentationItem(String documentId) {
        Map<String, Object> result = new HashMap<>();

        DocumentationManifest manifest = Common.ctx.getDocumentationManifest();
        DocumentationItem item = manifest.getItem(documentId);
        if (item == null) {
            result.put("error", getMessage("dox.notFound"));
        } else {
            // Read the content.
            String filename = Common.getDocPath() + "/" + getMessage("dox.dir") + "/" + documentId + ".htm";
            try {
                StringWriter out;
                try (Reader in = new FileReader(filename)) {
                    out = new StringWriter();
                    StreamUtils.transfer(in, out);
                }

                addDocumentationItem(result, item);
                result.put("content", out.toString());

                List<Map<String, Object>> related = new ArrayList<>();
                for (String relatedId : item.getRelated()) {
                    Map<String, Object> map = new HashMap<>();
                    related.add(map);
                    addDocumentationItem(map, manifest.getItem(relatedId));
                }

                result.put("relatedList", related);
            } catch (FileNotFoundException e) {
                result.put("error", getMessage("dox.notFound") + " " + filename);
            } catch (IOException e) {
                result.put("error", getMessage("dox.readError") + " " + e.getClass().getName() + ": " + e.getMessage());
            }
        }

        return result;
    }

    @Deprecated //APL old dox help stuff
    private void addDocumentationItem(Map<String, Object> map, DocumentationItem di) {
        map.put("id", di.getId());
        map.put("title", getMessage("dox." + di.getId()));
    }

    
    public void jsError(String desc, String page, String line, String browserName, String browserVersion,
            String osName, String location) {
        LOG.warn("Javascript error\r\n" + "   Description: " + desc + "\r\n" + "   Page: " + page + "\r\n"
                + "   Line: " + line + "\r\n" + "   Browser name: " + browserName + "\r\n" + "   Browser version: "
                + browserVersion + "\r\n" + "   osName: " + osName + "\r\n" + "   location: " + location);
    }

    @MethodFilter
    public DwrResponseI18n sendTestEmail(List<RecipientListEntryBean> recipientList, String prefix, String message) {
        DwrResponseI18n response = new DwrResponseI18n();

        String[] toAddrs = mailingListDao.getRecipientAddresses(recipientList, null).toArray(new String[0]);
        if (toAddrs.length == 0) {
            response.addGeneric("js.email.noRecipForEmail");
        } else {
            try {
                ResourceBundle bundle = Common.getBundle();
                Map<String, Object> model = new HashMap<>();
                model.put("user", Common.getUser());
                model.put("message", new LocalizableMessageImpl("common.default", message));
                MangoEmailContent cnt = new MangoEmailContent("testEmail", model, bundle, AbstractLocalizer.localizeI18nKey("ftl.testEmail", bundle), Common.UTF8);
                EmailWorkItem.queueEmail(toAddrs, cnt);
            } catch (Exception e) {
                response.addGeneric("common.default", e);
            }
        }

        response.addData("prefix", prefix);

        return response;
    }

    public void setLocale(String locale) {
        WebContext webContext = WebContextFactory.get();

        LocaleResolver localeResolver = new SessionLocaleResolver();

        LocaleEditor localeEditor = new LocaleEditor();
        localeEditor.setAsText(locale);

        localeResolver.setLocale(webContext.getHttpServletRequest(), webContext.getHttpServletResponse(),
                (Locale) localeEditor.getValue());
    }

    @Deprecated
    @MethodFilter
    public void setHomeUrl(String url) {
        // Remove the scheme, domain, and context if there.
        HttpServletRequest request = WebContextFactory.get().getHttpServletRequest();

        // Remove the scheme.
        url = url.substring(request.getScheme().length() + 3);

        // Remove the domain.
        url = url.substring(request.getServerName().length());

        // Remove the port
        if (url.charAt(0) == ':') {
            url = url.substring(Integer.toString(request.getServerPort()).length() + 1);
        }

        // Remove the context
        url = url.substring(request.getContextPath().length());

        // Remove any leading /
        if (url.charAt(0) == '/') {
            url = url.substring(1);
        }

        // Save the result
        userDao.saveHomeUrl(Common.getUser().getId(), url);
    }

    @Deprecated
    @MethodFilter
    public String getHomeUrl() {
        String url = Common.getUser().getHomeUrl();
        if (url.isEmpty()) {
            url = "watch_list.shtm";
        }
        return url;
    }

    /**
     * @return the dataPointDetailsDwr
     */
    public DataPointDetailsDwr getDataPointDetailsDwr() {
        return dataPointDetailsDwr;
    }

    /**
     * @param dataPointDetailsDwr the dataPointDetailsDwr to set
     */
    public void setDataPointDetailsDwr(DataPointDetailsDwr dataPointDetailsDwr) {
        this.dataPointDetailsDwr = dataPointDetailsDwr;
    }

    /**
     * @return the customViewDwr
     */
    public CustomViewDwr getCustomViewDwr() {
        return customViewDwr;
    }

    /**
     * @param customViewDwr the customViewDwr to set
     */
    public void setCustomViewDwr(CustomViewDwr customViewDwr) {
        this.customViewDwr = customViewDwr;
    }

    /**
     * @return the watchListDwr
     */
    public WatchListDwr getWatchListDwr() {
        return watchListDwr;
    }

    /**
     * @param watchListDwr the watchListDwr to set
     */
    public void setWatchListDwr(WatchListDwr watchListDwr) {
        this.watchListDwr = watchListDwr;
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

    /**
     * @return the eventManager
     */
    public EventManager getEventManager() {
        return eventManager;
    }

    /**
     * @param eventManager the eventManager to set
     */
    public void setEventManager(EventManager eventManager) {
        this.eventManager = eventManager;
    }

}
