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

import br.org.scadabr.DataType;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.List;
import java.util.ResourceBundle;

import org.springframework.jdbc.core.RowCallbackHandler;

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.dao.PointValueDao;
import br.org.scadabr.dao.ReportDao;
import br.org.scadabr.l10n.AbstractLocalizer;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.types.AlphanumericValue;
import com.serotonin.mango.rt.dataImage.types.BinaryValue;
import com.serotonin.mango.rt.dataImage.types.ImageValue;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.rt.dataImage.types.MultistateValue;
import com.serotonin.mango.rt.dataImage.types.NumericValue;
import com.serotonin.mango.rt.event.EventInstance;
import com.serotonin.mango.view.text.TextRenderer;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.UserComment;
import com.serotonin.mango.vo.report.ReportDataStreamHandler;
import com.serotonin.mango.vo.report.ReportDataValue;
import com.serotonin.mango.vo.report.ReportInstance;
import com.serotonin.mango.vo.report.ReportPointInfo;
import com.serotonin.mango.vo.report.ReportUserComment;
import com.serotonin.mango.vo.report.ReportVO;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.util.StringUtils;
import br.org.scadabr.rt.event.type.EventSources;
import br.org.scadabr.utils.ImplementMeException;
import com.serotonin.mango.vo.report.ReportChartCreator;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.Statement;
import java.util.Objects;
import javax.inject.Inject;
import javax.inject.Named;
import org.springframework.dao.EmptyResultDataAccessException;
import org.springframework.jdbc.core.PreparedStatementCreator;
import org.springframework.jdbc.core.RowMapper;

/**
 * @author Matthew Lohbihler
 */
@Named
public class ReportDaoImpl extends BaseDao implements ReportDao {

    @Inject
    private PointValueDao pointValueDao;
    //
    //
    // Report Templates
    //
    private static final String REPORT_SELECT = "select data, id, userId, name from reports ";

    public ReportDaoImpl() {
        super();
    }

    @Override
    public List<ReportVO> getReports() {
        return ejt.query(REPORT_SELECT, new ReportRowMapper());
    }

    public List<ReportVO> getReports(int userId) {
        return ejt.query(REPORT_SELECT + "where userId=? order by name", new ReportRowMapper(), userId);
    }

    public ReportVO getReport(int id) {
        try {
            return ejt.queryForObject(REPORT_SELECT + "where id=?", new ReportRowMapper(), id);
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

    @Override
    public PointInfo createPointInfo(DataPointVO point, String colour, boolean consolidatedChart) {
        return new PointInfoImpl(point, colour, consolidatedChart);
    }

    @Override
    public void reportInstanceData(int id, ReportChartCreator.StreamHandler handler) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    class ReportRowMapper implements RowMapper<ReportVO> {

        @Override
        public ReportVO mapRow(ResultSet rs, int rowNum) throws SQLException {
            int i = 0;
            ReportVO report = (ReportVO) SerializationHelper.readObject(rs.getBlob(++i).getBinaryStream());
            report.setId(rs.getInt(++i));
            report.setUserId(rs.getInt(++i));
            report.setName(rs.getString(++i));
            return report;
        }
    }

    public void saveReport(ReportVO report) {
        if (report.isNew()) {
            insertReport(report);
        } else {
            updateReport(report);
        }
    }

    private void insertReport(final ReportVO report) {
        final int id = doInsert(new PreparedStatementCreator() {

            final static String SQL_INSERT = "insert into reports (userId, name, data) values (?,?,?)";

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                ps.setInt(1, report.getUserId());
                ps.setString(2, report.getName());
                ps.setBlob(3, SerializationHelper.writeObject(report));
                return ps;
            }
        });
        report.setId(id);
    }

    private void updateReport(final ReportVO report) {
        ejt.update(new PreparedStatementCreator() {

            final static String REPORT_UPDATE = "update reports set userId=?, name=?, data=? where id=?";

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                final PreparedStatement ps = con.prepareStatement(REPORT_UPDATE);
                ps.setInt(1, report.getUserId());
                ps.setString(2, report.getName());
                ps.setBlob(3, SerializationHelper.writeObject(report));
                ps.setInt(4, report.getId());
                return ps;
            }
        });
    }

    public void deleteReport(int reportId) {
        ejt.update("delete from reports where id=?", new Object[]{reportId});
    }

    //
    //
    // Report Instances
    //
    private static final String REPORT_INSTANCE_SELECT = "select id, userId, name, includeEvents, includeUserComments, reportStartTime, reportEndTime, runStartTime, "
            + "  runEndTime, recordCount, preventPurge " + "from reportInstances ";

    public List<ReportInstance> getReportInstances(int userId) {
        return ejt.query(REPORT_INSTANCE_SELECT + "where userId=? order by runStartTime desc", new ReportInstanceRowMapper(), userId);
    }

    public ReportInstance getReportInstance(int id) {
        try {
            return ejt.queryForObject(REPORT_INSTANCE_SELECT + "where id=?", new ReportInstanceRowMapper(), id);
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

    class ReportInstanceRowMapper implements RowMapper<ReportInstance> {

        @Override
        public ReportInstance mapRow(ResultSet rs, int rowNum) throws SQLException {
            int i = 0;
            ReportInstance ri = new ReportInstance();
            ri.setId(rs.getInt(++i));
            ri.setUserId(rs.getInt(++i));
            ri.setName(rs.getString(++i));
            ri.setIncludeEvents(rs.getInt(++i));
            ri.setIncludeUserComments(charToBool(rs.getString(++i)));
            ri.setReportStartTime(rs.getLong(++i));
            ri.setReportEndTime(rs.getLong(++i));
            ri.setRunStartTime(rs.getLong(++i));
            ri.setRunEndTime(rs.getLong(++i));
            ri.setRecordCount(rs.getInt(++i));
            ri.setPreventPurge(charToBool(rs.getString(++i)));
            return ri;
        }
    }

    @Override
    public void deleteReportInstance(int id, int userId) {
        ejt.update("delete from reportInstances where id=? and userId=?", new Object[]{id, userId});
    }

    @Override
    public int purgeReportsBefore(final long time) {
        return ejt.update("delete from reportInstances where runStartTime<? and preventPurge=?", new Object[]{time,
            boolToChar(false)});
    }

    public void setReportInstancePreventPurge(int id, boolean preventPurge, int userId) {
        ejt.update("update reportInstances set preventPurge=? where id=? and userId=?", new Object[]{
            boolToChar(preventPurge), id, userId});
    }

    private static final String REPORT_INSTANCE_UPDATE = "update reportInstances set reportStartTime=?, reportEndTime=?, runStartTime=?, runEndTime=?, recordCount=? "
            + "where id=?";

    @Override
    public void saveReportInstance(final ReportInstance instance) {
        if (instance.isNew()) {
            final int id = doInsert(new PreparedStatementCreator() {

                final static String SQL_INSERT = "insert into reportInstances "
                        + "  (userId, name, includeEvents, includeUserComments, reportStartTime, reportEndTime, runStartTime, "
                        + "     runEndTime, recordCount, preventPurge) "
                        + "  values (?,?,?,?,?,?,?,?,?,?)";

                @Override
                public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                    PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                    ps.setInt(1, instance.getUserId());
                    ps.setString(2, instance.getName());
                    ps.setInt(3, instance.getIncludeEvents());
                    ps.setString(4, boolToChar(instance.isIncludeUserComments()));
                    ps.setLong(5, instance.getReportStartTime());
                    ps.setLong(6, instance.getReportEndTime());
                    ps.setLong(7, instance.getRunStartTime());
                    ps.setLong(8, instance.getRunEndTime());
                    ps.setInt(9, instance.getRecordCount());
                    ps.setString(10, boolToChar(instance.isPreventPurge()));
                    return ps;
                }
            });
            instance.setId(id);
        } else {
            ejt.update(
                    REPORT_INSTANCE_UPDATE,
                    new Object[]{instance.getReportStartTime(), instance.getReportEndTime(),
                        instance.getRunStartTime(), instance.getRunEndTime(), instance.getRecordCount(),
                        instance.getId()});
        }
    }

    public static class PointInfoImpl implements PointInfo {

        private final DataPointVO point;
        private final String colour;
        private final boolean consolidatedChart;

        public PointInfoImpl(DataPointVO point, String colour, boolean consolidatedChart) {
            this.point = point;
            this.colour = colour;
            this.consolidatedChart = consolidatedChart;
        }

        @Override
        public DataPointVO getPoint() {
            return point;
        }

        @Override
        public String getColour() {
            return colour;
        }

        @Override
        public boolean isConsolidatedChart() {
            return consolidatedChart;
        }
    }

    @Override
    public int runReport(final ReportInstance instance, List<PointInfo> points, ResourceBundle bundle) {
        int count = 0;
        String userLabel = AbstractLocalizer.localizeI18nKey("common.user", bundle);
        String setPointLabel = AbstractLocalizer.localizeI18nKey("annotation.eventHandler", bundle);
        String anonymousLabel = AbstractLocalizer.localizeI18nKey("annotation.anonymous", bundle);
        String deletedLabel = AbstractLocalizer.localizeI18nKey("common.deleted", bundle);

        // The timestamp selection code is used multiple times for different tables
        String timestampSql;
        Object[] timestampParams;
        if (instance.isFromInception() && instance.isToNow()) {
            timestampSql = "";
            timestampParams = new Object[0];
        } else if (instance.isFromInception()) {
            timestampSql = "and ${field}<?";
            timestampParams = new Object[]{instance.getReportEndTime()};
        } else if (instance.isToNow()) {
            timestampSql = "and ${field}>=?";
            timestampParams = new Object[]{instance.getReportStartTime()};
        } else {
            timestampSql = "and ${field}>=? and ${field}<?";
            timestampParams = new Object[]{instance.getReportStartTime(), instance.getReportEndTime()};
        }

        // For each point.
        for (final PointInfo pointInfo : points) {
            final DataPointVO point = pointInfo.getPoint();
            final DataType dataType = point.getDataType();

            MangoValue startValue = null;
            if (!instance.isFromInception()) {
                // Get the value just before the start of the report
                PointValueTime pvt = pointValueDao.getPointValueBefore(point.getId(), instance.getReportStartTime());
                if (pvt != null && pvt.getDataType() != dataType) {
                    // Make sure value is not null and the data types match
                    startValue = pvt.getValue();
                }
            }

            final MangoValue finalStartValue = startValue;

            final int reportPointId = doInsert(new PreparedStatementCreator() {

                final static String SQL_INSERT = "insert into reportInstancePoints " //
                        + "(reportInstanceId, dataSourceName, pointName, dataType, startValue, textRenderer, colour, consolidatedChart) "
                        + "values (?,?,?,?,?,?,?,?)";

                @Override
                public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                    PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                    // Insert the reportInstancePoints record
                    final String name = StringUtils.truncate(point.getName(), 100);
                    ps.setInt(1, instance.getId());
                    ps.setString(2, point.getDeviceName());
                    ps.setString(3, name);
                    ps.setInt(4, point.getDataType().mangoDbId);
                    ps.setString(5, Objects.toString(finalStartValue));
                    ps.setBlob(6, SerializationHelper.writeObject(point.getTextRenderer()));
                    ps.setString(7, pointInfo.getColour());
                    ps.setString(8, boolToChar(pointInfo.isConsolidatedChart()));
                    return ps;
                }
            });

            // Insert the reportInstanceData records
            String insertSQL = "insert into reportInstanceData \n"
                    + " select\n"
                    + "  id,\n"
                    + "  " + reportPointId + ",\n"
                    + "  pointValue, ts from pointValues\n"
                    + " where dataPointId=? and dataType=?\n"
                    + StringUtils.replaceMacro(timestampSql, "field", "ts");
            count += ejt.update(insertSQL, appendParameters(timestampParams, point.getId(), dataType));

            String annoCase;
            switch (getDataBaseType()) {
                case DERBY:
                    annoCase = "    case when pva.sourceType=1 then '" + userLabel //
                            + ": ' || (case when u.username is null then '" + deletedLabel + "' else u.username end) " //
                            + "         when pva.sourceType=2 then '" + setPointLabel + "' " //
                            + "         when pva.sourceType=3 then '" + anonymousLabel + "' " //
                            + "         else 'Unknown source type: ' || cast(pva.sourceType as char(3)) " //
                            + "    end ";
                    break;
                case MSSQL:
                    annoCase = "    case pva.sourceType" //
                            + "        when 1 then '" + userLabel + ": ' + isnull(u.username, '" + deletedLabel + "') " //
                            + "        when 2 then '" + setPointLabel + "'" //
                            + "        when 3 then '" + anonymousLabel + "'" //
                            + "        else 'Unknown source type: ' + cast(pva.sourceType as nvarchar)" //
                            + "    end ";
                    break;
                case MYSQL:
                    annoCase = "    case pva.sourceType" //
                            + "      when 1 then concat('" + userLabel + ": ',ifnull(u.username,'" + deletedLabel + "')) " //
                            + "      when 2 then '" + setPointLabel + "'" //
                            + "      when 3 then '" + anonymousLabel + "'" //
                            + "      else concat('Unknown source type: ', pva.sourceType)" //
                            + "    end ";
                    break;
                default:
                    throw new ShouldNeverHappenException("unhandled database type: "
                            + getDataBaseType());
            }

            // Insert the reportInstanceDataAnnotations records
            ejt.update("insert into reportInstanceDataAnnotations " //
                    + "  (pointValueId, reportInstancePointId, textPointValueShort, textPointValueLong, sourceValue) " //
                    + "  select rd.pointValueId, rd.reportInstancePointId, pva.textPointValueShort, " //
                    + "    pva.textPointValueLong, " + annoCase + "  from reportInstanceData rd " //
                    + "    join reportInstancePoints rp on rd.reportInstancePointId = rp.id " //
                    + "    join pointValueAnnotations pva on rd.pointValueId = pva.pointValueId " //
                    + "    left join users u on pva.sourceType=1 and pva.sourceId = u.id " //
                    + "  where rp.id = ?", new Object[]{reportPointId});

            // Insert the reportInstanceEvents records for the point.
            if (instance.getIncludeEvents() != ReportVO.EVENTS_NONE) {
                String eventSQL = "insert into reportInstanceEvents " //
                        + "  (eventId, reportInstanceId, typeId, typeRef1, typeRef2, activeTs, rtnApplicable, rtnTs," //
                        + "   rtnCause, alarmLevel, message, ackTs, ackUsername, alternateAckSource)" //
                        + "  select e.id, " + instance.getId() + ", e.typeId, e.typeRef1, e.typeRef2, e.activeTs, " //
                        + "    e.rtnApplicable, e.rtnTs, e.rtnCause, e.alarmLevel, e.message, e.ackTs, u.username, " //
                        + "    e.alternateAckSource " //
                        + "  from events e join userEvents ue on ue.eventId=e.id " //
                        + "    left join users u on e.ackUserId=u.id " //
                        + "  where ue.userId=? " //
                        + "    and e.typeId=" //
                        + EventSources.DATA_POINT.mangoDbId //
                        + "    and e.typeRef1=? ";

                if (instance.getIncludeEvents() == ReportVO.EVENTS_ALARMS) {
                    eventSQL += "and e.alarmLevel > 0 ";
                }

                eventSQL += StringUtils.replaceMacro(timestampSql, "field", "e.activeTs");
                ejt.update(eventSQL, appendParameters(timestampParams, instance.getUserId(), point.getId()));
            }

            // Insert the reportInstanceUserComments records for the point.
            if (instance.isIncludeUserComments()) {
                String commentSQL = "insert into reportInstanceUserComments " //
                        + "  (reportInstanceId, username, commentType, typeKey, ts, commentText)" //
                        + "  select " + instance.getId() + ", u.username, " + UserComment.TYPE_POINT + ", " //
                        + reportPointId + ", uc.ts, uc.commentText " //
                        + "  from userComments uc " //
                        + "    left join users u on uc.userId=u.id " //
                        + "  where uc.commentType=" + UserComment.TYPE_POINT //
                        + "    and uc.typeKey=? ";

                // Only include comments made in the duration of the report.
                commentSQL += StringUtils.replaceMacro(timestampSql, "field", "uc.ts");
                ejt.update(commentSQL, appendParameters(timestampParams, point.getId()));
            }
        }

        // Insert the reportInstanceUserComments records for the selected events
        if (instance.isIncludeUserComments()) {
            String commentSQL = "insert into reportInstanceUserComments " //
                    + "  (reportInstanceId, username, commentType, typeKey, ts, commentText)" //
                    + "  select " + instance.getId() + ", u.username, " + UserComment.TYPE_EVENT + ", uc.typeKey, " //
                    + "    uc.ts, uc.commentText " //
                    + "  from userComments uc " //
                    + "    left join users u on uc.userId=u.id " //
                    + "    join reportInstanceEvents re on re.eventId=uc.typeKey " //
                    + "  where uc.commentType=" + UserComment.TYPE_EVENT //
                    + "    and re.reportInstanceId=? ";
            ejt.update(commentSQL, new Object[]{instance.getId()});
        }

        // If the report had undefined start or end times, update them with values from the data.
        if (instance.isFromInception() || instance.isToNow()) {
            ejt.query(
                    "select min(rd.ts), max(rd.ts) " //
                    + "from reportInstancePoints rp "
                    + "  join reportInstanceData rd on rp.id=rd.reportInstancePointId "
                    + "where rp.reportInstanceId=?", new Object[]{instance.getId()},
                    new RowCallbackHandler() {
                        @Override
                        public void processRow(ResultSet rs) throws SQLException {
                            if (instance.isFromInception()) {
                                instance.setReportStartTime(rs.getLong(1));
                            }
                            if (instance.isToNow()) {
                                instance.setReportEndTime(rs.getLong(2));
                            }
                        }
                    });
        }

        return count;
    }

    private Object[] appendParameters(Object[] toAppend, Object... params) {
        if (toAppend.length == 0) {
            return params;
        }
        if (params.length == 0) {
            return toAppend;
        }

        Object[] result = new Object[params.length + toAppend.length];
        System.arraycopy(params, 0, result, 0, params.length);
        System.arraycopy(toAppend, 0, result, params.length, toAppend.length);
        return result;
    }

    /**
     * This method guarantees that the data is provided to the setData handler
     * method grouped by point (points are not ordered), and sorted by time
     * ascending.
     */
    private static final String REPORT_INSTANCE_POINT_SELECT = "select id, dataSourceName, pointName, dataType, " // 
            + "startValue, textRenderer, colour, consolidatedChart from reportInstancePoints ";
    private static final String REPORT_INSTANCE_DATA_SELECT = "select rd.pointValue, rda.textPointValueShort, " //
            + "  rda.textPointValueLong, rd.ts, rda.sourceValue "
            + "from reportInstanceData rd "
            + "  left join reportInstanceDataAnnotations rda on "
            + "      rd.pointValueId=rda.pointValueId and rd.reportInstancePointId=rda.reportInstancePointId ";

    public void reportInstanceData(int instanceId, final ReportDataStreamHandler handler) {
        // Retrieve point information.
        List<ReportPointInfo> pointInfos = ejt.query(REPORT_INSTANCE_POINT_SELECT + "where reportInstanceId=?",
                new Object[]{instanceId}, new RowMapper<ReportPointInfo>() {
                    @Override
                    public ReportPointInfo mapRow(ResultSet rs, int rowNum) throws SQLException {
                        ReportPointInfo rp = new ReportPointInfo();
                        rp.setReportPointId(rs.getInt(1));
                        rp.setDeviceName(rs.getString(2));
                        rp.setPointName(rs.getString(3));
                        rp.setDataType(DataType.fromMangoDbId(rs.getInt(4)));
                        String startValue = rs.getString(5);
                        if (startValue != null) {
                            rp.setStartValue(MangoValue.stringToValue(startValue, rp.getDataType()));
                        }
                        rp.setTextRenderer((TextRenderer) SerializationHelper.readObject(rs.getBlob(6)
                                        .getBinaryStream()));
                        rp.setColour(rs.getString(7));
                        rp.setConsolidatedChart(charToBool(rs.getString(8)));
                        return rp;
                    }
                });

        final ReportDataValue rdv = new ReportDataValue();
        for (final ReportPointInfo point : pointInfos) {
            handler.startPoint(point);

            rdv.setReportPointId(point.getReportPointId());
            final DataType dataType = point.getDataType();
            ejt.query(REPORT_INSTANCE_DATA_SELECT + "where rd.reportInstancePointId=? order by rd.ts",
                    new Object[]{point.getReportPointId()}, new RowCallbackHandler() {
                        @Override
                        public void processRow(ResultSet rs) throws SQLException {
                            switch (dataType) {
                                case NUMERIC:
                                    rdv.setValue(new NumericValue(rs.getDouble(1)));
                                    break;
                                case BINARY:
                                    rdv.setValue(new BinaryValue(rs.getDouble(1) == 1));
                                    break;
                                case MULTISTATE:
                                    rdv.setValue(new MultistateValue(rs.getInt(1)));
                                    break;
                                case ALPHANUMERIC:
                                    rdv.setValue(new AlphanumericValue(rs.getString(2)));
                                    if (rs.wasNull()) {
                                        rdv.setValue(new AlphanumericValue(rs.getString(3)));
                                    }
                                    break;
                                case IMAGE:
                                    rdv.setValue(new ImageValue(Integer.parseInt(rs.getString(2)), rs.getInt(1)));
                                    break;
                                default:
                                    rdv.setValue(null);
                            }

                            rdv.setTime(rs.getLong(4));
                            rdv.setAnnotation(rs.getString(5));

                            handler.pointData(rdv);
                        }
                    });
        }
        handler.done();
    }

    private static final String EVENT_SELECT = //
            "select eventId, typeId, typeRef1, typeRef2, activeTs, rtnApplicable, rtnTs, rtnCause, alarmLevel, message, " //
            + "ackTs, 0, ackUsername, alternateAckSource " //
            + "from reportInstanceEvents " //
            + "where reportInstanceId=? " //
            + "order by activeTs";
    private static final String EVENT_COMMENT_SELECT = "select username, typeKey, ts, commentText " //
            + "from reportInstanceUserComments " //
            + "where reportInstanceId=? and commentType=? " //
            + "order by ts";

    @Override
    public List<EventInstance> getReportInstanceEvents(int instanceId) {
        throw new ImplementMeException();
        /* TODO handle reports from live db and not from this separate tables ...
// Get the events.
        final List<EventInstance> events = ejt.query(EVENT_SELECT, new Object[]{instanceId}, new EventDao.EventInstanceRowMapper());
        // Add in the comments.
        ejt.query(EVENT_COMMENT_SELECT, new Object[]{instanceId, UserComment.TYPE_EVENT}, new RowCallbackHandler() {
            @Override
            public void processRow(ResultSet rs) throws SQLException {
                // Create the comment
                UserComment c = new UserComment();
                c.setUsername(rs.getString(1));
                c.setTs(rs.getLong(3));
                c.setComment(rs.getString(4));

                // Find the event and add the comment
                int eventId = rs.getInt(2);
                for (EventInstance event : events) {
                    if (event.getId() == eventId) {
                        if (event.getEventComments() == null) {
                            event.setEventComments(new ArrayList<UserComment>());
                        }
                        event.addEventComment(c);
                    }
                }
            }
        });
        // Done
        return events;
                */
    }

    private static final String USER_COMMENT_SELECT = "select rc.username, rc.commentType, rc.typeKey, rp.pointName, " //
            + "  rc.ts, rc.commentText "
            + "from reportInstanceUserComments rc "
            + "  left join reportInstancePoints rp on rc.typeKey=rp.id and rc.commentType="
            + UserComment.TYPE_POINT
            + " " + "where rc.reportInstanceId=? " + "order by rc.ts ";

    @Override
    public List<ReportUserComment> getReportInstanceUserComments(int instanceId) {
        return ejt.query(USER_COMMENT_SELECT, new ReportCommentRowMapper(), instanceId);
    }

    class ReportCommentRowMapper implements RowMapper<ReportUserComment> {

        @Override
        public ReportUserComment mapRow(ResultSet rs, int rowNum) throws SQLException {
            ReportUserComment c = new ReportUserComment();
            c.setUsername(rs.getString(1));
            c.setCommentType(rs.getInt(2));
            c.setTypeKey(rs.getInt(3));
            c.setPointName(rs.getString(4));
            c.setTs(rs.getLong(5));
            c.setComment(rs.getString(6));
            return c;
        }
    }
}
