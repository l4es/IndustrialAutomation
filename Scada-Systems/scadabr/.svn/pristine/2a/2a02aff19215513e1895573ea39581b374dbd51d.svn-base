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
package com.serotonin.mango.rt.maint;

import br.org.scadabr.dao.DataPointDao;
import br.org.scadabr.dao.DataSourceDao;
import br.org.scadabr.dao.SystemSettingsDao;
import br.org.scadabr.rt.SchedulerPool;
import br.org.scadabr.utils.ImplementMeException;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Enumeration;
import java.util.List;
import java.util.UUID;

import com.serotonin.mango.Common;
import com.serotonin.mango.rt.event.type.SystemEventType;
import br.org.scadabr.timer.cron.SystemCronTask;
import br.org.scadabr.util.queue.ByteQueue;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.event.type.SystemEventKey;
import java.text.ParseException;
import java.util.TimeZone;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

/**
 * @author Matthew Lohbihler
 *
 */
@Deprecated
@Configurable
public class VersionCheck extends SystemCronTask {

    @Autowired
    private DataPointDao dataPointDao;
    @Autowired
    private DataSourceDao dataSourceDao;
    @Autowired
    private SystemSettingsDao systemSettingsDao;
    @Autowired
    private SchedulerPool schedulerPool;

    private static final String INSTANCE_ID_FILE = "/WEB-INF/instance.txt";

    private static VersionCheck instance;
    private static String instanceId;

    /**
     * This method will set up the version checking job. It assumes that the
     * corresponding system setting for running this job is true.
     */
    public static void start(String cronPattern) throws ParseException {
        throw new ImplementMeException();
        /*
         synchronized (INSTANCE_ID_FILE) {
         stop();
         instance = new VersionCheck(cronPattern, CronExpression.TIMEZONE_UTC);
         // Common.systemCronPool.schedule(instance);
         }
         */
    }

    public static void stop() {
        synchronized (INSTANCE_ID_FILE) {
            if (instance != null) {
                instance.cancel();
                instance = null;
            }
        }
    }

    private VersionCheck(String cronPattern, TimeZone tz) throws ParseException {
        super(cronPattern, tz);
    }

    public static String getInstanceId() {
        if (instanceId == null) {
            instanceId = calcMachineId();
        }
        return instanceId;
    }

    @Override
    protected void run(long scheduledExecutionTime) {
        try {
            String notifLevel = systemSettingsDao.getNewVersionNotificationLevel();
            newVersionCheck(scheduledExecutionTime, notifLevel);
        } catch (SocketTimeoutException e) {
            // Ignore
        } catch (Exception e) {
            getEventType().fire(scheduledExecutionTime, "event.version.error", e.getClass().getName(), e.getMessage());
        }
    }

    public LocalizableMessage newVersionCheck(String notifLevel) throws Exception {
        return newVersionCheck(System.currentTimeMillis(), notifLevel);
    }

    private static SystemEventType getEventType() {
        return new SystemEventType(SystemEventKey.VERSION_CHECK, 0);
    }

    private LocalizableMessage newVersionCheck(long fireTime, String notifLevel) throws Exception {
        String result = newVersionCheckImpl(notifLevel);
        if (result == null) {
            // If the version matches, clear any outstanding event.
            getEventType().clearAlarm(fireTime);
            return new LocalizableMessageImpl("event.version.uptodate");
        }

        // If the version doesn't match this version, raise an event.
        final LocalizableMessage message = new LocalizableMessageImpl("event.version.available", result);
        getEventType().fire(fireTime, message);
        return message;
    }

    private String newVersionCheckImpl(String notifLevel) throws Exception {
        throw new ImplementMeException();
        /*
         HttpClient httpClient = common.getHttpClient();

         PostMethod postMethod = new PostMethod(Common.getGroveUrl(Common.GroveServlets.VERSION_CHECK));

         postMethod.addParameter("instanceId", getInstanceId());
         postMethod.addParameter("instanceName", systemSettingsDao.getValue(SystemSettingsDao.INSTANCE_DESCRIPTION));
         try {
         postMethod.addParameter("instanceIp", InetAddress.getLocalHost().getHostAddress());
         } catch (UnknownHostException e) {
         postMethod.addParameter("instanceIp", "unknown");
         }

         postMethod.addParameter("instanceVersion", Common.getVersion());

         StringBuilder datasourceTypes = new StringBuilder();
         for (DataSourceVO<?> config : dataSourceDao.getDataSources()) {
         if (config.isEnabled()) {
         int points = 0;
         for (DataPointVO point : dataPointDao.getDataPoints(config.getId(), null)) {
         if (point.isEnabled()) {
         points++;
         }
         }

         if (datasourceTypes.length() > 0) {
         datasourceTypes.append(',');
         }
         datasourceTypes.append(config.getType().getId()).append(':').append(points);
         }
         }
         postMethod.addParameter("datasourceTypes", datasourceTypes.toString());

         StringBuilder publisherTypes = new StringBuilder();
         for (PublisherVO<?> config : publisherDao.getPublishers()) {
         if (config.isEnabled()) {
         if (publisherTypes.length() > 0) {
         publisherTypes.append(',');
         }
         publisherTypes.append(config.getType().getId()).append(':').append(config.getPoints().size());
         }
         }
         postMethod.addParameter("publisherTypes", publisherTypes.toString());

         int responseCode = httpClient.executeMethod(postMethod);
         if (responseCode != HttpStatus.SC_OK) {
         throw new HttpException("Invalid response code: " + responseCode);
         }

         Header devHeader = postMethod.getResponseHeader("Mango-dev");
         if (devHeader != null) {
         String devVersion = devHeader.getValue();
         String stage = devVersion.substring(devVersion.length() - 1);
         devVersion = devVersion.substring(0, devVersion.length() - 1);

         // There is a new version development version. Check if we're interested.
         if (Common.getVersion().equals(devVersion)) // We already have it. Never mind.
         {
         return null;
         }

         // Beta?
         if (SystemSettingsDao.NOTIFICATION_LEVEL_BETA.equals(stage)
         && SystemSettingsDao.NOTIFICATION_LEVEL_BETA.equals(notifLevel)) {
         return devVersion + " beta";
         }

         // Release candidate?
         if (SystemSettingsDao.NOTIFICATION_LEVEL_RC.equals(stage)
         && (SystemSettingsDao.NOTIFICATION_LEVEL_BETA.equals(notifLevel) || SystemSettingsDao.NOTIFICATION_LEVEL_RC
         .equals(notifLevel))) {
         return devVersion + " release candidate";
         }
                
         }

         // Either there is no dev version available or we're not interested in it. Check the stable version
         String stableVersion = HttpUtils.readResponseBody(postMethod);

         if (Common.getVersion().equals(stableVersion)) {
         return null;
         }

         return stableVersion;
         */
    }

    private static String calcMachineId() {
        List<NI> nis = new ArrayList<>();

        try {
            Enumeration<NetworkInterface> eni = NetworkInterface.getNetworkInterfaces();
            while (eni.hasMoreElements()) {
                NetworkInterface netint = eni.nextElement();
                NI ni = new NI();
                ni.name = netint.getName();
                try {
                    ni.hwAddress = netint.getHardwareAddress();
                } catch (SocketException e) {
                    // ignore this too
                }
                if (ni.name != null && ni.hwAddress != null) // Should be for real.
                {
                    nis.add(ni);
                }
            }
        } catch (SocketException e) {
            // ignore
        }

        if (nis.isEmpty()) {
            return null;
        }

        // Sort the NIs just to make sure we always add them in the same order.
        Collections.sort(nis, new Comparator<NI>() {
            @Override
            public int compare(NI ni1, NI ni2) {
                return ni1.name.compareTo(ni2.name);
            }
        });

        ByteQueue queue = new ByteQueue();
        for (NI ni : nis) {
            queue.push(ni.name.getBytes(Common.UTF8_CS));
            queue.push(ni.hwAddress);
        }

        UUID uuid = UUID.nameUUIDFromBytes(queue.popAll());
        return uuid.toString();
    }

    @Override
    protected boolean overrunDetected(long lastExecutionTime, long thisExecutionTime) {
        throw new ImplementMeException();
    }

    static class NI {

        String name;
        byte[] hwAddress;
    }
}
