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

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.dao.DataSourceDao;
import br.org.scadabr.dao.EventDao;
import br.org.scadabr.i18n.I18NUtils;
import br.org.scadabr.i18n.LocalizableMessageParseException;
import br.org.scadabr.l10n.AbstractLocalizer;
import br.org.scadabr.rt.SchedulerPool;
import br.org.scadabr.rt.UserRT;
import br.org.scadabr.rt.event.type.EventSources;
import br.org.scadabr.timer.cron.EventRunnable;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.util.StringUtils;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.event.AlarmLevel;
import br.org.scadabr.vo.event.EventStatus;
import br.org.scadabr.vo.event.type.AuditEventKey;
import br.org.scadabr.vo.event.type.DataPointDetectorKey;
import br.org.scadabr.vo.event.type.SystemEventKey;
import com.serotonin.mango.rt.event.AlternateAcknowledgementSources;
import com.serotonin.mango.rt.event.EventInstance;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.rt.event.type.DataPointEventType;
import com.serotonin.mango.rt.event.type.DataSourceEventType;
import com.serotonin.mango.rt.event.type.EventType;
import com.serotonin.mango.rt.event.type.MaintenanceEventType;
import com.serotonin.mango.rt.event.type.ScheduledEventType;
import com.serotonin.mango.rt.event.type.SystemEventType;
import com.serotonin.mango.vo.UserComment;
import com.serotonin.mango.vo.event.EventHandlerVO;
import com.serotonin.mango.vo.event.EventTypeVO;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.sql.Types;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Date;
import java.util.List;
import java.util.Map;
import java.util.ResourceBundle;
import java.util.concurrent.ConcurrentHashMap;
import javax.inject.Inject;
import javax.inject.Named;
import org.springframework.dao.EmptyResultDataAccessException;
import org.springframework.jdbc.core.BatchPreparedStatementSetter;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.jdbc.core.PreparedStatementCreator;
import org.springframework.jdbc.core.RowMapper;
import org.springframework.transaction.TransactionStatus;
import org.springframework.transaction.support.TransactionCallbackWithoutResult;

@Named
public class EventDaoImpl extends BaseDao implements EventDao {

    private static final int MAX_PENDING_EVENTS = 100;

    @Inject
    private SchedulerPool schedulerPool;
    @Inject
    private DataSourceDao dataSourceDao;

    public EventDaoImpl() {
        super();
    }

    @Override
    public void saveEvent(EventInstance event) {
        if (event.isNew()) {
            insertEvent(event);
        } else {
            updateEvent(event);
        }
    }

    private void insertEvent(final EventInstance event) {

        final int id = doInsert(new PreparedStatementCreator() {

            final static String SQL_INSERT
                    = "insert into events\n"
                    + " (typeId, eventState, alarmLevel, typeRef1, typeRef2, typeRef3, fireTs, message, goneTs, ackTs)\n"
                    + "values\n"
                    + " (?,?,?,?,?,?,?,?,?,?)";

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                final EventType type = event.getEventType();
                ps.setInt(1, type.getEventSource().ordinal());
                ps.setInt(2, event.getEventState().ordinal());
                ps.setInt(3, event.getAlarmLevel().ordinal());
                switch (type.getEventSource()) {
                    case AUDIT: {
                        final AuditEventType et = (AuditEventType) type;
                        ps.setInt(4, et.getEventKey().getId());
                        ps.setInt(5, et.getReferenceId());
                        ps.setNull(6, Types.INTEGER);
                    }
                    break;
                    /*TODO                    case COMPOUND:
                     ps.setInt(4, ((CompoundEventTy) type).getAuditEventType().mangoDbId);
                     ps.setInt(5, type.getReferenceId2());
                        ps.setNull(6, Types.INTEGER);
                     break;
                     */ case DATA_POINT:
                        ps.setInt(4, ((DataPointEventType) type).getReferenceId1());
                        ps.setInt(5, ((DataPointEventType) type).getReferenceId2());
                        ps.setNull(6, Types.INTEGER);
                        break;
                    case DATA_SOURCE:
                        ps.setInt(4, ((DataSourceEventType) type).getDataSourceId());
                        ps.setInt(5, ((DataSourceEventType) type).getEventKey().getId());
                        ps.setNull(6, Types.INTEGER);
                        break;
                    case MAINTENANCE:
                        ps.setInt(4, ((MaintenanceEventType) type).getReferenceId1());
                        ps.setInt(5, ((MaintenanceEventType) type).getReferenceId2());
                        ps.setNull(6, Types.INTEGER);
                        break;
                    case PUBLISHER:
                        ps.setInt(4, type.getReferenceId1());
                        ps.setInt(5, type.getReferenceId2());
                        ps.setNull(6, Types.INTEGER);
                        break;
                    case SCHEDULED:
                        ps.setInt(4, ((ScheduledEventType) type).getReferenceId1());
                        ps.setInt(5, ((ScheduledEventType) type).getReferenceId2());
                        ps.setNull(6, Types.INTEGER);
                        break;
                    case SYSTEM: {
                        final SystemEventType et = (SystemEventType) type;
                        ps.setInt(4, et.getEventKey().getId());
                        ps.setInt(5, et.getReferenceId());
                        ps.setNull(6, Types.INTEGER);
                    }
                    break;
                    default:
                        throw new ImplementMeException();
                }
                ps.setLong(7, event.getFireTimestamp());
                ps.setString(8, I18NUtils.serialize(event.getMessage()));
                if (event.getGoneTimestamp() == 0) {
                    ps.setNull(9, Types.BIGINT);
                } else {
                    ps.setLong(9, event.getGoneTimestamp());
                }
                if (event.getAcknowledgedTimestamp()== 0) {
                    ps.setNull(10, Types.BIGINT);
                } else {
                    ps.setLong(10, event.getAcknowledgedTimestamp());
                }
                return ps;
            }
        });
        
        event.setId(id);
    }

    private static final String EVENT_UPDATE = "update events set goneTs=?, eventState=? where id=?";

    private void updateEvent(EventInstance event) {
        ejt.update(EVENT_UPDATE,
                new Object[]{event.getGoneTimestamp(), event.getEventState().ordinal(),
                    event.getId()});
        updateCache(event);
    }

    private static final String EVENT_ACK = "update events set ackTs=?, ackUserId=?, alternateAckSource=? where id=? and ackTs is null";
    private static final String USER_EVENT_ACK = "update userEvents set silenced=? where eventId=?";

    @Override
    public void ackEvent(int eventId, long ackTs, UserRT user, AlternateAcknowledgementSources alternateAckSource) {
        // Ack the event
        ejt.update(EVENT_ACK, new Object[]{ackTs, user == null ? null : user.getId(),
            alternateAckSource, eventId});
        // Silence the user events
        ejt.update(USER_EVENT_ACK, new Object[]{boolToChar(true), eventId});
        // Clear the cache
        clearCache();
    }

    private static final String USER_EVENTS_INSERT = "insert into userEvents (eventId, userId, silenced) values (?,?,?)";

    @Override
    public void insertUserEvents(final int eventId,
            final List<Integer> userIds, final boolean alarm) {
        ejt.batchUpdate(USER_EVENTS_INSERT, new BatchPreparedStatementSetter() {
            @Override
            public int getBatchSize() {
                return userIds.size();
            }

            @Override
            public void setValues(PreparedStatement ps, int i)
                    throws SQLException {
                ps.setInt(1, eventId);
                ps.setInt(2, userIds.get(i));
                ps.setString(3, boolToChar(!alarm));
            }
        });

        if (alarm) {
            userIds.stream().forEach((userId) -> {
                removeUserIdFromCache(userId);
            });
        }
    }

    private static final String BASIC_EVENT_SELECT
            = "select\n"
            + " e.id, e.typeId, e.eventState, e.alarmLevel, e.typeRef1, e.typeRef2, e.typeRef3, e.fireTs, e.message, e.goneTs,\n"
            + " e.ackTs, e.ackUserId, u.username, e.alternateAckSource\n"
            + "from events e\n"
            + "  left join users u on e.ackUserId = u.id\n";

    @Override
    public Collection<EventInstance> getActiveEvents() {
        List<EventInstance> results = ejt.query(BASIC_EVENT_SELECT
                + "where e.eventState=?",
                new EventInstanceRowMapper(), EventStatus.ACTIVE.ordinal());
        attachRelationalInfo(results);
        return results;
    }

    private static final String EVENT_SELECT_WITH_USER_DATA
            = "select\n"
            + " e.id, e.typeId, e.eventState, e.alarmLevel, e.typeRef1, e.typeRef2, e.typeRef3, e.fireTs, e.message, e.goneTs,\n"
            + " e.ackTs, e.ackUserId, u.username, e.alternateAckSource, ue.silenced\n"
            + "from events e\n"
            + "  left join users u on e.ackUserId=u.id\n"
            + "  left join userEvents ue on e.id=ue.eventId\n";

    public List<EventInstance> getEventsForDataPoint(int dataPointId, int userId) {
        List<EventInstance> results = ejt.query(EVENT_SELECT_WITH_USER_DATA
                + "where e.typeId=" + EventSources.DATA_POINT.ordinal()
                + "  and e.typeRef1=? " + "  and ue.userId=? "
                + "order by e.fireTs desc", new Object[]{dataPointId,
                    userId}, new UserEventInstanceRowMapper());
        attachRelationalInfo(results);
        return results;
    }
    
    public List<EventInstance> getPendingEventsForDataPoint(int dataPointId,
            int userId) {
        // Check the cache
        List<EventInstance> userEvents = getFromCache(userId);
        if (userEvents == null) {
            // This is a potentially long running query, so run it offline.
            userEvents = Collections.emptyList();
            addToCache(userId, userEvents);
            schedulerPool.execute(new UserPendingEventRetriever(userId));
        }

        List<EventInstance> list = null;
        for (EventInstance e : userEvents) {
            if (e.getEventType().getDataPointId() == dataPointId) {
                if (list == null) {
                    list = new ArrayList<>();
                }
                list.add(e);
            }
        }

        if (list == null) {
            return Collections.emptyList();
        }
        return list;
    }

    class UserPendingEventRetriever implements EventRunnable {

        private final int userId;

        UserPendingEventRetriever(int userId) {
            this.userId = userId;
        }

        @Override
        public void run() {
            addToCache(
                    userId,
                    getPendingEvents(EventSources.DATA_POINT, -1, userId));
        }
    }

    public List<EventInstance> getPendingEventsForDataSource(int dataSourceId,
            int userId) {
        return getPendingEvents(EventSources.DATA_SOURCE, dataSourceId, userId);
    }

    public List<EventInstance> getPendingEventsForPublisher(int publisherId,
            int userId) {
        return getPendingEvents(EventSources.PUBLISHER, publisherId,
                userId);
    }

    List<EventInstance> getPendingEvents(EventSources eventSource, int typeRef1, int userId) {
        Object[] params;
        StringBuilder sb = new StringBuilder();
        sb.append(EVENT_SELECT_WITH_USER_DATA);
        sb.append("where e.typeId=?");

        if (typeRef1 == -1) {
            params = new Object[]{eventSource.ordinal(), userId, boolToChar(true)};
        } else {
            sb.append("  and e.typeRef1=?");
            params = new Object[]{eventSource.ordinal(), typeRef1, userId, boolToChar(true)};
        }
        sb.append("  and ue.userId=? ");
        sb.append("  and (e.ackTs is null or (e.rtnApplicable=? and e.rtnTs is null and e.alarmLevel > 0)) ");
        sb.append("order by e.activeTs desc");

        List<EventInstance> results = ejt.query(sb.toString(), params,
                new UserEventInstanceRowMapper());
        attachRelationalInfo(results);
        return results;
    }

    @Override
    public Collection<EventInstance> getPendingEvents(final UserRT user) {
        List<EventInstance> results = ejt.query((Connection con) -> {
            PreparedStatement ps = con.prepareCall(EVENT_SELECT_WITH_USER_DATA
                    + "where ue.userId=? and e.ackTs is null order by e.fireTs desc");
            ps.setInt(1, user.getId());
            ps.setMaxRows(MAX_PENDING_EVENTS);
            return ps;
        }, new UserEventInstanceRowMapper());
        attachRelationalInfo(results);
        return results;
    }

    @Override
    public EventInstance getEventInstance(int eventId) {
        return ejt.queryForObject(BASIC_EVENT_SELECT + "where e.id=?",
                new EventInstanceRowMapper(), eventId);
    }

    public class EventInstanceRowMapper implements RowMapper<EventInstance> {

        @Override
        public EventInstance mapRow(ResultSet rs, int rowNum)
                throws SQLException {
            final AlarmLevel alarmLevel = AlarmLevel.values()[rs.getInt(4)];
            final EventStatus state = EventStatus.values()[rs.getInt(3)];
            final EventType type = createEventType(EventSources.values()[rs.getInt(2)], rs.getInt(5), rs.getInt(6), rs.getInt(7), alarmLevel);

            LocalizableMessage message;
            try {
                message = I18NUtils.deserialize(rs.getString(9));
            } catch (LocalizableMessageParseException e) {
                message = new LocalizableMessageImpl("common.default",
                        rs.getString(10));
            }

            EventInstance event = new EventInstance(type, rs.getLong(8), alarmLevel, state, rs.getLong(10), message);
            event.setId(rs.getInt(1));
            
            long ackTs = rs.getLong(11);
            if (!rs.wasNull()) {
                event.setAcknowledgedTimestamp(ackTs);
                event.setAcknowledgedByUserId(rs.getInt(12));
                if (!rs.wasNull()) {
                    event.setAcknowledgedByUsername(rs.getString(13));
                }
                event.setAlternateAckSource(AlternateAcknowledgementSources.values()[rs.getInt(14)]);
            }

            return event;
        }
    }

    class UserEventInstanceRowMapper extends EventInstanceRowMapper {

        @Override
        public EventInstance mapRow(ResultSet rs, int rowNum)
                throws SQLException {
            EventInstance event = super.mapRow(rs, rowNum);
            event.setSilenced(charToBool(rs.getString(15)));
            if (!rs.wasNull()) {
                event.setUserNotified(true);
            }
            return event;
        }
    }

    EventType createEventType(final EventSources eventSource, final int refId1, final int refId2,  final int refId3, final AlarmLevel alarmLevel)
            throws SQLException {
        switch (eventSource) {
            case DATA_POINT:
                return new DataPointEventType(refId1, DataPointDetectorKey.fromId(refId2), alarmLevel);
            case DATA_SOURCE:
                return dataSourceDao.getEventType(refId1, refId2);
            case SYSTEM:
                return new SystemEventType(SystemEventKey.fromId(refId1), refId2); // TODO set alarmlevel from old db value???
            case COMPOUND:
                throw new ImplementMeException();
                //TODO return new CompoundDetectorEventType(refId1);
            case SCHEDULED:
                throw new ImplementMeException();
                //TODO return new ScheduledEventType(refId1);
            case PUBLISHER:
                throw new ImplementMeException();
                //TODO return new PublisherEventType(refId1, refId2);
            case AUDIT:
                return new AuditEventType(AuditEventKey.fromId(refId1), refId2, null); // TODO we do not know the user who did this. alarmlevel??
            case MAINTENANCE:
                throw new ImplementMeException();
                //TODO return new MaintenanceEventType(refId1);
            default:
                throw new ShouldNeverHappenException("Unknown eventSource: " + eventSource);
        }
    }

    private void attachRelationalInfo(List<EventInstance> list) {
        list.stream().forEach((e) -> {
            attachRelationalInfo(e);
        });
    }

    private static final String EVENT_COMMENT_SELECT = UserCommentRowMapper.USER_COMMENT_SELECT
            + "where uc.commentType= "
            + UserComment.TYPE_EVENT
            + " and uc.typeKey=? " + "order by uc.ts";

    void attachRelationalInfo(EventInstance event) {
        event.setEventComments(ejt.query(EVENT_COMMENT_SELECT,
                new Object[]{event.getId()}, new UserCommentRowMapper()));
    }

    public EventInstance insertEventComment(int eventId, UserComment comment) {
        throw new ImplementMeException();
/*        userDao.insertUserComment(UserComment.TYPE_EVENT, eventId,
                comment);
        return getEventInstance(eventId);
*/
    }

    @Override
    public int purgeEventsBefore(final long time) {
        // Find a list of event ids with no remaining acknowledgements pending.
        final JdbcTemplate ejt2 = ejt;
        int count = getTransactionTemplate().execute((TransactionStatus status) -> {
            int count1 = ejt2
                    .update("delete from events "
                            + "where activeTs < ? "
                            + "  and ackTs is not null "
                            + "  and (rtnApplicable=? or (rtnApplicable=? and rtnTs is not null))",
                            new Object[]{time, boolToChar(false),
                                boolToChar(true)});
            // Delete orphaned user comments.
            ejt2.update("delete from userComments where commentType="
                    + UserComment.TYPE_EVENT
                    + "  and typeKey not in (select id from events)");
            return count1;
        });

        clearCache();

        return count;
    }

    public int getEventCount() {
        return ejt.queryForObject("select count(*) from events", Integer.class);
    }

    public List<EventInstance> search(int eventId, int eventSourceType,
            EventStatus status, int alarmLevel, final String[] keywords, int userId,
            final ResourceBundle bundle, final int from, final int to,
            final Date date) {
        return search(eventId, eventSourceType, status, alarmLevel, keywords,
                -1, -1, userId, bundle, from, to, date);
    }

    public List<EventInstance> search(int eventId, int eventSourceType,
            EventStatus status, int alarmLevel, final String[] keywords,
            long dateFrom, long dateTo, int userId,
            final ResourceBundle bundle, final int from, final int to,
            final Date date) {
        List<String> where = new ArrayList<>();
        List<Object> params = new ArrayList<>();

        StringBuilder sql = new StringBuilder();
        sql.append(EVENT_SELECT_WITH_USER_DATA);
        sql.append("where ue.userId=?");
        params.add(userId);

        if (eventId != 0) {
            where.add("e.id=?");
            params.add(eventId);
        }

        if (eventSourceType != -1) {
            where.add("e.typeId=?");
            params.add(eventSourceType);
        }

        if (null != status) {
            switch (status) {
                case ACTIVE:
                    where.add("e.rtnApplicable=? and e.rtnTs is null");
                    params.add(boolToChar(true));
                    break;
                /*                case INACTIVE:
                 where.add("e.rtnApplicable=? and e.rtnTs is not null");
                 params.add(boolToChar(true));
                 break;
                 */ case STATELESS:
                    where.add("e.rtnApplicable=?");
                    params.add(boolToChar(false));
                    break;
            }
        }

        if (alarmLevel != -1) {
            where.add("e.alarmLevel=?");
            params.add(alarmLevel);
        }

        if (dateFrom != -1) {
            where.add("activeTs>=?");
            params.add(dateFrom);
        }

        if (dateTo != -1) {
            where.add("activeTs<?");
            params.add(dateTo);
        }

        if (!where.isEmpty()) {
            where.stream().forEach((s) -> {
                sql.append(" and ");
                sql.append(s);
            });
        }
        sql.append(" order by e.activeTs desc");

        final List<EventInstance> results = new ArrayList<>();
        final UserEventInstanceRowMapper rowMapper = new UserEventInstanceRowMapper();

        final int[] data = new int[2];

        ejt.query(sql.toString(), params.toArray(), (ResultSet rs) -> {
            int row = 0;
            long dateTs = date == null ? -1 : date.getTime();
            int startRow1 = -1;
            while (rs.next()) {
                EventInstance e = rowMapper.mapRow(rs, 0);
                attachRelationalInfo(e);
                boolean add = true;
                if (keywords != null) {
                    // Do the text search. If the instance has a match, put
                    // it in the result. Otherwise ignore.
                    StringBuilder text = new StringBuilder();
                    text.append(AbstractLocalizer.localizeMessage(e.getMessage(), bundle));
                    for (UserComment comment : e.getEventComments()) {
                        text.append(' ').append(comment.getComment());
                    }
                    
                    String[] values = text.toString().split("\\s+");
                    
                    for (String keyword : keywords) {
                        if (keyword.startsWith("-")) {
                            if (StringUtils.globWhiteListMatchIgnoreCase(
                                    values, keyword.substring(1))) {
                                add = false;
                                break;
                            }
                        } else {
                            if (!StringUtils.globWhiteListMatchIgnoreCase(
                                    values, keyword)) {
                                add = false;
                                break;
                            }
                        }
                    }
                }
                if (add) {
                    if (date != null) {
                        if (e.getFireTimestamp() <= dateTs
                                && results.size() < to - from) {
                            if (startRow1 == -1) {
                                startRow1 = row;
                            }
                            results.add(e);
                        }
                    } else if (row >= from && row < to) {
                        results.add(e);
                    }
                    row++;
                }
            }
            data[0] = row;
            data[1] = startRow1;
            return null;
        });

        searchRowCount = data[0];
        startRow = data[1];

        return results;
    }

    private int searchRowCount;
    private int startRow;

    public int getSearchRowCount() {
        return searchRowCount;
    }

    public int getStartRow() {
        return startRow;
    }

    //
    // /
    // / Event handlers
    // /
    //
    public String generateUniqueXid() {
        return generateUniqueXid(EventHandlerVO.XID_PREFIX, "eventHandlers");
    }

    public boolean isXidUnique(String xid, int excludeId) {
        return isXidUnique(xid, excludeId, "eventHandlers");
    }

    public EventType getEventHandlerType(int handlerId) {
        return ejt.queryForObject("select eventTypeId, eventTypeRef1, eventTypeRef2, alarmLevel from eventHandlers where id=?", (ResultSet rs, int rowNum) -> {
            final AlarmLevel alarmLevel = AlarmLevel.values()[rs.getInt(4)];
            throw new ImplementMeException();
            //TODO return createEventType(EventSources.values()[rs.getInt(1)], rs.getInt(2), rs.getInt(3), alarmLevel);
        }, handlerId);
    }

    @Override
    public List<EventHandlerVO> getEventHandlers(EventType type) {
        switch (type.getEventSource()) {
            case AUDIT: {
                final AuditEventType et = (AuditEventType) type;
                return getEventHandlers(et.getEventSource(), et.getEventKey().getId(), et.getReferenceId());
            }
            case DATA_POINT: {
                final DataPointEventType et = (DataPointEventType) type;
                return getEventHandlers(et.getEventSource(), et.getReferenceId1(), et.getReferenceId2());
            }
            case DATA_SOURCE: {
                final DataSourceEventType et = (DataSourceEventType) type;
                return getEventHandlers(et.getEventSource(), et.getDataSourceId(), et.getEventKey().getId());
            }
            case MAINTENANCE: {
                final MaintenanceEventType et = (MaintenanceEventType) type;
                return getEventHandlers(et.getEventSource(), et.getReferenceId1(), et.getReferenceId2());
            }
            case PUBLISHER: {
                return getEventHandlers(type.getEventSource(), type.getReferenceId1(), type.getReferenceId2());
            }
            case SCHEDULED: {
                final ScheduledEventType et = (ScheduledEventType) type;
                return getEventHandlers(et.getEventSource(), et.getReferenceId1(), et.getReferenceId2());
            }
            case SYSTEM: {
                final SystemEventType et = (SystemEventType) type;
                return getEventHandlers(et.getEventSource(), et.getEventKey().getId(), et.getReferenceId());
            }
            default:
                throw new ShouldNeverHappenException("Eventtype not supported");
        }
    }

    public List<EventHandlerVO> getEventHandlers(EventTypeVO type) {
        return getEventHandlers(type.getEventSource(), type.getTypeRef1(),
                type.getTypeRef2());
    }

    public List<EventHandlerVO> getEventHandlers() {
        return ejt.query(EVENT_HANDLER_SELECT, new EventHandlerRowMapper());
    }

    /**
     * Note: eventHandlers.eventTypeRef2 matches on both the given ref2 and 0.
     * This is to allow a single set of event handlers to be defined for user
     * login events, rather than have to individually define them for each user.
     */
    private List<EventHandlerVO> getEventHandlers(EventSources eventSource, int ref1, int ref2) {
        return ejt.query(EVENT_HANDLER_SELECT
                + "where eventTypeId=? and eventTypeRef1=? "
                + "  and (eventTypeRef2=? or eventTypeRef2=0)", new Object[]{
                    eventSource.ordinal(), ref1, ref2}, new EventHandlerRowMapper());
    }

    public EventHandlerVO getEventHandler(int eventHandlerId) {
        return ejt.queryForObject(EVENT_HANDLER_SELECT + "where id=?",
                new Object[]{eventHandlerId}, new EventHandlerRowMapper());
    }

    public EventHandlerVO getEventHandler(String xid) {
        try {
            return ejt.queryForObject(EVENT_HANDLER_SELECT + "where xid=?",
                    new EventHandlerRowMapper(), xid);
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

    private static final String EVENT_HANDLER_SELECT = "select id, xid, alias, data from eventHandlers ";

    class EventHandlerRowMapper implements RowMapper<EventHandlerVO> {

        @Override
        public EventHandlerVO mapRow(ResultSet rs, int rowNum)
                throws SQLException {
            EventHandlerVO h = (EventHandlerVO) SerializationHelper
                    .readObject(rs.getBlob(4).getBinaryStream());
            h.setId(rs.getInt(1));
            h.setXid(rs.getString(2));
            h.setAlias(rs.getString(3));
            return h;
        }
    }

    public EventHandlerVO saveEventHandler(final EventType type,
            final EventHandlerVO handler) {
        if (type == null) {
            throw new ShouldNeverHappenException("saveEventHandler EventType is null");
//            return saveEventHandler(0, 0, 0, handler);
        }
        switch (type.getEventSource()) {
            case AUDIT: {
                final AuditEventType et = (AuditEventType) type;
                return saveEventHandler(et.getEventSource(), et.getEventKey().getId(), et.getReferenceId(), handler);
            }
            case DATA_POINT: {
                final DataPointEventType et = (DataPointEventType) type;
                return saveEventHandler(et.getEventSource(), et.getReferenceId1(), et.getReferenceId2(), handler);
            }
            case DATA_SOURCE: {
                final DataSourceEventType et = (DataSourceEventType) type;
                return saveEventHandler(et.getEventSource(), et.getDataSourceId(), et.getEventKey().getId(), handler);
            }
            case MAINTENANCE: {
                final MaintenanceEventType et = (MaintenanceEventType) type;
                return saveEventHandler(et.getEventSource(), et.getReferenceId1(), et.getReferenceId2(), handler);
            }
            case PUBLISHER: {
//                final PublisherEventType et = (PublisherEventType) type;
                return saveEventHandler(type.getEventSource(), type.getReferenceId1(), type.getReferenceId2(), handler);
            }
            case SCHEDULED: {
                final ScheduledEventType et = (ScheduledEventType) type;
                return saveEventHandler(et.getEventSource(), et.getReferenceId1(), et.getReferenceId2(), handler);
            }
            case SYSTEM: {
                final SystemEventType et = (SystemEventType) type;
                return saveEventHandler(et.getEventSource(), et.getEventKey().getId(), et.getReferenceId(), handler);
            }
            default:
                throw new ShouldNeverHappenException("Eventtype not supported");
        }
    }

    public EventHandlerVO saveEventHandler(final EventTypeVO type,
            final EventHandlerVO handler) {
        if (type == null) {
            throw new ShouldNeverHappenException("saveEventHandler EventTypeVO is null");
//            return saveEventHandler(0, 0, 0, handler);
        }
        return saveEventHandler(type.getEventSource(), type.getTypeRef1(),
                type.getTypeRef2(), handler);
    }

    private EventHandlerVO saveEventHandler(final EventSources evetnSource,
            final int typeRef1, final int typeRef2, final EventHandlerVO handler) {
        getTransactionTemplate().execute(
                new TransactionCallbackWithoutResult() {
                    @Override
                    protected void doInTransactionWithoutResult(
                            TransactionStatus status) {
                                if (handler.isNew()) {
                                    insertEventHandler(evetnSource, typeRef1, typeRef2,
                                            handler);
                                } else {
                                    updateEventHandler(handler);
                                }
                            }
                });
        return getEventHandler(handler.getId());
    }

    void insertEventHandler(final EventSources eventSource, final int typeRef1, final int typeRef2,
            final EventHandlerVO handler) {
        handler.setId(doInsert(
                new PreparedStatementCreator() {

                    final static String SQL_INSERT = "insert into eventHandlers (xid, alias, eventTypeId, eventTypeRef1, eventTypeRef2, data) values (?,?,?,?,?,?)";

                    @Override
                    public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                        PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                        ps.setString(1, handler.getXid());
                        ps.setString(2, handler.getAlias());
                        ps.setInt(3, eventSource.ordinal());
                        ps.setInt(4, typeRef1);
                        ps.setInt(5, typeRef2);
                        ps.setBlob(6, SerializationHelper.writeObject(handler));
                        return ps;
                    }
                }));

        AuditEventType.raiseAddedEvent(AuditEventKey.EVENT_HANDLER,
                handler);
    }

    void updateEventHandler(final EventHandlerVO handler) {
        EventHandlerVO old = getEventHandler(handler.getId());

        ejt.update((Connection con) -> {
            final PreparedStatement ps = con.prepareStatement("update eventHandlers set xid=?, alias=?, data=? where id=?");
            ps.setString(1, handler.getXid());
            ps.setString(2, handler.getAlias());
            ps.setBlob(3, SerializationHelper.writeObject(handler));
            ps.setInt(4, handler.getId());
            return ps;
        });

        AuditEventType.raiseChangedEvent(AuditEventKey.EVENT_HANDLER,
                old, handler);
    }

    public void deleteEventHandler(final int handlerId) {
        EventHandlerVO handler = getEventHandler(handlerId);
        ejt.update("delete from eventHandlers where id=?",
                new Object[]{handlerId});
        AuditEventType.raiseDeletedEvent(AuditEventKey.EVENT_HANDLER,
                handler);
    }

    //
    // /
    // / User alarms
    // /
    //
    private static final String SILENCED_SELECT = "select ue.silenced "
            + "from events e " + "  join userEvents ue on e.id=ue.eventId "
            + "where e.id=? " + "  and ue.userId=? " + "  and e.ackTs is null";

    public boolean toggleSilence(int eventId, int userId) {
        String result = ejt.queryForObject(SILENCED_SELECT, new Object[]{eventId, userId}, String.class);
        if (result == null) {
            return true;
        }

        boolean silenced = !charToBool(result);
        ejt.update(
                "update userEvents set silenced=? where eventId=? and userId=?",
                new Object[]{boolToChar(silenced), eventId, userId});
        return silenced;
    }

    public int getHighestUnsilencedAlarmLevel(int userId) {
        return ejt.queryForObject(
                "select max(e.alarmLevel) from userEvents u "
                + "  join events e on u.eventId=e.id "
                + "where u.silenced=? and u.userId=?", 
                Integer.class,
                boolToChar(false), userId);
    }

    //
    // /
    // / Pending event caching
    // /
    //
    static class PendingEventCacheEntry {

        private final List<EventInstance> list;
        private final long createTime;

        public PendingEventCacheEntry(List<EventInstance> list) {
            this.list = list;
            createTime = System.currentTimeMillis();
        }

        public List<EventInstance> getList() {
            return list;
        }

        public boolean hasExpired() {
            return System.currentTimeMillis() - createTime > CACHE_TTL;
        }
    }

    private static final Map<Integer, PendingEventCacheEntry> pendingEventCache = new ConcurrentHashMap<>();

    private static final long CACHE_TTL = 300000; // 5 minutes

    public static List<EventInstance> getFromCache(int userId) {
        PendingEventCacheEntry entry = pendingEventCache.get(userId);
        if (entry == null) {
            return null;
        }
        if (entry.hasExpired()) {
            pendingEventCache.remove(userId);
            return null;
        }
        return entry.getList();
    }

    public static void addToCache(int userId, List<EventInstance> list) {
        pendingEventCache.put(userId, new PendingEventCacheEntry(list));
    }

    public static void updateCache(EventInstance event) {
        if (event.isAlarm()
                && event.getEventType().getEventSource() == EventSources.DATA_POINT) {
            pendingEventCache.clear();
        }
    }

    public static void removeUserIdFromCache(int userId) {
        pendingEventCache.remove(userId);
    }

    public static void clearCache() {
        pendingEventCache.clear();
    }

}
