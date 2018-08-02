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
package br.org.scadabr.dao.jdbc;

import java.awt.Color;
import java.util.HashMap;
import java.util.Map;

import org.springframework.transaction.TransactionStatus;
import org.springframework.transaction.support.TransactionCallbackWithoutResult;

import br.org.scadabr.InvalidArgumentException;
import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.dao.SystemSettingsDao;
import br.org.scadabr.util.ColorUtils;
import br.org.scadabr.utils.TimePeriods;
import br.org.scadabr.vo.event.AlarmLevel;
import br.org.scadabr.vo.event.type.AuditEventKey;
import br.org.scadabr.vo.event.type.SystemEventKey;
import com.serotonin.mango.web.email.MangoEmailContent;
import java.net.InetAddress;
import java.net.UnknownHostException;
import javax.inject.Named;
import org.joda.time.DateTime;
import org.springframework.dao.EmptyResultDataAccessException;
import org.springframework.jdbc.core.JdbcTemplate;

@Named
public class SystemSettingsDaoImpl extends BaseDao implements SystemSettingsDao {

    // Email settings
    private static final String EMAIL_SMTP_HOST = "emailSmtpHost";
    private static final String EMAIL_SMTP_PORT = "emailSmtpPort";
    private static final String EMAIL_FROM_ADDRESS = "emailFromAddress";
    private static final String EMAIL_FROM_NAME = "emailFromName";
    private static final String EMAIL_AUTHORIZATION = "emailAuthorization";
    private static final String EMAIL_SMTP_USERNAME = "emailSmtpUsername";
    private static final String EMAIL_SMTP_PASSWORD = "emailSmtpPassword";
    private static final String EMAIL_TLS = "emailTls";
    private static final String EMAIL_CONTENT_TYPE = "emailContentType";

    // Colours
    private static final String CHART_BACKGROUND_COLOUR = "chartBackgroundColour";
    private static final String PLOT_BACKGROUND_COLOUR = "plotBackgroundColour";
    private static final String PLOT_GRIDLINE_COLOUR = "plotGridlineColour";

    // New Mango version
    private static final String NEW_VERSION_NOTIFICATION_LEVEL = "newVersionNotificationLevel";
    private static final String NOTIFICATION_LEVEL_STABLE = "S";
    private static final String NOTIFICATION_LEVEL_RC = "C";
    private static final String NOTIFICATION_LEVEL_BETA = "B";

    // Database schema version
    private static final String DATABASE_SCHEMA_VERSION = "databaseSchemaVersion";

    // Servlet context name
    private static final String SERVLET_CONTEXT_PATH = "servletContextPath";

    // Event purging
    private static final String EVENT_PURGE_PERIOD_TYPE = "eventPurgePeriodType";
    private static final String EVENT_PURGE_PERIODS = "eventPurgePeriods";

    // Report purging
    private static final String REPORT_PURGE_PERIOD_TYPE = "reportPurgePeriodType";
    private static final String REPORT_PURGE_PERIODS = "reportPurgePeriods";

    // HTTP Client configuration
    private static final String HTTP_CLIENT_USE_PROXY = "httpClientUseProxy";
    private static final String HTTP_CLIENT_PROXY_SERVER = "httpClientProxyServer";
    private static final String HTTP_CLIENT_PROXY_PORT = "httpClientProxyPort";
    private static final String HTTP_CLIENT_PROXY_USERNAME = "httpClientProxyUsername";
    private static final String HTTP_CLIENT_PROXY_PASSWORD = "httpClientProxyPassword";

    // i18n
    private static final String LANGUAGE = "language";

    // Customization
    private static final String FILEDATA_PATH = "filedata.path";
    private static final String DATASOURCE_DISPLAY_SUFFIX = ".display";
    private static final String HTTPDS_PROLOGUE = "httpdsPrologue";
    private static final String HTTPDS_EPILOGUE = "httpdsEpilogue";
    private static final String UI_PERFORAMANCE = "uiPerformance";
    private static final String GROVE_LOGGING = "groveLogging";
    private static final String FUTURE_DATE_LIMIT_PERIODS = "futureDateLimitPeriods";
    private static final String FUTURE_DATE_LIMIT_PERIOD_TYPE = "futureDateLimitPeriodType";
    private static final String INSTANCE_DESCRIPTION = "instanceDescription";

    //
    // /
    // / Static stuff
    // /
    //
    private static final String SYSTEM_EVENT_ALARMLEVEL_PREFIX = "systemEventAlarmLevel";
    private static final String AUDIT_EVENT_ALARMLEVEL_PREFIX = "auditEventAlarmLevel";

    public static void setSetSchemaVersion(JdbcTemplate ejt, String version) {
        setValue(ejt, DATABASE_SCHEMA_VERSION, version);
    }

    // Value cache 
    //TODO cache real objects???
    private final Map<String, String> cache = new HashMap<>();
    private final ReportsImpl reportsImpl = new ReportsImpl();
    private final EventsImpl eventsImpl = new EventsImpl();
    private final HttpClientProxyImpl httpClientProxyImpl = new HttpClientProxyImpl();
    private final EmailImpl emailImpl = new EmailImpl();
    

//    @PostConstruct // todo if getInstance is removed and no static access use this ...
    @Override
    public void init() {
        super.init();
        for (SystemEventKey s : SystemEventKey.values()) {
            final AlarmLevel l = getAlarmLevel(s);
            if (l != null) {
                // override if avail
                s.setAlarmLevel(l);
            }
        }
        for (AuditEventKey a : AuditEventKey.values()) {
            final AlarmLevel l = getAlarmLevel(a);
            if (l != null) {
                // override if avail
                a.setAlarmLevel(l);
            }
        }
    }

    public AlarmLevel getAlarmLevel(SystemEventKey key) {
        String value = getValue0(SYSTEM_EVENT_ALARMLEVEL_PREFIX + key.name(), null);
        if (value == null) {
            return null;
        }
        return AlarmLevel.values()[Integer.parseInt(value)];
    }

    public AlarmLevel getAlarmLevel(AuditEventKey key) {
        String value = getValue0(AUDIT_EVENT_ALARMLEVEL_PREFIX + key.name(), null);
        if (value == null) {
            return null;
        }
        return AlarmLevel.values()[Integer.parseInt(value)];
    }

    private static void setValue(JdbcTemplate ejt, String key, String value) {
        ejt.execute(String.format("insert into systemSettings values ('%s','%s')", key, value));
    }

    private String getValue0(String key) {
        return getValue0(key, (String) DEFAULT_VALUES.get(key));
    }

    private String getValue0(String key, String defaultValue) {
        String result = cache.get(key);
        if (result == null) {
            if (!cache.containsKey(key)) {
                try {
                    //TODOD was BaseDao
                    result = ejt.queryForObject("select settingValue from systemSettings where settingName=?", String.class, key);
                } catch (EmptyResultDataAccessException e) {
                    result = null;
                }
                cache.put(key, result);
                if (result == null) {
                    result = defaultValue;
                }
            } else {
                result = defaultValue;
            }
        }
        return result;
    }

    private int getIntValue0(String key) {
        Integer defaultValue = (Integer) DEFAULT_VALUES.get(key);
        if (defaultValue == null) {
            return getIntValue0(key, 0);
        }
        return getIntValue0(key, defaultValue);
    }

    private TimePeriods getTimePeriodsValue0(String key) {
        TimePeriods defaultValue = (TimePeriods) DEFAULT_VALUES.get(key);
        if (defaultValue == null) {
            throw new ShouldNeverHappenException("No default for: " + key);
        }
        return TimePeriods.fromId(getIntValue0(key, defaultValue.getId()));
    }

    private int getIntValue0(String key, int defaultValue) {
        String value = getValue0(key, null);
        if (value == null) {
            return defaultValue;
        }
        try {
            return Integer.parseInt(value);
        } catch (NumberFormatException e) {
            return defaultValue;
        }
    }

    private boolean getBooleanValue0(String key) {
        return getBooleanValue0(key, false);
    }

    private boolean getBooleanValue0(String key, boolean defaultValue) {
        String value = getValue0(key, null);
        if (value == null) {
            return defaultValue;
        }
        return charToBool(value);
    }

    public void setValue0(final String key, final String value) {
        // Update the cache
        cache.put(key, value);

        // Update the database
        final JdbcTemplate ejt2 = ejt;
        getTransactionTemplate().execute(
                new TransactionCallbackWithoutResult() {
                    @Override
                    protected void doInTransactionWithoutResult(
                            TransactionStatus status) {
                                // Delete any existing value.
                                removeValue(key);

                                // Insert the new value if it's not null.
                                if (value != null) {
                                    ejt2.update(
                                            "insert into systemSettings values (?,?)",
                                            new Object[]{key, value});
                                }
                            }
                });
    }

    public void setIntValue(String key, int value) {
        setValue0(key, Integer.toString(value));
    }

    public void setBooleanValue(String key, boolean value) {
        setValue0(key, boolToChar(value));
    }

    public void removeValue(String key) {
        // Remove the value from the cache
        cache.remove(key);

        // Reset the cached values too.
        FUTURE_DATE_LIMIT = -1;

        ejt.update("delete from systemSettings where settingName=?",
                new Object[]{key});
    }

    public long getFutureDateLimit() {
        if (FUTURE_DATE_LIMIT == -1) {
            FUTURE_DATE_LIMIT = getTimePeriodsValue0(FUTURE_DATE_LIMIT_PERIOD_TYPE).getMillis(getIntValue0(FUTURE_DATE_LIMIT_PERIODS));
        }
        return FUTURE_DATE_LIMIT;
    }

    public Color getColour(String key) {
        try {
            return ColorUtils.toColor(getValue0(key));
        } catch (InvalidArgumentException e) {
            // Should never happen. Just use the default.
            try {
                return ColorUtils.toColor((String) DEFAULT_VALUES.get(key));
            } catch (InvalidArgumentException e1) {
                // This should definitely never happen
                throw new ShouldNeverHappenException(e1);
            }
        }
    }

    /**
     * Special caching for the future dated values property, which needs high
     * performance.
     */
    private static long FUTURE_DATE_LIMIT = -1;

    public static final Map<String, Object> DEFAULT_VALUES = new HashMap<>();

    static {
        DEFAULT_VALUES.put(DATABASE_SCHEMA_VERSION, "0.7.0");

        DEFAULT_VALUES.put(HTTP_CLIENT_PROXY_SERVER, "");
        DEFAULT_VALUES.put(HTTP_CLIENT_PROXY_PORT, -1);
        DEFAULT_VALUES.put(HTTP_CLIENT_PROXY_USERNAME, "");
        DEFAULT_VALUES.put(HTTP_CLIENT_PROXY_PASSWORD, "");

        DEFAULT_VALUES.put(EMAIL_SMTP_HOST, "");
        DEFAULT_VALUES.put(EMAIL_SMTP_PORT, 25);
        DEFAULT_VALUES.put(EMAIL_FROM_ADDRESS, "");
        DEFAULT_VALUES.put(EMAIL_SMTP_USERNAME, "");
        DEFAULT_VALUES.put(EMAIL_SMTP_PASSWORD, "");
        DEFAULT_VALUES.put(EMAIL_FROM_NAME, "ScadaBR");

        DEFAULT_VALUES.put(EVENT_PURGE_PERIOD_TYPE, TimePeriods.YEARS);
        DEFAULT_VALUES.put(EVENT_PURGE_PERIODS, 1);

        DEFAULT_VALUES.put(REPORT_PURGE_PERIOD_TYPE, TimePeriods.MONTHS);
        DEFAULT_VALUES.put(REPORT_PURGE_PERIODS, 1);

        DEFAULT_VALUES.put(NEW_VERSION_NOTIFICATION_LEVEL,
                NOTIFICATION_LEVEL_STABLE);

        DEFAULT_VALUES.put(LANGUAGE, "en");

        DEFAULT_VALUES.put(FILEDATA_PATH, "~/WEB-INF/filedata");
        DEFAULT_VALUES.put(HTTPDS_PROLOGUE, "");
        DEFAULT_VALUES.put(HTTPDS_EPILOGUE, "");
        DEFAULT_VALUES.put(UI_PERFORAMANCE, 2000);
        DEFAULT_VALUES.put(GROVE_LOGGING, false);
        DEFAULT_VALUES.put(FUTURE_DATE_LIMIT_PERIODS, 24);
        DEFAULT_VALUES.put(FUTURE_DATE_LIMIT_PERIOD_TYPE,
                TimePeriods.HOURS);
        try {
            DEFAULT_VALUES.put(INSTANCE_DESCRIPTION, String.format("ScadaBR @%s", InetAddress.getLocalHost().getCanonicalHostName()));
        } catch (UnknownHostException uhe) {
            DEFAULT_VALUES.put(INSTANCE_DESCRIPTION, "ScadaBR @Unknown host");
        }

        DEFAULT_VALUES.put(CHART_BACKGROUND_COLOUR, "white");
        DEFAULT_VALUES.put(PLOT_BACKGROUND_COLOUR, "white");
        DEFAULT_VALUES.put(PLOT_GRIDLINE_COLOUR, "silver");
    }

    public void resetDataBase() {
        final JdbcTemplate ejt2 = ejt;
        getTransactionTemplate().execute(
                new TransactionCallbackWithoutResult() {
                    @Override
                    protected void doInTransactionWithoutResult(
                            TransactionStatus status) {

                                ejt2.execute("delete from watchLists");
                                ejt2.execute("delete from mangoViews");

                                ejt2.execute("delete from pointEventDetectors");
                                ejt2.execute("delete from compoundEventDetectors");
                                ejt2.execute("delete from scheduledEvents");

                                ejt2.execute("delete from pointLinks");

                                ejt2.execute("delete from events");
                                ejt2.execute("delete from reports");
                                ejt2.execute("delete from pointHierarchy");

                                ejt2.execute("delete from eventHandlers");
                                ejt2.execute("delete from scripts");

                                ejt2.execute("delete from pointValues");
                                ejt2.execute("delete from maintenanceEvents");
                                ejt2.execute("delete from mailingLists");
                                ejt2.execute("delete from compoundEventDetectors");

                                ejt2.execute("delete from users");

                                ejt2.execute("delete from publishers");

                                ejt2.execute("delete from dataPointUsers");
                                ejt2.execute("delete from dataSourceUsers");

                                ejt2.execute("delete from dataPoints");
                                ejt2.execute("delete from dataSources");

                            }
                });

    }

    public void saveAlarmLevel(SystemEventKey key) {
        setValue0(SYSTEM_EVENT_ALARMLEVEL_PREFIX + key.name(), Integer.toString(key.getAlarmLevel().ordinal()));
    }

    public void saveAlarmlevel(AuditEventKey key) {
        setValue0(AUDIT_EVENT_ALARMLEVEL_PREFIX + key.name(), Integer.toString(key.getAlarmLevel().ordinal()));
    }

    @Override
    public String getInstanceDescription() {
        return getValue0(INSTANCE_DESCRIPTION);
    }

    @Override
    public String getServletContextPath() {
        return getValue0(SERVLET_CONTEXT_PATH);
    }

    @Override
    public void setServletContextPath(String contextPath) {
        setValue0(SERVLET_CONTEXT_PATH, contextPath);
    }

    @Override
    public String getNewVersionNotificationLevel() {
        return getValue0(NEW_VERSION_NOTIFICATION_LEVEL);
    }

    @Override
    public HttpClientProxy getHttpClientProxy() {
        return httpClientProxyImpl;
    }

    @Override
    public Email getEmail() {
        return emailImpl;
    }

    @Override
    public Events getEvents() {
        return eventsImpl;
    }

    @Override
    public Reports getReports() {
        return reportsImpl;
    }

    public class EventsImpl implements Events {

        @Override
        public TimePeriods getPurgePeriodType() {
            return getTimePeriodsValue0(EVENT_PURGE_PERIOD_TYPE);
        }

        @Override
        public int getPurgePeriods() {
            return getIntValue0(EVENT_PURGE_PERIODS);
        }

        @Override
        public DateTime getCutoff(DateTime cutoff) {
            return getPurgePeriodType().minus(cutoff, getPurgePeriods());
        }
    }

    public class ReportsImpl implements Reports {

        @Override
        public TimePeriods getPurgePeriodType() {
            return getTimePeriodsValue0(REPORT_PURGE_PERIOD_TYPE);
        }

        @Override
        public int getPurgePeriods() {
            return getIntValue0(REPORT_PURGE_PERIODS);
        }
        
        @Override
        public DateTime getCutoff(DateTime cutoff) {
            return getPurgePeriodType().minus(cutoff, getPurgePeriods());
        }
    }

    @Override
    public String getFileDataPath() {
        return getValue0(FILEDATA_PATH);
    }

    public class EmailImpl implements Email {

        @Override
        public MangoEmailContent.ContentType getContentType() {
            return MangoEmailContent.ContentType.valueOf(getValue0(EMAIL_CONTENT_TYPE));
        }

        @Override
        public String getFromAddress() {
            return getValue0(EMAIL_FROM_ADDRESS);
        }

        @Override
        public String getFromName() {
            return getValue0(EMAIL_FROM_NAME);
        }

        @Override
        public String getSmtpHost() {
            return getValue0(EMAIL_SMTP_HOST);
        }

        @Override
        public int getSmtpPort() {
            return getIntValue0(EMAIL_SMTP_PORT);
        }

        @Override
        public boolean isAuthorization() {
            return getBooleanValue0(EMAIL_AUTHORIZATION);
        }

        @Override
        public String getSmtpUsername() {
            return getValue0(EMAIL_SMTP_USERNAME);
        }

        @Override
        public String getSmtpPassword() {
            return getValue0(EMAIL_SMTP_PASSWORD);
        }

        @Override
        public boolean isTls() {
            return getBooleanValue0(EMAIL_TLS);
        }
    }

    public class HttpClientProxyImpl implements HttpClientProxy {

        @Override
        public boolean isEnabled() {
            return getBooleanValue0(HTTP_CLIENT_USE_PROXY);
        }

        @Override
        public String getServer() {
            return getValue0(HTTP_CLIENT_PROXY_SERVER);
        }

        @Override
        public int getPort() {
            return getIntValue0(HTTP_CLIENT_PROXY_PORT);
        }

        @Override
        public String getUsername() {
            return getValue0(HTTP_CLIENT_PROXY_USERNAME, "");
        }

        @Override
        public String getPassword() {
            return getValue0(HTTP_CLIENT_PROXY_PASSWORD, "");
        }
    }
}
