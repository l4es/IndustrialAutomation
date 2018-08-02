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
package com.serotonin.mango.web.dwr.beans;

import br.org.scadabr.utils.ImplementMeException;
import java.util.ArrayList;
import java.util.List;
import java.util.ResourceBundle;
import java.util.concurrent.CopyOnWriteArrayList;

import br.org.scadabr.db.dao.ScriptDao;
import br.org.scadabr.vo.exporter.util.SystemSettingsJSONWrapper;
import br.org.scadabr.vo.scripting.ScriptVO;

import br.org.scadabr.json.JsonArray;
import br.org.scadabr.json.JsonException;
import br.org.scadabr.json.JsonObject;
import br.org.scadabr.json.JsonReader;
import br.org.scadabr.json.JsonValue;
import br.org.scadabr.l10n.AbstractLocalizer;
import br.org.scadabr.timer.cron.SystemRunnable;
import com.serotonin.mango.Common;
import com.serotonin.mango.db.dao.CompoundEventDetectorDao;
import com.serotonin.mango.db.dao.DataPointDao;
import com.serotonin.mango.db.dao.DataSourceDao;
import com.serotonin.mango.db.dao.EventDao;
import com.serotonin.mango.db.dao.MailingListDao;
import com.serotonin.mango.db.dao.MaintenanceEventDao;
import com.serotonin.mango.db.dao.PointLinkDao;
import com.serotonin.mango.db.dao.PointValueDao;
import com.serotonin.mango.db.dao.PublisherDao;
import com.serotonin.mango.db.dao.ScheduledEventDao;
import com.serotonin.mango.db.dao.UserDao;
import com.serotonin.mango.db.dao.ViewDao;
import com.serotonin.mango.db.dao.WatchListDao;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.rt.event.type.EventType;
import com.serotonin.mango.util.BackgroundContext;
import com.serotonin.mango.util.LocalizableJsonException;
import com.serotonin.mango.view.View;
import com.serotonin.mango.view.text.PlainRenderer;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.User;
import com.serotonin.mango.vo.WatchList;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.event.CompoundEventDetectorVO;
import com.serotonin.mango.vo.event.EventHandlerVO;
import com.serotonin.mango.vo.event.MaintenanceEventVO;
import com.serotonin.mango.vo.event.PointEventDetectorVO;
import com.serotonin.mango.vo.event.ScheduledEventVO;
import com.serotonin.mango.vo.hierarchy.PointFolder;
import com.serotonin.mango.vo.link.PointLinkVO;
import com.serotonin.mango.vo.mailingList.MailingList;
import com.serotonin.mango.vo.permission.DataPointAccess;
import com.serotonin.mango.vo.publish.PublisherVO;
import com.serotonin.mango.web.dwr.EmportDwr;
import br.org.scadabr.util.ProgressiveTask;
import br.org.scadabr.util.StringUtils;
import br.org.scadabr.web.dwr.DwrMessageI18n;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.l10n.Localizer;

/**
 * @author Matthew Lohbihler
 */
public class ImportTask extends ProgressiveTask implements SystemRunnable {

    private final JsonReader reader;
    private final ResourceBundle bundle;
    private final User user;
    private final DwrResponseI18n response;
    private final UserDao userDao = UserDao.getInstance();
    private final DataSourceDao dataSourceDao = DataSourceDao.getInstance();
    private final DataPointDao dataPointDao = DataPointDao.getInstance();
    private final ViewDao viewDao = ViewDao.getInstance();
    private final PointLinkDao pointLinkDao = PointLinkDao.getInstance();
    private final ScheduledEventDao scheduledEventDao = ScheduledEventDao.getInstance();
    private final CompoundEventDetectorDao compoundEventDetectorDao = CompoundEventDetectorDao.getInstance();
    private final EventDao eventDao = EventDao.getInstance();
    private final MailingListDao mailingListDao = MailingListDao.getInstance();
    private final PublisherDao publisherDao = PublisherDao.getInstance();
    private final WatchListDao watchListDao = WatchListDao.getInstance();
    private final MaintenanceEventDao maintenanceEventDao = MaintenanceEventDao.getInstance();
    private final ScriptDao scriptDao = ScriptDao.getInstance();

    private final List<JsonValue> users;
    private int userIndexPass1;
    private final List<JsonValue> pass2users;
    private int userIndexPass2;
    private final List<JsonValue> dataSources;
    private int dataSourceIndex;
    private final List<JsonValue> dataPoints;
    private int dataPointIndex;
    private final List<JsonValue> views;
    private int viewIndex;
    private JsonArray pointHierarchy;
    private final List<JsonValue> pointLinks;
    private int pointLinkIndex;
    private final List<JsonValue> scheduledEvents;
    private int scheduledEventIndex;
    private final List<JsonValue> compoundEventDetectors;
    private int compoundEventDetectorIndex;
    private final List<JsonValue> eventHandlers;
    private int eventHandlerIndex;
    private final List<JsonValue> mailingLists;
    private int mailingListIndex;
    private final List<JsonValue> publishers;
    private int publisherIndex;
    private final List<JsonValue> watchLists;
    private int watchListIndex;
    private final List<JsonValue> maintenanceEvents;
    private int maintenanceEventIndex;
    private final List<JsonValue> scripts;
    private int scriptsIndex;
    private final List<JsonValue> pointValues;
    private int pointValuesIndex;
    private final List<JsonValue> systemSettings;
    private int systemSettingsIndex;

    private final List<Integer> disabledDataSources = new ArrayList<>();

    public ImportTask(JsonReader reader, JsonObject root,
            ResourceBundle bundle, User user) {
        this.reader = reader;
        this.bundle = bundle;
        this.user = user;
        response = new DwrResponseI18n();
        response.setMessages(new CopyOnWriteArrayList<DwrMessageI18n>());

        users = nonNullList(root, EmportDwr.USERS);
        pass2users = new ArrayList<>();
        dataSources = nonNullList(root, EmportDwr.DATA_SOURCES);
        dataPoints = nonNullList(root, EmportDwr.DATA_POINTS);
        views = nonNullList(root, EmportDwr.GRAPHICAL_VIEWS);
        pointHierarchy = root.getJsonArray(EmportDwr.POINT_HIERARCHY);
        pointLinks = nonNullList(root, EmportDwr.POINT_LINKS);
        scheduledEvents = nonNullList(root, EmportDwr.SCHEDULED_EVENTS);
        compoundEventDetectors = nonNullList(root,
                EmportDwr.COMPOUND_EVENT_DETECTORS);
        mailingLists = nonNullList(root, EmportDwr.MAILING_LISTS);
        publishers = nonNullList(root, EmportDwr.PUBLISHERS);
        eventHandlers = nonNullList(root, EmportDwr.EVENT_HANDLERS);
        watchLists = nonNullList(root, EmportDwr.WATCH_LISTS);
        maintenanceEvents = nonNullList(root, EmportDwr.MAINTENANCE_EVENTS);
        scripts = nonNullList(root, EmportDwr.SCRIPTS);
        pointValues = nonNullList(root, EmportDwr.POINT_VALUES);
        systemSettings = nonNullList(root, EmportDwr.SYSTEM_SETTINGS);
        Common.systemCronPool.execute(this);
    }

    private List<JsonValue> nonNullList(JsonObject root, String key) {
        JsonArray arr = root.getJsonArray(key);
        if (arr == null) {
            return new ArrayList<>();
        }
        return arr.getElements();
    }

    public DwrResponseI18n getResponse() {
        return response;
    }

    @Override
    protected void runImpl() {
        try {
            BackgroundContext.set(user);

            if (userIndexPass1 < users.size()) {
                importUser(users.get(userIndexPass1++).toJsonObject());
                return;
            }

            if (dataSourceIndex < dataSources.size()) {
                importDataSource(dataSources.get(dataSourceIndex++)
                        .toJsonObject());
                return;
            }

            if (dataPointIndex < dataPoints.size()) {
                importDataPoint(dataPoints.get(dataPointIndex++).toJsonObject());
                return;
            }

            if (viewIndex < views.size()) {
                importView(views.get(viewIndex++).toJsonObject());
                return;
            }

            if (userIndexPass2 < pass2users.size()) {
                importUserPermissions(pass2users.get(userIndexPass2++)
                        .toJsonObject());
                return;
            }

            if (pointHierarchy != null) {
                importPointHierarchy(pointHierarchy);
                pointHierarchy = null;
                return;
            }

            if (pointLinkIndex < pointLinks.size()) {
                importPointLink(pointLinks.get(pointLinkIndex++).toJsonObject());
                return;
            }

            if (scheduledEventIndex < scheduledEvents.size()) {
                importScheduledEvent(scheduledEvents.get(scheduledEventIndex++)
                        .toJsonObject());
                return;
            }

            if (compoundEventDetectorIndex < compoundEventDetectors.size()) {
                importCompoundEventDetector(compoundEventDetectors.get(
                        compoundEventDetectorIndex++).toJsonObject());
                return;
            }

            if (mailingListIndex < mailingLists.size()) {
                importMailingList(mailingLists.get(mailingListIndex++)
                        .toJsonObject());
                return;
            }

            if (publisherIndex < publishers.size()) {
                importPublisher(publishers.get(publisherIndex++).toJsonObject());
                return;
            }

            if (eventHandlerIndex < eventHandlers.size()) {
                importEventHandler(eventHandlers.get(eventHandlerIndex++)
                        .toJsonObject());
                return;
            }

            if (watchListIndex < watchLists.size()) {
                importWatchList(watchLists.get(watchListIndex++).toJsonObject());
                return;
            }

            if (maintenanceEventIndex < maintenanceEvents.size()) {
                importMaintenanceEvent(maintenanceEvents.get(
                        maintenanceEventIndex++).toJsonObject());
                return;
            }

            // wait users
            if (scriptsIndex < scripts.size()) {
                importScripts(scripts.get(scriptsIndex++).toJsonObject());
                return;
            }

            if (pointValuesIndex < pointValues.size()) {
                importPointValues(pointValues.get(pointValuesIndex++)
                        .toJsonObject());
                return;
            }

            if (systemSettingsIndex < systemSettings.size()) {
                importSystemSettings(systemSettings.get(systemSettingsIndex++)
                        .toJsonObject());
                return;
            }

            completed = true;

            // Restart any data sources that were disabled.
            for (Integer id : disabledDataSources) {
                DataSourceVO<?> ds = dataSourceDao.getDataSource(id);
                ds.setEnabled(true);
                Common.ctx.getRuntimeManager().saveDataSource(ds);
            }
        } catch (Exception e) {
            String msg = e.getMessage();
            Throwable t = e;
            while ((t = t.getCause()) != null) {
                msg += ", " + AbstractLocalizer.localizeI18nKey("emport.causedBy", bundle)
                        + " '" + t.getMessage() + "'";
            }
            response.addGeneric("common.default", msg);
        } finally {
            BackgroundContext.remove();
        }
    }

    private void importUser(JsonObject userJson) {
        String username = userJson.getString("username");
        if (username == null) {
            response.addGeneric("emport.user.username");
        } else {
            User user = userDao.getUser(username);
            if (user == null) {
                user = new User();
                user.setUsername(username);
                user.setPassword(Common.encrypt(username));
                user.setDataSourcePermissions(new ArrayList<Integer>());
                user.setDataPointPermissions(new ArrayList<DataPointAccess>());
            }

            try {
                reader.populateObject(user, userJson);

                // Now validate it. Use a new response object so we can
                // distinguish errors in this user from other
                // errors.
                DwrResponseI18n userResponse = new DwrResponseI18n();
                user.validate(userResponse);
                if (!userResponse.isEmpty()) // Too bad. Copy the errors into the actual response.
                {
                    copyValidationMessages(userResponse, "emport.user.prefix",
                            username);
                } else {
                    // Sweet. Save it.
                    boolean isnew = user.getId() == Common.NEW_ID;
                    userDao.saveUser(user);
                    addSuccessMessage(isnew, "emport.user.prefix", username);

                    // Add the user to the second pass list.
                    pass2users.add(userJson);
                }
            } catch (LocalizableJsonException e) {
                response.addGeneric("emport.user.prefix", username, e);
            } catch (JsonException e) {
                response.addGeneric("emport.user.prefix", username, getJsonExceptionMessage(e));
            }
        }
    }

    private void importDataSource(JsonObject dataSource) {
        String xid = dataSource.getString("xid");
        String name = dataSource.getString("name");
        if (xid.isEmpty()) {
            response.addGeneric("emport.dataSource.xid",
                    name == null ? "(undefined)" : name);
        } else {
            DataSourceVO<?> vo = dataSourceDao.getDataSource(xid);
            if (vo == null) {
                String typeStr = dataSource.getString("type");
                if (typeStr == null) {
                    response.addGeneric("emport.dataSource.missingType",
                            xid, DataSourceVO.Type.getTypeList());
                } else {
                    DataSourceVO.Type type = DataSourceVO.Type
                            .valueOfIgnoreCase(typeStr);
                    if (type == null) {
                        response.addGeneric(
                                "emport.dataSource.invalidType", xid, typeStr,
                                DataSourceVO.Type.getTypeList());
                    } else {
                        vo = type.createDataSourceVO();
                        vo.setXid(xid);
                    }
                }
            }

            if (vo != null) {
                try {
                    // The VO was found or successfully created. Finish reading
                    // it in.
                    reader.populateObject(vo, dataSource);

                    // Now validate it. Use a new response object so we can
                    // distinguish errors in this vo from
                    // other errors.
                    DwrResponseI18n voResponse = new DwrResponseI18n();
                    vo.validate(voResponse);
                    if (!voResponse.isEmpty()) // Too bad. Copy the errors into the actual response.
                    {
                        copyValidationMessages(voResponse,
                                "emport.dataSource.prefix", xid);
                    } else {
                        // Sweet. Save it.
                        boolean isnew = vo.isNew();
                        Common.ctx.getRuntimeManager().saveDataSource(vo);
                        addSuccessMessage(isnew, "emport.dataSource.prefix",
                                xid);
                    }
                } catch (LocalizableJsonException e) {
                    response.addGeneric("emport.dataSource.prefix", xid, e);
                } catch (JsonException e) {
                    response.addGeneric("emport.dataSource.prefix", xid, getJsonExceptionMessage(e));
                }
            }
        }
    }

    private void importDataPoint(JsonObject dataPoint) {
        String xid = dataPoint.getString("xid");
        String name = dataPoint.getString("name");
        if (xid == null) {
            response.addGeneric("emport.dataPoint.xid", name == null ? "(undefined)" : name);
        } else {
            DataSourceVO<?> dsvo;
            DataPointVO vo = dataPointDao.getDataPoint(xid);
            if (vo == null) {
                // Locate the data source for the point.
                String dsxid = dataPoint.getString("dataSourceXid");
                dsvo = dataSourceDao.getDataSource(dsxid);
                if (dsvo == null) {
                    response.addGeneric("emport.dataPoint.badReference", xid);
                } else {
                    vo = new DataPointVO();
                    vo.setXid(xid);
                    vo.setDataSourceId(dsvo.getId());
                    vo.setDataSourceXid(dsxid);
                    vo.setPointLocator(dsvo.createPointLocator());
                    vo.setEventDetectors(new ArrayList<PointEventDetectorVO>(0));
                    vo.setTextRenderer(new PlainRenderer());
                }
            } else {
                dsvo = dataSourceDao.getDataSource(vo.getDataSourceId());
            }

            if (vo != null) {
                try {
                    reader.populateObject(vo, dataPoint);

                    // Now validate it. Use a new response object so we can
                    // distinguish errors in this vo from
                    // other errors.
                    DwrResponseI18n voResponse = new DwrResponseI18n();
                    vo.validate(voResponse);
                    if (!voResponse.isEmpty()) // Too bad. Copy the errors into the actual response.
                    {
                        copyValidationMessages(voResponse,
                                "emport.dataPoint.prefix", xid);
                    } else {
                        // Sweet. Save it.
                        boolean isnew = vo.isNew();

                        // Check if this data source is enabled. Because data
                        // sources do automatic stuff upon the
                        // starting of a point, we need to shut it down. We
                        // restart again once all data points are
                        // imported.
                        if (dsvo.isEnabled()
                                && !disabledDataSources.contains(dsvo.getId())) {
                            disabledDataSources.add(dsvo.getId());
                            dsvo.setEnabled(false);
                            Common.ctx.getRuntimeManager().saveDataSource(dsvo);
                        }

                        Common.ctx.getRuntimeManager().saveDataPoint(vo);
                        addSuccessMessage(isnew, "emport.dataPoint.prefix", xid);
                    }
                } catch (LocalizableJsonException e) {
                    response.addGeneric("emport.dataPoint.prefix", xid, e);
                } catch (JsonException e) {
                    response.addGeneric("emport.dataPoint.prefix", xid, getJsonExceptionMessage(e));
                }
            }
        }
    }

    private void importView(JsonObject viewJson) {
        String xid = viewJson.getString("xid");
        if (xid == null) {
            response.addGeneric("emport.view.xid");
        } else {
            View view = viewDao.getViewByXid(xid);
            if (view == null) {
                view = new View();
                view.setXid(xid);
            }

            try {
                reader.populateObject(view, viewJson);

                // Now validate it. Use a new response object so we can
                // distinguish errors in this view from other
                // errors.
                DwrResponseI18n viewResponse = new DwrResponseI18n();
                view.validate(viewResponse);
                if (!viewResponse.isEmpty()) // Too bad. Copy the errors into the actual response.
                {
                    copyValidationMessages(viewResponse, "emport.view.prefix",
                            xid);
                } else {
                    // Sweet. Save it.
                    boolean isnew = view.isNew();
                    viewDao.saveView(view);
                    addSuccessMessage(isnew, "emport.view.prefix", xid);
                }
            } catch (LocalizableJsonException e) {
                response.addGeneric("emport.view.prefix", xid, e);
            } catch (JsonException e) {
                response.addGeneric("emport.view.prefix", xid, getJsonExceptionMessage(e));
            }
        }
    }

    private void importUserPermissions(JsonObject userJson) {
        // This method uses user objects from the second pass list, which have
        // already been validated.
        String username = userJson.getString("username");
        User user = userDao.getUser(username);

        try {
            user.jsonDeserializePermissions(reader, userJson);
            userDao.saveUser(user);
            addSuccessMessage(false, "emport.userPermission.prefix", username);
        } catch (LocalizableJsonException e) {
            response.addGeneric("emport.userPermission.prefix", username, e);
        } catch (JsonException e) {
            response.addGeneric("emport.userPermission.prefix", username, getJsonExceptionMessage(e));
        }
    }

    @SuppressWarnings("unchecked")
    private void importPointHierarchy(JsonArray pointHierarchyJson) {
        PointFolder root = new PointFolder(0, "Root");
        try {
            List<PointFolder> subfolders = reader.readPropertyValue(
                    pointHierarchyJson, List.class, PointFolder.class);
            root.setSubfolders(subfolders);

            for (DataPointVO dp : dataPointDao.getDataPoints(null, false)) {
                dp.setPointFolderId(0);
                dataPointDao.updateDataPointShallow(dp);
            }

            // Save the new values.
            dataPointDao.savePointHierarchy(root);
            response.addGeneric("emport.pointHierarchy.prefix",
                    AbstractLocalizer.localizeI18nKey("emport.saved", bundle));
        } catch (LocalizableJsonException e) {
            response.addGeneric("emport.pointHierarchy.prefix", e);
        } catch (JsonException e) {
            response.addGeneric("emport.pointHierarchy.prefix", getJsonExceptionMessage(e));
        }
    }

    private void importPointLink(JsonObject pointLink) {
        String xid = pointLink.getString("xid");
        if (xid == null) {
            response.addGeneric("emport.pointLink.xid");
        } else {
            PointLinkVO vo = pointLinkDao.getPointLink(xid);
            if (vo == null) {
                vo = new PointLinkVO();
                vo.setXid(xid);
            }

            try {
                reader.populateObject(vo, pointLink);

                // Now validate it. Use a new response object so we can
                // distinguish errors in this vo from other errors.
                DwrResponseI18n voResponse = new DwrResponseI18n();
                vo.validate(voResponse);
                if (!voResponse.isEmpty()) // Too bad. Copy the errors into the actual response.
                {
                    copyValidationMessages(voResponse,
                            "emport.pointLink.prefix", xid);
                } else {
                    // Sweet. Save it.
                    boolean isnew = vo.isNew();
                    Common.ctx.getRuntimeManager().savePointLink(vo);
                    addSuccessMessage(isnew, "emport.pointLink.prefix", xid);
                }
            } catch (LocalizableJsonException e) {
                response.addGeneric("emport.pointLink.prefix", xid, e);
            } catch (JsonException e) {
                response.addGeneric("emport.pointLink.prefix", xid, getJsonExceptionMessage(e));
            }
        }
    }

    private void importScheduledEvent(JsonObject scheduledEvent) {
        String xid = scheduledEvent.getString("xid");
        if (xid == null) {
            response.addGeneric("emport.scheduledEvent.xid");
        } else {
            ScheduledEventVO vo = scheduledEventDao.getScheduledEvent(xid);
            if (vo == null) {
                vo = new ScheduledEventVO();
                vo.setXid(xid);
            }

            try {
                reader.populateObject(vo, scheduledEvent);

                // Now validate it. Use a new response object so we can
                // distinguish errors in this vo from other errors.
                DwrResponseI18n voResponse = new DwrResponseI18n();
                vo.validate(voResponse);
                if (!voResponse.isEmpty()) // Too bad. Copy the errors into the actual response.
                {
                    copyValidationMessages(voResponse,
                            "emport.scheduledEvent.prefix", xid);
                } else {
                    // Sweet. Save it.
                    boolean isnew = vo.isNew();
                    Common.ctx.getRuntimeManager().saveScheduledEvent(vo);
                    addSuccessMessage(isnew, "emport.scheduledEvent.prefix",
                            xid);
                }
            } catch (LocalizableJsonException e) {
                response.addGeneric("emport.scheduledEvent.prefix", xid, e);
            } catch (JsonException e) {
                response.addGeneric("emport.scheduledEvent.prefix", xid, getJsonExceptionMessage(e));
            }
        }
    }

    private void importCompoundEventDetector(JsonObject compoundEventDetector) {
        String xid = compoundEventDetector.getString("xid");
        if (xid == null) {
            response.addGeneric("emport.compoundEvent.xid");
        } else {
            CompoundEventDetectorVO vo = compoundEventDetectorDao
                    .getCompoundEventDetector(xid);
            if (vo == null) {
                vo = new CompoundEventDetectorVO();
                vo.setXid(xid);
            }

            try {
                reader.populateObject(vo, compoundEventDetector);

                // Now validate it. Use a new response object so we can
                // distinguish errors in this vo from other errors.
                DwrResponseI18n voResponse = new DwrResponseI18n();
                vo.validate(voResponse);
                if (!voResponse.isEmpty()) // Too bad. Copy the errors into the actual response.
                {
                    copyValidationMessages(voResponse,
                            "emport.compoundEvent.prefix", xid);
                } else {
                    // Sweet. Save it.
                    boolean isnew = vo.isNew();
                    Common.ctx.getRuntimeManager()
                            .saveCompoundEventDetector(vo);
                    addSuccessMessage(isnew, "emport.compoundEvent.prefix", xid);
                }
            } catch (LocalizableJsonException e) {
                response.addGeneric("emport.compoundEvent.prefix", xid, e);
            } catch (JsonException e) {
                response.addGeneric("emport.compoundEvent.prefix", xid, getJsonExceptionMessage(e));
            }
        }
    }

    private void importMailingList(JsonObject mailingList) {
        String xid = mailingList.getString("xid");
        if (xid == null) {
            response.addGeneric("emport.mailingList.xid");
        } else {
            MailingList vo = mailingListDao.getMailingList(xid);
            if (vo == null) {
                vo = new MailingList();
                vo.setXid(xid);
            }

            try {
                reader.populateObject(vo, mailingList);

                // Now validate it. Use a new response object so we can
                // distinguish errors in this vo from other errors.
                DwrResponseI18n voResponse = new DwrResponseI18n();
                vo.validate(voResponse);
                if (!voResponse.isEmpty()) // Too bad. Copy the errors into the actual response.
                {
                    copyValidationMessages(voResponse,
                            "emport.mailingList.prefix", xid);
                } else {
                    // Sweet. Save it.
                    boolean isnew = vo.getId() == Common.NEW_ID;
                    mailingListDao.saveMailingList(vo);
                    addSuccessMessage(isnew, "emport.mailingList.prefix", xid);
                }
            } catch (LocalizableJsonException e) {
                response.addGeneric("emport.mailingList.prefix", xid, e);
            } catch (JsonException e) {
                response.addGeneric("emport.mailingList.prefix", xid, getJsonExceptionMessage(e));
            }
        }
    }

    private void importPublisher(JsonObject publisher) {
        String xid = publisher.getString("xid");
        String name = publisher.getString("name");
        if (xid == null) {
            response.addGeneric("emport.publisher.xid",
                    name == null ? "(undefined)" : name);
        } else {
            PublisherVO<?> vo = publisherDao.getPublisher(xid);
            if (vo == null) {
                String typeStr = publisher.getString("type");
                if (typeStr == null) {
                    response.addGeneric("emport.publisher.missingType",
                            xid, PublisherVO.Type.getTypeList());
                } else {
                    PublisherVO.Type type = PublisherVO.Type
                            .valueOfIgnoreCase(typeStr);
                    if (type == null) {
                        response.addGeneric(
                                "emport.publisher.invalidType", xid, typeStr,
                                PublisherVO.Type.getTypeList());
                    } else {
                        vo = type.createPublisherVO();
                        vo.setXid(xid);
                    }
                }
            }

            if (vo != null) {
                try {
                    // The VO was found or successfully created. Finish reading
                    // it in.
                    reader.populateObject(vo, publisher);

                    // Now validate it. Use a new response object so we can
                    // distinguish errors in this vo from
                    // other errors.
                    DwrResponseI18n voResponse = new DwrResponseI18n();
                    vo.validate(voResponse);
                    if (!voResponse.isEmpty()) // Too bad. Copy the errors into the actual response.
                    {
                        copyValidationMessages(voResponse,
                                "emport.publisher.prefix", xid);
                    } else {
                        // Sweet. Save it.
                        boolean isnew = vo.isNew();
                        Common.ctx.getRuntimeManager().savePublisher(vo);
                        addSuccessMessage(isnew, "emport.publisher.prefix", xid);
                    }
                } catch (LocalizableJsonException e) {
                    response.addGeneric("emport.publisher.prefix", xid, e);
                } catch (JsonException e) {
                    response.addGeneric("emport.publisher.prefix", xid, getJsonExceptionMessage(e));
                }
            }
        }
    }

    private void importEventHandler(JsonObject eventHandler) {
        String xid = eventHandler.getString("xid");
        if (xid == null) {
            response.addGeneric("emport.eventHandler.xid");
        } else {
            EventHandlerVO handler = eventDao.getEventHandler(xid);
            if (handler == null) {
                handler = new EventHandlerVO();
                handler.setXid(xid);
            }

            try {
                // Find the event type.
                EventType eventType = reader.readObject(
                        eventHandler.getJsonObject("eventType"),
                        EventType.class);

                reader.populateObject(handler, eventHandler);

                // Now validate it. Use a new response object so we can
                // distinguish errors in this vo from other errors.
                DwrResponseI18n voResponse = new DwrResponseI18n();
                handler.validate(voResponse);
                if (!voResponse.isEmpty()) // Too bad. Copy the errors into the actual response.
                {
                    copyValidationMessages(voResponse,
                            "emport.eventHandler.prefix", xid);
                } else {
                    // Sweet.
                    boolean isnew = handler.getId() == Common.NEW_ID;

                    if (!isnew) {
                        // Check if the event type has changed.
                        EventType oldEventType = eventDao
                                .getEventHandlerType(handler.getId());
                        if (!oldEventType.equals(eventType)) {
                            // Event type has changed. Delete the old one.
                            eventDao.deleteEventHandler(handler.getId());

                            // Call it new
                            handler.setId(Common.NEW_ID);
                            isnew = true;
                        }
                    }

                    // Save it.
                    eventDao.saveEventHandler(eventType, handler);
                    addSuccessMessage(isnew, "emport.eventHandler.prefix", xid);
                }
            } catch (LocalizableJsonException e) {
                response.addGeneric("emport.eventHandler.prefix", xid, e);
            } catch (JsonException e) {
                response.addGeneric("emport.eventHandler.prefix", xid, getJsonExceptionMessage(e));
            }
        }
    }

    private void importWatchList(JsonObject watchListJson) {
        String xid = watchListJson.getString("xid");
        if (xid == null) {
            response.addGeneric("emport.watchList.xid");
        } else {
            WatchList watchList = watchListDao.getWatchList(xid);
            if (watchList == null) {
                watchList = new WatchList();
                watchList.setXid(xid);
            }

            try {
                reader.populateObject(watchList, watchListJson);

                // Now validate it. Use a new response object so we can
                // distinguish errors in this user from other
                // errors.
                DwrResponseI18n watchListResponse = new DwrResponseI18n();
                watchList.validate(watchListResponse);
                if (!watchListResponse.isEmpty()) // Too bad. Copy the errors into the actual response.
                {
                    copyValidationMessages(watchListResponse, "emport.watchList.prefix", xid);
                } else {
                    // Sweet. Save it.
                    boolean isnew = watchList.getId() == Common.NEW_ID;
                    watchListDao.saveWatchList(watchList);
                    addSuccessMessage(isnew, "emport.watchList.prefix", xid);
                }
            } catch (LocalizableJsonException e) {
                response.addGeneric("emport.watchList.prefix", xid, e);
            } catch (JsonException e) {
                response.addGeneric("emport.watchList.prefix", xid, getJsonExceptionMessage(e));
            }
        }
    }

    private void importMaintenanceEvent(JsonObject maintenanceEvent) {
        String xid = maintenanceEvent.getString("xid");
        if (xid == null) {
            response.addGeneric("emport.maintenanceEvent.xid");
        } else {
            MaintenanceEventVO vo = maintenanceEventDao
                    .getMaintenanceEvent(xid);
            if (vo == null) {
                vo = new MaintenanceEventVO();
                vo.setXid(xid);
            }

            try {
                reader.populateObject(vo, maintenanceEvent);

                // Now validate it. Use a new response object so we can
                // distinguish errors in this vo from other errors.
                DwrResponseI18n voResponse = new DwrResponseI18n();
                vo.validate(voResponse);
                if (!voResponse.isEmpty()) // Too bad. Copy the errors into the actual response.
                {
                    copyValidationMessages(voResponse,
                            "emport.maintenanceEvent.prefix", xid);
                } else {
                    // Sweet. Save it.
                    boolean isnew = vo.isNew();
                    Common.ctx.getRuntimeManager().saveMaintenanceEvent(vo);
                    addSuccessMessage(isnew, "emport.maintenanceEvent.prefix",
                            xid);
                }
            } catch (LocalizableJsonException e) {
                response.addGeneric("emport.maintenanceEvent.prefix", xid, e);
            } catch (JsonException e) {
                response.addGeneric("emport.maintenanceEvent.prefix", xid, getJsonExceptionMessage(e));
            }
        }
    }

    private void importScripts(JsonObject script) {

        String xid = script.getString("xid");
        if (xid == null) {
            response.addGeneric("emport.script.xid");
        } else {
            ScriptVO vo = scriptDao.getScript(xid);
            if (vo == null) {

                String typeStr = script.getString("type");
                ScriptVO.Type type = ScriptVO.Type.valueOfIgnoreCase(typeStr);
                vo = type.createScriptVO();
                vo.setXid(xid);
            }
            try {
                reader.populateObject(vo, script);
                // Now validate it. Use a new response object so we can
                // distinguish errors in this vo from other errors.
                DwrResponseI18n voResponse = new DwrResponseI18n();
                vo.validate(voResponse);
                if (!voResponse.isEmpty()) {
                    copyValidationMessages(voResponse, "emport.script.prefix",
                            xid);
                } // Too bad. Copy the errors into the actual response.
                else {
                    // Sweet. Save it.
                    boolean isnew = vo.isNew();
                    scriptDao.saveScript(vo);
                    addSuccessMessage(isnew, "emport.script.prefix", xid);
                }
            } catch (LocalizableJsonException e) {
                e.printStackTrace();
                response.addGeneric("emport.script.prefix", xid, e);
            } catch (JsonException e) {
                e.printStackTrace();
                response.addGeneric("emport.script.prefix", xid, getJsonExceptionMessage(e));
            }
        }
    }

    private void importPointValues(JsonObject json) {
        String pointXid = json.getString("pointXid");
        DataPointVO dp = DataPointDao.getInstance().getDataPoint(pointXid);
        if (dp == null) {
            response.addGeneric("emport.script.xid");
        } else {
            long time = json.getLong("timestamp");
            String value = json.getString("value");
            PointValueDao dao = PointValueDao.getInstance();
            PointValueTime pointValue = new PointValueTime(
                    MangoValue.stringToValue(value, dp.getDataType()), time);
            dao.savePointValue(dp.getId(), pointValue);
        }

    }

    private void importSystemSettings(JsonObject json) {
        try {
            reader.populateObject(new SystemSettingsJSONWrapper(), json);
        } catch (JsonException e) {
            e.printStackTrace();
            response.addGeneric("emport.systemSettingsFailed");
        }

    }

    private void copyValidationMessages(DwrResponseI18n voResponse, String key,
            String desc) {
        for (DwrMessageI18n msg : voResponse.getMessages()) {
            response.addGeneric(key, desc, AbstractLocalizer.localizeMessage(msg, bundle));
        }
    }

    private void addSuccessMessage(boolean isnew, String key, String desc) {
        //TODO AbstractLocalizer.localizeI18nKey should be done later wrap this in an LocalizableMsg????
        if (isnew) {
            response.addGeneric(key, desc, AbstractLocalizer.localizeI18nKey("emport.added", bundle));
        } else {
            response.addGeneric(key, desc, AbstractLocalizer.localizeI18nKey("emport.saved", bundle));
        }
    }

    private String getJsonExceptionMessage(JsonException e) {
        String msg = "'" + e.getMessage() + "'";
        Throwable t = e;
        while ((t = t.getCause()) != null) {
            if (t instanceof LocalizableJsonException) {
                msg += ", "
                        + AbstractLocalizer.localizeI18nKey("emport.causedBy", bundle)
                        + " '"
                        + AbstractLocalizer.localizeMessage((LocalizableJsonException) t, bundle) + "'";
            } else {
                msg += ", " + AbstractLocalizer.localizeI18nKey("emport.causedBy", bundle)
                        + " '" + t.getMessage() + "'";
            }
        }

        // Throwable t = e;
        // while (t.getCause() != null)
        // t = t.getCause();
        // String msg = msgPrefix + t.getMessage();
        return msg;
    }

}
