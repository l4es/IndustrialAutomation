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

import br.org.scadabr.dao.ScheduledEventDao;
import br.org.scadabr.rt.event.type.EventSources;
import br.org.scadabr.vo.event.AlarmLevel;
import br.org.scadabr.vo.event.type.AuditEventKey;
import br.org.scadabr.vo.event.type.ScheduledEventKey;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.List;

import org.springframework.transaction.TransactionStatus;
import org.springframework.transaction.support.TransactionCallbackWithoutResult;

import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.vo.event.ScheduledEventVO;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.Statement;
import javax.inject.Named;
import org.springframework.dao.EmptyResultDataAccessException;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.jdbc.core.PreparedStatementCreator;
import org.springframework.jdbc.core.RowMapper;

/**
 * @author Matthew Lohbihler
 *
 */
@Named
public class ScheduledEventDaoImpl extends BaseDao implements ScheduledEventDao {

    private static final String SCHEDULED_EVENT_SELECT = "select id, xid, alias, alarmLevel, scheduleType, "
            + "  returnToNormal, disabled, activeYear, activeMonth, activeDay, activeHour, activeMinute, activeSecond, "
            + "  activeCron, inactiveYear, inactiveMonth, inactiveDay, inactiveHour, inactiveMinute, inactiveSecond, "
            + "inactiveCron from scheduledEvents ";

    public ScheduledEventDaoImpl() {
        super();
    }

    public String generateUniqueXid() {
        return generateUniqueXid(ScheduledEventVO.XID_PREFIX, "scheduledEvents");
    }

    public boolean isXidUnique(String xid, int excludeId) {
        return isXidUnique(xid, excludeId, "scheduledEvents");
    }

    public List<ScheduledEventVO> getScheduledEvents() {
        return ejt.query(SCHEDULED_EVENT_SELECT + " order by scheduleType", new ScheduledEventRowMapper());
    }

    public ScheduledEventVO getScheduledEvent(int id) {
        return ejt.queryForObject(SCHEDULED_EVENT_SELECT + "where id=?", new ScheduledEventRowMapper(), id);
    }

    public ScheduledEventVO getScheduledEvent(String xid) {
        try {
            return ejt.queryForObject(SCHEDULED_EVENT_SELECT + "where xid=?", new ScheduledEventRowMapper(), xid);
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

    class ScheduledEventRowMapper implements RowMapper<ScheduledEventVO> {

        @Override
        public ScheduledEventVO mapRow(ResultSet rs, int rowNum) throws SQLException {
            ScheduledEventVO se = new ScheduledEventVO();
            int i = 0;
            se.setId(rs.getInt(++i));
            se.setXid(rs.getString(++i));
            se.setAlias(rs.getString(++i));
            se.setAlarmLevel(AlarmLevel.values()[rs.getInt(++i)]);
            se.setScheduleType(ScheduledEventKey.fromId(rs.getInt(++i)));
            se.setStateful(charToBool(rs.getString(++i)));
            se.setDisabled(charToBool(rs.getString(++i)));
            se.setActiveYear(rs.getInt(++i));
            se.setActiveMonth(rs.getInt(++i));
            se.setActiveDay(rs.getInt(++i));
            se.setActiveHour(rs.getInt(++i));
            se.setActiveMinute(rs.getInt(++i));
            se.setActiveSecond(rs.getInt(++i));
            se.setActiveCron(rs.getString(++i));
            se.setInactiveYear(rs.getInt(++i));
            se.setInactiveMonth(rs.getInt(++i));
            se.setInactiveDay(rs.getInt(++i));
            se.setInactiveHour(rs.getInt(++i));
            se.setInactiveMinute(rs.getInt(++i));
            se.setInactiveSecond(rs.getInt(++i));
            se.setInactiveCron(rs.getString(++i));
            return se;
        }
    }

    public void saveScheduledEvent(final ScheduledEventVO se) {
        if (se.isNew()) {
            insertScheduledEvent(se);
        } else {
            updateScheduledEvent(se);
        }
    }

    private void insertScheduledEvent(final ScheduledEventVO se) {
        final int id = doInsert(new PreparedStatementCreator() {

            final static String SQL_INSERT = "insert into scheduledEvents ("
                    + "  xid, alarmLevel, alias, scheduleType, returnToNormal, disabled, "
                    + "  activeYear, activeMonth, activeDay, activeHour, activeMinute, activeSecond, activeCron, "
                    + "  inactiveYear, inactiveMonth, inactiveDay, inactiveHour, inactiveMinute, inactiveSecond, inactiveCron "
                    + ") values (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                ps.setString(1, se.getXid());
                ps.setInt(2, se.getAlarmLevel().ordinal());
                ps.setString(3, se.getAlias());
                ps.setInt(4, se.getScheduleType().getId());
                ps.setString(5, boolToChar(se.isStateful()));
                ps.setString(6, boolToChar(se.isDisabled()));
                ps.setInt(7, se.getActiveYear());
                ps.setInt(8, se.getActiveMonth());
                ps.setInt(9, se.getActiveDay());
                ps.setInt(10, se.getActiveHour());
                ps.setInt(11, se.getActiveMinute());
                ps.setInt(12, se.getActiveSecond());
                ps.setString(13, se.getActiveCron());
                ps.setInt(14, se.getInactiveYear());
                ps.setInt(15, se.getInactiveMonth());
                ps.setInt(16, se.getInactiveDay());
                ps.setInt(17, se.getInactiveHour());
                ps.setInt(18, se.getInactiveMinute());
                ps.setInt(19, se.getInactiveSecond());
                ps.setString(20, se.getInactiveCron());
                return ps;
            }

        });
        se.setId(id);
        AuditEventType.raiseAddedEvent(AuditEventKey.SCHEDULED_EVENT, se);
    }

    private void updateScheduledEvent(ScheduledEventVO se) {
        ScheduledEventVO old = getScheduledEvent(se.getId());
        ejt
                .update(
                        "update scheduledEvents set "
                        + "  xid=?, alarmLevel=?, alias=?, scheduleType=?, returnToNormal=?, disabled=?, "
                        + "  activeYear=?, activeMonth=?, activeDay=?, activeHour=?, activeMinute=?, activeSecond=?, activeCron=?, "
                        + "  inactiveYear=?, inactiveMonth=?, inactiveDay=?, inactiveHour=?, inactiveMinute=?, inactiveSecond=?, "
                        + "  inactiveCron=? " + "where id=?", new Object[]{se.getXid(), se.getAlarmLevel(),
                            se.getAlias(), se.getScheduleType(), boolToChar(se.isStateful()),
                            boolToChar(se.isDisabled()), se.getActiveYear(), se.getActiveMonth(),
                            se.getActiveDay(), se.getActiveHour(), se.getActiveMinute(), se.getActiveSecond(),
                            se.getActiveCron(), se.getInactiveYear(), se.getInactiveMonth(), se.getInactiveDay(),
                            se.getInactiveHour(), se.getInactiveMinute(), se.getInactiveSecond(),
                            se.getInactiveCron(), se.getId()});
        AuditEventType.raiseChangedEvent(AuditEventKey.SCHEDULED_EVENT, old, se);
    }

    public void deleteScheduledEvent(final int scheduledEventId) {
        ScheduledEventVO se = getScheduledEvent(scheduledEventId);
        final JdbcTemplate ejt2 = ejt;
        if (se != null) {
            getTransactionTemplate().execute(new TransactionCallbackWithoutResult() {
                @Override
                protected void doInTransactionWithoutResult(TransactionStatus status) {
                    ejt2.update("delete from eventHandlers where eventTypeId=" + EventSources.SCHEDULED.ordinal()
                            + " and eventTypeRef1=?", new Object[]{scheduledEventId});
                    ejt2.update("delete from scheduledEvents where id=?", new Object[]{scheduledEventId});
                }
            });

            AuditEventType.raiseDeletedEvent(AuditEventKey.SCHEDULED_EVENT, se);
        }
    }
}
