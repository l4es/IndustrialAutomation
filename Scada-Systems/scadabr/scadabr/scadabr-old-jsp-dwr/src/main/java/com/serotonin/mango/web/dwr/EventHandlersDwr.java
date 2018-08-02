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

import br.org.scadabr.rt.event.type.EventSources;
import br.org.scadabr.utils.TimePeriods;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.ResourceBundle;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.directwebremoting.WebContextFactory;

import com.serotonin.mango.Common;
import com.serotonin.mango.db.dao.CompoundEventDetectorDao;
import com.serotonin.mango.db.dao.DataPointDao;
import com.serotonin.mango.db.dao.DataSourceDao;
import com.serotonin.mango.db.dao.EventDao;
import com.serotonin.mango.db.dao.MailingListDao;
import com.serotonin.mango.db.dao.MaintenanceEventDao;
import com.serotonin.mango.db.dao.PublisherDao;
import com.serotonin.mango.db.dao.ScheduledEventDao;
import com.serotonin.mango.db.dao.UserDao;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.rt.event.type.SystemEventType;
import com.serotonin.mango.rt.maint.work.ProcessWorkItem;
import com.serotonin.mango.view.text.TextRenderer;
import com.serotonin.mango.vo.DataPointExtendedNameComparator;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.User;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.event.CompoundEventDetectorVO;
import com.serotonin.mango.vo.event.EventHandlerVO;
import com.serotonin.mango.vo.event.EventTypeVO;
import com.serotonin.mango.vo.event.MaintenanceEventVO;
import com.serotonin.mango.vo.event.PointEventDetectorVO;
import com.serotonin.mango.vo.event.ScheduledEventVO;
import com.serotonin.mango.vo.permission.Permissions;
import com.serotonin.mango.vo.publish.PublishedPointVO;
import com.serotonin.mango.vo.publish.PublisherVO;
import com.serotonin.mango.web.dwr.beans.DataPointBean;
import com.serotonin.mango.web.dwr.beans.EventSourceBean;
import com.serotonin.mango.vo.event.RecipientListEntryBean;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import javax.inject.Inject;

public class EventHandlersDwr extends BaseDwr {

    private static final Log LOG = LogFactory.getLog(EventHandlersDwr.class);

    @Inject
    private ScheduledEventDao scheduledEventDao;
    @Inject
    private CompoundEventDetectorDao compoundEventDetectorDao;
    @Inject
    private DataSourceDao dataSourceDao;
    @Inject
    private PublisherDao publisherDao;
    @Inject
    private MaintenanceEventDao maintenanceEventDao;
    @Inject
    private MailingListDao mailingListDao;

    private final ResourceBundle setPointSnippetMap = ResourceBundle
            .getBundle("setPointSnippetMap");

    public Map<String, Object> getInitData() {
        User user = Common.getUser();
        Permissions.ensureDataSourcePermission(user);

        Map<String, Object> model = new HashMap<>();

        // Get the data points
        List<DataPointBean> allPoints = new ArrayList<>();
        List<EventSourceBean> dataPoints = new ArrayList<>();
        List<DataPointVO> dps = dataPointDao.getDataPoints(
                DataPointExtendedNameComparator.instance, true);
        for (DataPointVO dp : dps) {
            if (!Permissions
                    .hasDataSourcePermission(user, dp.getDataSourceId())) {
                continue;
            }

            allPoints.add(new DataPointBean(dp));

            if (dp.getEventDetectors().size() > 0) {
                EventSourceBean source = new EventSourceBean();
                source.setId(dp.getId());
                source.setName(dp.getExtendedName());

                for (PointEventDetectorVO ped : dp.getEventDetectors()) {
                    EventTypeVO dpet = ped.getEventType();
                    dpet.setHandlers(eventDao.getEventHandlers(dpet));
                    source.getEventTypes().add(dpet);
                }

                dataPoints.add(source);
            }
        }

        // Get the scheduled events
        List<EventTypeVO> scheduledEvents = new ArrayList<>();
        List<ScheduledEventVO> ses = scheduledEventDao.getScheduledEvents();
        for (ScheduledEventVO se : ses) {
            EventTypeVO et = se.getEventType();
            et.setHandlers(eventDao.getEventHandlers(et));
            scheduledEvents.add(et);
        }
        model.put("scheduledEvents", scheduledEvents);

        // Get the compound event detectors
        List<EventTypeVO> compoundEvents = new ArrayList<>();
        List<CompoundEventDetectorVO> ceds = compoundEventDetectorDao.getCompoundEventDetectors();
        for (CompoundEventDetectorVO ced : ceds) {
            EventTypeVO et = ced.getEventType();
            et.setHandlers(eventDao.getEventHandlers(et));
            compoundEvents.add(et);
        }
        model.put("compoundEvents", compoundEvents);

        // Get the data sources
        List<EventSourceBean> dataSources = new ArrayList<>();
        for (DataSourceVO<?> ds : dataSourceDao.getDataSources()) {
            if (!Permissions.hasDataSourcePermission(user, ds.getId())) {
                continue;
            }

            if (ds.getEventTypes().size() > 0) {
                EventSourceBean source = new EventSourceBean();
                source.setId(ds.getId());
                source.setName(ds.getName());

                for (EventTypeVO dset : ds.getEventTypes()) {
                    dset.setHandlers(eventDao.getEventHandlers(dset));
                    source.getEventTypes().add(dset);
                }

                dataSources.add(source);
            }
        }

        if (Permissions.hasAdmin(user)) {
            // Get the publishers
            List<EventSourceBean> publishers = new ArrayList<>();
            for (PublisherVO<? extends PublishedPointVO> p : publisherDao.getPublishers(new PublisherDao.PublisherNameComparator())) {
                if (p.getEventTypes().size() > 0) {
                    EventSourceBean source = new EventSourceBean();
                    source.setId(p.getId());
                    source.setName(p.getName());

                    for (EventTypeVO pet : p.getEventTypes()) {
                        pet.setHandlers(eventDao.getEventHandlers(pet));
                        source.getEventTypes().add(pet);
                    }

                    publishers.add(source);
                }
            }
            model.put("publishers", publishers);

            // Get the maintenance events
            List<EventTypeVO> maintenanceEvents = new ArrayList<>();
            List<MaintenanceEventVO> mes = maintenanceEventDao.getMaintenanceEvents();
            for (MaintenanceEventVO me : mes) {
                EventTypeVO et = me.getEventType();
                et.setHandlers(eventDao.getEventHandlers(et));
                maintenanceEvents.add(et);
            }
            model.put("maintenanceEvents", maintenanceEvents);

            // Get the system events
            List<EventTypeVO> systemEvents = new ArrayList<>();
            for (EventTypeVO sets : SystemEventType.SYSTEM_EVENT_TYPES.values()) {
                sets.setHandlers(eventDao.getEventHandlers(sets));
                systemEvents.add(sets);
            }
            model.put("systemEvents", systemEvents);

            // Get the audit events
            List<EventTypeVO> auditEvents = new ArrayList<>();
            for (EventTypeVO aets : AuditEventType.AUDIT_EVENT_TYPES.values()) {
                aets.setHandlers(eventDao.getEventHandlers(aets));
                auditEvents.add(aets);
            }
            model.put("auditEvents", auditEvents);
        }

        // Get the mailing lists.
        model.put("mailingLists", mailingListDao.getMailingLists());

        // Get the users.
        model.put("users", userDao.getUsers());

        model.put("allPoints", allPoints);
        model.put("dataPoints", dataPoints);
        model.put("dataSources", dataSources);

        return model;
    }

    public String createSetValueContent(int pointId, String valueStr,
            String idSuffix) {
        DataPointVO pointVO = dataPointDao.getDataPoint(pointId);
        Permissions.ensureDataSourcePermission(Common.getUser(),
                pointVO.getDataSourceId());

        MangoValue value = MangoValue.stringToValue(valueStr, pointVO.getDataType());

        Map<String, Object> model = new HashMap<>();
        model.put("point", pointVO);
        model.put("idSuffix", idSuffix);
        model.put("text",
                pointVO.getTextRenderer()
                .getText(value, TextRenderer.HINT_FULL));
        model.put("rawText",
                pointVO.getTextRenderer().getText(value, TextRenderer.HINT_RAW));

        String snippet = setPointSnippetMap.getString(pointVO.getTextRenderer()
                .getClass().getName());
        return generateContent(WebContextFactory.get().getHttpServletRequest(),
                snippet, model);
    }

    public DwrResponseI18n saveSetPointEventHandler(EventSources eventSource,
            int eventTypeRef1, int eventTypeRef2, int handlerId, String xid,
            String alias, boolean disabled, int targetPointId,
            int activeAction, String activeValueToSet, int activePointId,
            int inactiveAction, String inactiveValueToSet, int inactivePointId) {
        EventHandlerVO handler = new EventHandlerVO();
        handler.setHandlerType(EventHandlerVO.TYPE_SET_POINT);
        handler.setTargetPointId(targetPointId);
        handler.setActiveAction(activeAction);
        handler.setActiveValueToSet(activeValueToSet);
        handler.setActivePointId(activePointId);
        handler.setInactiveAction(inactiveAction);
        handler.setInactiveValueToSet(inactiveValueToSet);
        handler.setInactivePointId(inactivePointId);
        return save(eventSource, eventTypeRef1, eventTypeRef2, handler,
                handlerId, xid, alias, disabled);
    }

    public DwrResponseI18n saveEmailEventHandler(EventSources eventSource,
            int eventTypeRef1, int eventTypeRef2, int handlerId, String xid,
            String alias, boolean disabled,
            List<RecipientListEntryBean> activeRecipients,
            boolean sendEscalation, TimePeriods escalationDelayType,
            int escalationDelay,
            List<RecipientListEntryBean> escalationRecipients,
            boolean sendInactive, boolean inactiveOverride,
            List<RecipientListEntryBean> inactiveRecipients) {
        EventHandlerVO handler = new EventHandlerVO();
        handler.setHandlerType(EventHandlerVO.TYPE_EMAIL);
        handler.setActiveRecipients(activeRecipients);
        handler.setSendEscalation(sendEscalation);
        handler.setEscalationDelayType(escalationDelayType);
        handler.setEscalationDelay(escalationDelay);
        handler.setEscalationRecipients(escalationRecipients);
        handler.setSendInactive(sendInactive);
        handler.setInactiveOverride(inactiveOverride);
        handler.setInactiveRecipients(inactiveRecipients);
        return save(eventSource, eventTypeRef1, eventTypeRef2, handler,
                handlerId, xid, alias, disabled);
    }

    public DwrResponseI18n saveProcessEventHandler(EventSources eventSource,
            int eventTypeRef1, int eventTypeRef2, int handlerId, String xid,
            String alias, boolean disabled, String activeProcessCommand,
            String inactiveProcessCommand) {
        EventHandlerVO handler = new EventHandlerVO();
        handler.setHandlerType(EventHandlerVO.TYPE_PROCESS);
        handler.setActiveProcessCommand(activeProcessCommand);
        handler.setInactiveProcessCommand(inactiveProcessCommand);
        return save(eventSource, eventTypeRef1, eventTypeRef2, handler,
                handlerId, xid, alias, disabled);
    }

    public DwrResponseI18n saveScriptEventHandler(EventSources eventSource,
            int eventTypeRef1, int eventTypeRef2, int handlerId, String xid,
            String alias, boolean disabled, int activeScriptCommand,
            int inactiveScriptCommand) {
        EventHandlerVO handler = new EventHandlerVO();
        handler.setHandlerType(EventHandlerVO.TYPE_SCRIPT);
        handler.setActiveScriptCommand(activeScriptCommand);
        handler.setInactiveScriptCommand(inactiveScriptCommand);
        return save(eventSource, eventTypeRef1, eventTypeRef2, handler,
                handlerId, xid, alias, disabled);
    }

    private DwrResponseI18n save(EventSources eventSource, int eventTypeRef1,
            int eventTypeRef2, EventHandlerVO vo, int handlerId, String xid,
            String alias, boolean disabled) {
        EventTypeVO type = new EventTypeVO(eventSource, eventTypeRef1,
                eventTypeRef2);
        Permissions.ensureEventTypePermission(Common.getUser(), type);

        vo.setId(handlerId);
        vo.setXid(xid.isEmpty() ? eventDao.generateUniqueXid() : xid);
        vo.setAlias(alias);
        vo.setDisabled(disabled);

        DwrResponseI18n response = new DwrResponseI18n();
        vo.validate(response);

        if (response.isEmpty()) {
            eventDao.saveEventHandler(type, vo);
            response.addData("handler", vo);
        }

        return response;
    }

    public void deleteEventHandler(int handlerId) {
        Permissions.ensureEventTypePermission(Common.getUser(),
                eventDao.getEventHandlerType(handlerId));
        eventDao.deleteEventHandler(handlerId);
    }

    public LocalizableMessage testProcessCommand(String command) {
        if (command.isEmpty()) {
            return null;
        }

        try {
            ProcessWorkItem.executeProcessCommand(command);
            return new LocalizableMessageImpl("eventHandlers.commandTest.result");
        } catch (IOException e) {
            LOG.warn("Process error", e);
            return new LocalizableMessageImpl("common.default", e.getMessage());
        }
    }

    /**
     * @return the scheduledEventDao
     */
    public ScheduledEventDao getScheduledEventDao() {
        return scheduledEventDao;
    }

    /**
     * @param scheduledEventDao the scheduledEventDao to set
     */
    public void setScheduledEventDao(ScheduledEventDao scheduledEventDao) {
        this.scheduledEventDao = scheduledEventDao;
    }

    /**
     * @return the compoundEventDetectorDao
     */
    public CompoundEventDetectorDao getCompoundEventDetectorDao() {
        return compoundEventDetectorDao;
    }

    /**
     * @param compoundEventDetectorDao the compoundEventDetectorDao to set
     */
    public void setCompoundEventDetectorDao(CompoundEventDetectorDao compoundEventDetectorDao) {
        this.compoundEventDetectorDao = compoundEventDetectorDao;
    }

    /**
     * @return the dataSourceDao
     */
    public DataSourceDao getDataSourceDao() {
        return dataSourceDao;
    }

    /**
     * @param dataSourceDao the dataSourceDao to set
     */
    public void setDataSourceDao(DataSourceDao dataSourceDao) {
        this.dataSourceDao = dataSourceDao;
    }

    /**
     * @return the publisherDao
     */
    public PublisherDao getPublisherDao() {
        return publisherDao;
    }

    /**
     * @param publisherDao the publisherDao to set
     */
    public void setPublisherDao(PublisherDao publisherDao) {
        this.publisherDao = publisherDao;
    }

    /**
     * @return the maintenanceEventDao
     */
    public MaintenanceEventDao getMaintenanceEventDao() {
        return maintenanceEventDao;
    }

    /**
     * @param maintenanceEventDao the maintenanceEventDao to set
     */
    public void setMaintenanceEventDao(MaintenanceEventDao maintenanceEventDao) {
        this.maintenanceEventDao = maintenanceEventDao;
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
