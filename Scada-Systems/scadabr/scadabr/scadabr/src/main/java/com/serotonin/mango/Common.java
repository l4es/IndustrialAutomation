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
package com.serotonin.mango;

import gnu.io.CommPortIdentifier;

import java.io.File;
import java.nio.charset.Charset;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.text.MessageFormat;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.LinkedList;
import java.util.List;
import java.util.Locale;
import java.util.ResourceBundle;


import org.apache.commons.codec.binary.Base64;
import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.UsernamePasswordCredentials;
import org.apache.commons.httpclient.auth.AuthScope;
import org.apache.commons.httpclient.params.HttpClientParams;
import org.apache.commons.httpclient.params.HttpConnectionManagerParams;

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.dao.SystemSettingsDao;
import br.org.scadabr.db.KeyValuePair;
import com.serotonin.mango.util.BackgroundContext;
import com.serotonin.mango.util.CommPortConfigException;
import com.serotonin.mango.vo.CommPortProxy;
import com.serotonin.mango.web.ContextWrapper;
import br.org.scadabr.utils.ImplementMeException;
import java.util.MissingResourceException;
import javax.inject.Inject;
import javax.inject.Named;

@Deprecated // Convert to singleton bean
@Named
public class Common {

    @Inject
    private SystemSettingsDao systemSettingsDao;

    @Deprecated
    private static final String SESSION_USER = "sessionUser";
    private static final String ANON_VIEW_KEY = "anonymousViews";
    private static final String CUSTOM_VIEW_KEY = "customView";

    @Deprecated
    public static ContextWrapper ctx;

    //TODO inject this
    private static final ResourceBundle env = ResourceBundle.getBundle("env");

    /*
     * Updating the Mango version: - Create a DBUpdate subclass for the old
     * version number. This may not do anything in particular to the schema, but
     * is still required to update the system settings so that the database has
     * the correct version.
     */
    @Deprecated // Use ScadaBrVersion bean
    public static final String getVersion() {
        return "0.9.1";
    }

    @Deprecated
    public interface ContextKeys {

        @Deprecated
        String IMAGE_SETS = "IMAGE_SETS";
        @Deprecated
        String DYNAMIC_IMAGES = "DYNAMIC_IMAGES";
        @Deprecated
        String SCHEDULER = "SCHEDULER";
        String FREEMARKER_CONFIG = "FREEMARKER_CONFIG";
        String HTTP_RECEIVER_MULTICASTER = "HTTP_RECEIVER_MULTICASTER";
        String DOCUMENTATION_MANIFEST = "DOCUMENTATION_MANIFEST";
        String DATA_POINTS_NAME_ID_MAPPING = "DATAPOINTS_NAME_ID_MAPPING";
    }

    public interface GroveServlets {

        String VERSION_CHECK = "versionCheckComm";
        String MANGO_LOG = "mangoLog";
    }

    //
    // Background process description. Used for audit logs when the system
    // automatically makes changes to data, such as
    // safe mode disabling stuff.
    public static String getBackgroundProcessDescription() {
        BackgroundContext backgroundContext = BackgroundContext.get();
        if (backgroundContext == null) {
            return null;
        }
        return backgroundContext.getProcessDescriptionKey();
    }

    //
    // Environment profile
    public static ResourceBundle getEnvironmentProfile() {
        return env;
    }

    public static boolean getEnvironmentBoolean(String key, boolean defaultValue) {
        try {
            return Boolean.parseBoolean(env.getString(key));
        } catch (MissingResourceException e) {
            return defaultValue;
        }
    }

    public static int getEnvironmentInt(String key, int defaultValue) {
        try {
            return Integer.parseInt(env.getString(key));
        } catch (MissingResourceException | NumberFormatException e) {
            return defaultValue;
        }
    }

    public static String getEnvironmentString(String key, String defaultValue) {
        try {
            return env.getString(key);
        } catch (MissingResourceException e) {
            return defaultValue;
        }
    }

    public static String getGroveUrl(String servlet) {
        final String grove = getEnvironmentString("grove.url",
                "http://mango.serotoninsoftware.com/servlet");
        return grove + "/" + servlet;
    }

    public static String getDocPath() {
        return ctx.getServletContext().getRealPath("/WEB-INF/dox") + "/";
    }

    private static String lazyFiledataPath = null;

    public String getFiledataPath() {
        if (lazyFiledataPath == null) {
            String name = systemSettingsDao.getFileDataPath();
            if (name.startsWith("~")) {
                name = ctx.getServletContext().getRealPath(name.substring(1));
            }

            File file = new File(name);
            if (!file.exists()) {
                file.mkdirs();
            }

            lazyFiledataPath = name;
        }
        return lazyFiledataPath;
    }

    //
    // Misc
    @Deprecated
    public static List<CommPortProxy> getCommPorts()
            throws CommPortConfigException {
        try {
            List<CommPortProxy> ports = new LinkedList<>();
            Enumeration<?> portEnum = CommPortIdentifier.getPortIdentifiers();
            CommPortIdentifier cpid;
            while (portEnum.hasMoreElements()) {
                cpid = (CommPortIdentifier) portEnum.nextElement();
                if (cpid.getPortType() == CommPortIdentifier.PORT_SERIAL) {
                    ports.add(new CommPortProxy(cpid));
                }
            }
            return ports;
        } catch (UnsatisfiedLinkError e) {
            throw new CommPortConfigException(e.getMessage());
        } catch (NoClassDefFoundError e) {
            throw new CommPortConfigException(
                    "Comm configuration error. Check that rxtx DLL or libraries have been correctly installed.");
        }
    }

    //
    // HttpClient
    public HttpClient getHttpClient() {
        return getHttpClient(30000); // 30 seconds.
    }

    public HttpClient getHttpClient(int timeout) {
        HttpConnectionManagerParams managerParams = new HttpConnectionManagerParams();
        managerParams.setConnectionTimeout(timeout);
        managerParams.setSoTimeout(timeout);

        HttpClientParams params = new HttpClientParams();
        params.setSoTimeout(timeout);

        HttpClient client = new HttpClient();
        client.getHttpConnectionManager().setParams(managerParams);
        client.setParams(params);

        if (systemSettingsDao.getHttpClientProxy().isEnabled()) {
            String proxyHost = systemSettingsDao.getHttpClientProxy().getServer();
            int proxyPort = systemSettingsDao.getHttpClientProxy().getPort();

            // Set up the proxy configuration.
            client.getHostConfiguration().setProxy(proxyHost, proxyPort);

            // Set up the proxy credentials. All realms and hosts.
            client.getState()
                    .setProxyCredentials(
                            AuthScope.ANY,
                            new UsernamePasswordCredentials(
                                    systemSettingsDao.getHttpClientProxy().getUsername(),
                                    systemSettingsDao.getHttpClientProxy().getPassword()));
        }

        return client;
    }

    @Deprecated
    public static String getMessage(String key) {
        throw new ImplementMeException();
// ensureI18n();
//        return AbstractLocalizer.localizeI18nKey(key, systemBundle);
    }

    @Deprecated // Use per user settings ...
    public static ResourceBundle getBundle() {
        throw new ImplementMeException();
        //    ensureI18n();
//        return systemBundle;
    }

    @Deprecated
    public static String getMessage(String key, Object... args) {
        String pattern = getMessage(key);
        return MessageFormat.format(pattern, args);
    }

    @Deprecated
    private static Locale findLocale(String language) {
        for (Locale locale : Locale.getAvailableLocales()) {
            if (locale.getLanguage().equals(language)) {
                return locale;
            }
        }
        return null;
    }

    public static List<KeyValuePair> getLanguages() {
        List<KeyValuePair> languages = new ArrayList<>();
        ResourceBundle i18n = ResourceBundle.getBundle("i18n");
        for (String key : i18n.keySet()) {
            languages.add(new KeyValuePair(key, i18n.getString(key)));
        }
        return languages;
    }

}
