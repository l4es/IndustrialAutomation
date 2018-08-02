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
import br.org.scadabr.dao.DataPointDao;
import br.org.scadabr.dao.PointLinkDao;
import br.org.scadabr.db.IntValuePair;
import br.org.scadabr.rt.event.type.EventSources;
import br.org.scadabr.rt.link.PointLinkManager;
import br.org.scadabr.util.Tuple;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.utils.TimePeriods;
import br.org.scadabr.vo.datapoints.PointFolderVO;
import br.org.scadabr.vo.event.AlarmLevel;
import br.org.scadabr.vo.event.type.AuditEventKey;
import br.org.scadabr.vo.event.type.DataPointDetectorKey;
import br.org.scadabr.web.LazyTreeNode;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.DoubleDataPointVO;
import com.serotonin.mango.vo.UserComment;
import com.serotonin.mango.vo.bean.PointHistoryCount;
import com.serotonin.mango.vo.event.DoublePointEventDetectorVO;
import com.serotonin.mango.vo.hierarchy.PointFolder;
import com.serotonin.mango.vo.hierarchy.PointHierarchy;
import com.serotonin.mango.vo.hierarchy.PointHierarchyEventDispatcher;
import com.serotonin.mango.vo.link.PointLinkVO;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.sql.Types;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import javax.inject.Inject;
import javax.inject.Named;
import org.springframework.dao.EmptyResultDataAccessException;
import org.springframework.jdbc.UncategorizedSQLException;
import org.springframework.jdbc.core.BatchPreparedStatementSetter;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.jdbc.core.PreparedStatementCreator;
import org.springframework.jdbc.core.RowMapper;
import org.springframework.transaction.TransactionStatus;
import org.springframework.transaction.support.TransactionCallbackWithoutResult;
import br.org.scadabr.vo.EdgeConsumer;

@Named
public class DataPointDaoImpl extends BaseDao implements DataPointDao {

    @Inject
    private PointLinkManager pointLinkManager;
    @Inject
    private PointLinkDao pointLinkDao;

    public DataPointDaoImpl() {
        super();
    }

    @Override
    public String getCanonicalPointName(final DataPointVO dp) {
        throw new ImplementMeException();
        /*        int folderId = dp.getPointFolderId();
        StringBuilder sb = new StringBuilder();
        while (folderId != 0) {
            LazyTreeNode pf = getPointFolderById(folderId);
            sb.insert(0, '.');
            sb.insert(0, pf.getName());
            folderId = pf.getParentId();
        }
        sb.append(dp.getName());
        return sb.toString();
         */
    }

    private static final String DATA_POINT_SELECT = "select\n"
            + " dp.id, dp.xid, dp.clazz, dp.pointLocatorId, dp.name, dp.valuePattern, dp.valueAndUnitPattern, dp.unit, dp.purgePeriodType, dp.purgePeriod, dp.jsonFields\n"
            + "from\n"
            + " dataPoints dp\n";

    @Override
    public Iterable<DataPointVO> getDataPoints(boolean includeRelationalData) {
        List<DataPointVO> dps = ejt.query(DATA_POINT_SELECT, new DataPointRowMapper());
        if (includeRelationalData) {
            setRelationalData(dps);
        }
        return dps;
    }

    @Deprecated
    @Override
    public Iterable<DataPointVO> getDataPoints(int dataSourceId) {
        throw new IllegalArgumentException();
        /*
         final List<DataPointVO> dps = ejt.query(DATA_POINT_SELECT + "where dp.dataSourceId=?", new DataPointRowMapper(), dataSourceId);
         setRelationalData(dps);
         return dps;
         */
    }

    @Override
    public DataPointVO getDataPoint(int id) {
        DataPointVO dp;
        try {
            dp = ejt.queryForObject(DATA_POINT_SELECT + "where dp.id=?", new DataPointRowMapper(), id);
        } catch (EmptyResultDataAccessException e) {
            dp = null;
        }
        setRelationalData(dp);
        return dp;
    }

    @Override
    public DataPointVO getDataPoint(String xid) {
        DataPointVO dp;
        try {
            dp = ejt.queryForObject(DATA_POINT_SELECT + " where dp.xid=?", new DataPointRowMapper(), xid);
        } catch (EmptyResultDataAccessException e) {
            dp = null;
        }
        setRelationalData(dp);
        return dp;
    }


    //TODO Quick and dirty
    @Override
    public Collection<LazyTreeNode> getFoldersAndDpByParentId(final int parentId) {
        throw new ImplementMeException();
        /*
        final Collection<LazyTreeNode> result = new LinkedList<>();

        // Get the folder list.      
        ejt.query("select\n"
                + " id, name\n"
                + "from\n"
                + " dataPointFolders\n"
                + "where\n"
                + " parentId = ? and parentId <> id", (ResultSet rs) -> {
                    LazyTreePointFolder n = new LazyTreePointFolder();
                    n.setParentId(parentId);
                    n.setId(rs.getInt(1));
                    n.setName(rs.getString(2));
                    result.add(n);
                }, parentId);
        //TODO the folderID is saved in the blob ... workaround: reasd all dp ... DataPoints
        // This will change the db...
        for (DataPointVO dp : getDataPoints(false)) {
            if (dp.getPointFolderId() == parentId) {
                LazyTreeDataPoint n = new LazyTreeDataPoint();
                n.setParentId(parentId);
                n.setId(dp.getId());
                n.setName(dp.getName());
                n.setDataType(dp.getDataType().name());
                result.add(n);
            }
        }
        return result;
         */
    }

    @Override
    public String getExtendedPointName(int targetPointId) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public Iterable<PointFolderVO> getPointFolders() {
        final LinkedList<PointFolderVO> result = new LinkedList<>();

        // Get the folder list.      
        ejt.query("select\n"
                + " id, name\n"
                + "from\n"
                + " dataPointFolders\n", (ResultSet rs) -> {
                    PointFolderVO n = new PointFolderVO();
                    n.setId(rs.getInt(1));
                    n.setName(rs.getString(2));
                    result.add(n);
                });
        return result;
    }

    @Override
    public Iterable<DataPointVO> getDataPoints() {
        return getDataPoints(false);
    }

    //TODO create an edge builder interface??? 
    @Override
    public void iteratePointFolderEdges(final EdgeConsumer edgeIterator) {
        ejt.query("select\n"
                + " id,"
                + " parentId\n"
                + "from\n"
                + " dataPointFolders\n", (ResultSet rs) -> {
//                    edgeIterator.edge(rs.getInt(2), rs.getInt(1), "subfolders");
                });
    }

    @Override
    public void iterateDataPointEdges(final EdgeConsumer edgeIterator) {
        ejt.query("select\n"
                + " id,"
                + " folderId\n"
                + "from\n"
                + " dataPoints\n", (ResultSet rs) -> {
//                    edgeIterator.edge(rs.getInt(2), rs.getInt(1), "datapoints");
                });
    }

    @Override
    public String generateUniqueXid() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public boolean isXidUnique(String xid, int id) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }


    class DataPointRowMapper implements RowMapper<DataPointVO> {

        @Override
        public DataPointVO mapRow(ResultSet rs, int rowNum) throws SQLException {
            final DataPointVO dp = null; //(DataPointVO) jsonMapperFactory.read(rs.getClob(11).getAsciiStream(), rs.getString(3));
            dp.setId(rs.getInt(1));
            dp.setPointLocatorId(rs.getObject(4, Integer.class));
            dp.setName(rs.getString(5));
            dp.setValuePattern(rs.getString(6));
            dp.setValueAndUnitPattern(rs.getString(7));
            dp.setUnit(rs.getString(8));
            dp.setPurgeType(TimePeriods.valueOf(rs.getString(9)));
            dp.setPurgePeriod(rs.getInt(10));
            return dp;
        }
    }

    private void setRelationalData(List<DataPointVO> dps) {
        for (DataPointVO dp : dps) {
            setRelationalData(dp);
        }
    }

    private void setRelationalData(DataPointVO dp) {
        if (dp == null) {
            return;
        }
        setEventDetectors(dp);
        setPointComments(dp);
    }

    @Override
    public DataPointVO saveDataPoint(final DataPointVO dp) {
        getTransactionTemplate().execute(new TransactionCallbackWithoutResult() {
            @Override
            protected void doInTransactionWithoutResult(TransactionStatus status) {
                // Decide whether to insert or update.
                if (dp.isNew()) {
                    insertDataPoint(dp);
                    // Reset the point hierarchy so that the new point gets included.
                    cachedPointHierarchy = null;
                } else {
                    updateDataPoint(dp);
                }
            }
        });
        return dp;
    }

    void insertDataPoint(final DataPointVO dp) {

        // Insert the main data point record.
        final int id = doInsert((Connection con) -> {
            final String SQL_INSERT = "insert into dataPoints (xid, clazz, pointLocatorId, name, valuePattern, valueAndUnitPattern, unit, purgePeriodType, purgePeriod, jsonFields) values (?,?,?,?,?,?,?,?,?,?)";
            PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
            ps.setString(2, dp.getClass().getName());
            ps.setObject(3, dp.getPointLocatorId());
            ps.setString(4, dp.getName());
            ps.setString(5, dp.getValuePattern());
            ps.setString(6, dp.getValueAndUnitPattern());
            ps.setString(7, dp.getUnit());
            ps.setString(8, dp.getPurgeType().name());
            ps.setInt(9, dp.getPurgePeriod());
//            ps.setClob(10, jsonMapperFactory.write(dp));
            return ps;
        });
        dp.setId(id);
        // Save the relational information.
        saveEventDetectors(dp);

        AuditEventType.raiseAddedEvent(AuditEventKey.DATA_POINT, dp);
    }

    @Override
    public void updateDataPoint(final DataPointVO dp) {
        DataPointVO old = getDataPoint(dp.getId());

        ejt.update((Connection con) -> {
            final String SQL_UPDATE = "update dataPoints set xid=?, clazz=?, pointLocatorId=?, name=?, valuePattern=?, valueAndUnitPattern=?, unit=?, purgePeriodType=?, purgePeriod=?, jsonFields=? where id=?";
            PreparedStatement ps = con.prepareStatement(SQL_UPDATE);
            ps.setString(2, dp.getClass().getName());
            ps.setObject(3, dp.getPointLocatorId());
            ps.setString(4, dp.getName());
            ps.setString(5, dp.getValuePattern());
            ps.setString(6, dp.getValueAndUnitPattern());
            ps.setString(7, dp.getUnit());
            ps.setString(8, dp.getPurgeType().name());
            ps.setInt(9, dp.getPurgePeriod());
//            ps.setClob(10, jsonMapperFactory.write(dp));
            ps.setInt(11, dp.getId());
            return ps;
        });

        AuditEventType.raiseChangedEvent(AuditEventKey.DATA_POINT, old, dp);

        // Save the relational information.
        saveEventDetectors(dp);
    }

    @Deprecated // cut connection between dp an pl ...  
    @Override
    public void deleteDataPoints(final int dataSourceId) {
        final Iterable<DataPointVO> old = getDataPoints(dataSourceId);
        for (DataPointVO dp : old) {
            beforePointDelete(dp.getId());
        }

        for (DataPointVO dp : old) {
            deletePointHistory(dp);
        }

        getTransactionTemplate().execute((TransactionStatus status) -> {
            final List<Integer> pointIds = ejt.queryForList("select id from dataPoints where dataSourceId=?", Integer.class, dataSourceId);
            if (!pointIds.isEmpty()) {
                deleteDataPointImpl(createDelimitedList(pointIds, ","));
            }
            return null;
        });

        for (DataPointVO dp : old) {
            AuditEventType.raiseDeletedEvent(AuditEventKey.DATA_POINT, dp);
        }
    }

    @Override
    public DataPointVO deleteDataPoint(final int dataPointId) {
        final DataPointVO dp = getDataPoint(dataPointId);
        if (dp != null) {
            beforePointDelete(dataPointId);
            deletePointHistory(dp);
            getTransactionTemplate().execute(new TransactionCallbackWithoutResult() {
                @Override
                protected void doInTransactionWithoutResult(TransactionStatus status) {
                    deleteDataPointImpl(Integer.toString(dataPointId));
                }
            });

            AuditEventType.raiseDeletedEvent(AuditEventKey.DATA_POINT, dp);
        }
        return dp;
    }

    private void beforePointDelete(int dataPointId) {
        for (PointLinkVO link : pointLinkDao.getPointLinksForPoint(dataPointId)) {
            pointLinkManager.deletePointLink(link.getId());
        }
    }

//    void deletePointHistory(int dataPointId) {
    void deletePointHistory(final DataPointVO dpvo) {
        assert dpvo.getDataType() == DataType.DOUBLE; // Gate for adding more datatypes (later...)
        ejt.update("delete from doublePointValues where dataPointId=?", dpvo.getId());
    }

    void deletePointHistory(int dataPointId, long min, long max) {
        while (true) {
            try {
                ejt.update("delete from doublePointValues where dataPointId=? and ts <= ?", new Object[]{dataPointId, max});
                break;
            } catch (UncategorizedSQLException e) {
                if ("The total number of locks exceeds the lock table size".equals(e.getSQLException().getMessage())) {
                    long mid = (min + max) >> 1;
                    deletePointHistory(dataPointId, min, mid);
                    min = mid;
                } else {
                    throw e;
                }
            }
        }
    }

    void deleteDataPointImpl(String dataPointIdList) {
        dataPointIdList = "(" + dataPointIdList + ")";
        ejt.update("delete from eventHandlers where eventTypeId=" + EventSources.DATA_POINT.ordinal()
                + " and eventTypeRef1 in " + dataPointIdList);
        ejt.update("delete from userComments where commentType=2 and typeKey in " + dataPointIdList);
        ejt.update("delete from pointEventDetectors where dataPointId in " + dataPointIdList);
        ejt.update("delete from dataPointUsers where dataPointId in " + dataPointIdList);
        ejt.update("delete from watchListPoints where dataPointId in " + dataPointIdList);
        ejt.update("delete from dataPoints where id in " + dataPointIdList);

        cachedPointHierarchy = null;
    }

    //
    //
    // Event detectors
    //
    public int getDataPointIdFromDetectorId(int pedId) {
        return ejt.queryForObject("select dataPointId from pointEventDetectors where id=?", Integer.class, pedId);
    }

    public String getDetectorXid(int pedId) {
        try {
            return ejt.queryForObject("select xid from pointEventDetectors where id=?", String.class, pedId);
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

    public int getDetectorId(String pedXid, int dataPointId) {
        try {
            return ejt.queryForObject("select id from pointEventDetectors where xid=? and dataPointId=?", Integer.class, pedXid, dataPointId);
        } catch (EmptyResultDataAccessException e) {
            return -1;
        }
    }

    public String generateEventDetectorUniqueXid(int dataPointId) {
        String xid = generateXid(DoublePointEventDetectorVO.XID_PREFIX);
        while (!isEventDetectorXidUnique(dataPointId, xid, -1)) {
            xid = generateXid(DoublePointEventDetectorVO.XID_PREFIX);
        }
        return xid;
    }

    public boolean isEventDetectorXidUnique(int dataPointId, String xid, int excludeId) {
        return ejt.queryForObject("select count(*) from pointEventDetectors where dataPointId=? and xid=? and id<>?", Integer.class, dataPointId, xid, excludeId) == 0;
    }

    private void setEventDetectors(DataPointVO dp) {
        dp.setEventDetectors(getEventDetectors(dp));
    }

    private List<DoublePointEventDetectorVO> getEventDetectors(DataPointVO dp) {
        return ejt.query(
                "select id, xid, alias, detectorType, alarmLevel, stateLimit, duration, durationType, binaryState, " //
                + "  multistateState, changeCount, alphanumericState, weight " //
                + "from pointEventDetectors " //
                + "where dataPointId=? " // 
                + "order by id",
                new EventDetectorRowMapper(dp), dp.getId());
    }

    class EventDetectorRowMapper implements RowMapper<DoublePointEventDetectorVO> {

        private final DataPointVO dp;

        public EventDetectorRowMapper(DataPointVO dp) {
            this.dp = dp;
        }

        @Override
        public DoublePointEventDetectorVO mapRow(ResultSet rs, int rowNum) throws SQLException {
            DoublePointEventDetectorVO detector = new DoublePointEventDetectorVO();
            int i = 0;
            detector.setId(rs.getInt(++i));
            detector.setXid(rs.getString(++i));
            detector.setAlias(rs.getString(++i));
            detector.setDataPointDetectorKey(DataPointDetectorKey.fromId(rs.getInt(++i)));
            detector.setAlarmLevel(AlarmLevel.values()[rs.getInt(++i)]);
            detector.setLimit(rs.getDouble(++i));
            detector.setDuration(rs.getInt(++i));
            detector.setDurationType(TimePeriods.fromId(rs.getInt(++i)));
            detector.setBinaryState(charToBool(rs.getString(++i)));
            detector.setMultistateState(rs.getInt(++i));
            detector.setChangeCount(rs.getInt(++i));
            detector.setAlphanumericState(rs.getString(++i));
            detector.setWeight(rs.getDouble(++i));
            detector.njbSetDataPoint((DoubleDataPointVO) dp);
            return detector;
        }
    }

    private <T extends PointValueTime> void saveEventDetectors(final DataPointVO<?, T> dp) {
        if (dp.isEvtDetectorsEmpty()) {
            return;
        }
        // Get the ids of the existing detectors for this point.
        final List<DoublePointEventDetectorVO> existingDetectors = getEventDetectors(dp);

        // Insert or update each detector in the point.
        for (final DoublePointEventDetectorVO ped : dp.getEventDetectors()) {
            if (ped.getId() < 0) {
                // Insert the record.
                final int id = doInsert(new PreparedStatementCreator() {

                    final static String SQL_INSERT = "insert into pointEventDetectors "
                            + "  (xid, alias, dataPointId, detectorType, alarmLevel, stateLimit, duration, durationType, "
                            + "  binaryState, multistateState, changeCount, alphanumericState, weight) "
                            + "values (?,?,?,?,?,?,?,?,?,?,?,?,?)";

                    @Override
                    public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                        PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                        ps.setString(1, ped.getXid());
                        ps.setString(2, ped.getAlias());
                        ps.setInt(3, dp.getId());
                        ps.setInt(4, ped.getDataPointDetectorKey().getId());
                        ps.setInt(5, ped.getAlarmLevel().ordinal());
                        ps.setDouble(6, ped.getLimit());
                        ps.setInt(7, ped.getDuration());
                        ps.setInt(8, ped.getDurationType().getId());
                        ps.setString(9, boolToChar(ped.isBinaryState()));
                        ps.setInt(10, ped.getMultistateState());
                        ps.setInt(11, ped.getChangeCount());
                        ps.setString(12, ped.getAlphanumericState());
                        ps.setDouble(13, ped.getWeight());
                        return ps;
                    }
                });
                ped.setId(id);
                AuditEventType.raiseAddedEvent(AuditEventKey.POINT_EVENT_DETECTOR, ped);
            } else {
                DoublePointEventDetectorVO old = removeFromList(existingDetectors, ped.getId());

                ejt.update(
                        "update pointEventDetectors set xid=?, alias=?, alarmLevel=?, stateLimit=?, duration=?, "
                        + "  durationType=?, binaryState=?, multistateState=?, changeCount=?, alphanumericState=?, "
                        + "  weight=? " + "where id=?",
                        new Object[]{ped.getXid(), ped.getAlias(), ped.getAlarmLevel(), ped.getLimit(),
                            ped.getDuration(), ped.getDurationType(), boolToChar(ped.isBinaryState()),
                            ped.getMultistateState(), ped.getChangeCount(), ped.getAlphanumericState(),
                            ped.getWeight(), ped.getId()}, new int[]{Types.VARCHAR, Types.VARCHAR,
                            Types.INTEGER, Types.DOUBLE, Types.INTEGER, Types.INTEGER, Types.VARCHAR,
                            Types.INTEGER, Types.INTEGER, Types.VARCHAR, Types.DOUBLE, Types.INTEGER});

                AuditEventType.raiseChangedEvent(AuditEventKey.POINT_EVENT_DETECTOR, old, ped);
            }
        }

        // Delete detectors for any remaining ids in the list of existing detectors.
        for (DoublePointEventDetectorVO ped : existingDetectors) {
            ejt.update("delete from eventHandlers " + "where eventTypeId=" + EventSources.DATA_POINT.ordinal()
                    + " and eventTypeRef1=? and eventTypeRef2=?", new Object[]{dp.getId(), ped.getId()});
            ejt.update("delete from pointEventDetectors where id=?", new Object[]{ped.getId()});

            AuditEventType.raiseDeletedEvent(AuditEventKey.POINT_EVENT_DETECTOR, ped);
        }
    }

    private DoublePointEventDetectorVO removeFromList(List<DoublePointEventDetectorVO> list, int id) {
        for (DoublePointEventDetectorVO ped : list) {
            if (ped.getId() == id) {
                list.remove(ped);
                return ped;
            }
        }
        return null;
    }

    @Override
    public void copyPermissions(final int fromDataPointId, final int toDataPointId) {
        final List<Tuple<Integer, Integer>> ups = new ArrayList<>();
        ejt.query("select userId, permission from dataPointUsers where dataPointId=?",
                (ResultSet rs) -> ups.add(new Tuple<>(rs.getInt(1), rs.getInt(2))),
                fromDataPointId);

        ejt.batchUpdate("insert into dataPointUsers values (?,?,?)", new BatchPreparedStatementSetter() {
            @Override
            public int getBatchSize() {
                return ups.size();
            }

            @Override
            public void setValues(PreparedStatement ps, int i) throws SQLException {
                ps.setInt(1, toDataPointId);
                ps.setInt(2, ups.get(i).getElement1());
                ps.setInt(3, ups.get(i).getElement2());
            }
        });
    }

    //
    //
    // Point comments
    //
    private static final String POINT_COMMENT_SELECT = UserCommentRowMapper.USER_COMMENT_SELECT
            + "where uc.commentType= " + UserComment.TYPE_POINT + " and uc.typeKey=? " + "order by uc.ts";

    private void setPointComments(DataPointVO dp) {
        dp.setComments(ejt.query(POINT_COMMENT_SELECT, new UserCommentRowMapper(), dp.getId()));
    }

    //
    //
    // Point hierarchy
    //
    @Deprecated
    static PointHierarchy cachedPointHierarchy;

    @Deprecated // get Rootfolder ...
    public PointHierarchy getPointHierarchy() {
        if (cachedPointHierarchy == null) {
            final Map<Integer, List<PointFolder>> folders = new HashMap<>();

            // Get the folder list.
            ejt.query("select id, parentId, name from dataPointFolders", (ResultSet rs) -> {
                PointFolder f = new PointFolder(rs.getInt(1), rs.getString(3));
                int parentId = rs.getInt(2);
                List<PointFolder> folderList = folders.get(parentId);
                if (folderList == null) {
                    folderList = new LinkedList<>();
                    folders.put(parentId, folderList);
                }
                folderList.add(f);
            });

            // Create the folder hierarchy.
            PointHierarchy ph = new PointHierarchy();
//            addFoldersToHeirarchy(ph, ROOT_ID, folders);

            if (true) {
                throw new ImplementMeException();
            }
            // Add data points.
            for (DataPointVO dp : getDataPoints(false)) {
//TODO                ph.addDataPoint(dp.getId(), dp.getPointFolderId(), dp.getName());
            }

            cachedPointHierarchy = ph;
        }

        return cachedPointHierarchy;
    }

    private void addFoldersToHeirarchy(PointHierarchy ph, int parentId, Map<Integer, List<PointFolder>> folders) {
        List<PointFolder> folderList = folders.remove(parentId);
        if (folderList == null) {
            return;
        }

        for (PointFolder f : folderList) {
            ph.addPointFolder(f, parentId);
            addFoldersToHeirarchy(ph, f.getId(), folders);
        }
    }

    @Deprecated // Keep db in sync ... and not remove all and then...
    public void savePointHierarchy(final PointFolder root) {
        final JdbcTemplate ejt2 = ejt;
        getTransactionTemplate().execute(new TransactionCallbackWithoutResult() {
            @Override
            protected void doInTransactionWithoutResult(TransactionStatus status) {
                // Dump the hierarchy table.
                ejt2.update("delete from dataPointFolders");

                // Save the point folders.
//                savePointFolder(root, ROOT_ID);
            }
        });

        // Save the point folders. This is not done in the transaction because it can cause deadlocks in Derby.
        savePointsInFolder(root);

        cachedPointHierarchy = null;
        cachedPointHierarchy = getPointHierarchy();
        PointHierarchyEventDispatcher.firePointHierarchySaved(root);
    }

    public void savePointFolder(final PointFolder folder, final int parentId) {
        // Save the folder.
        if (folder.isNew()) {
            final int id = doInsert(new PreparedStatementCreator() {

                final static String SQL_INSERT = "insert into dataPointFolders (parentId, name) values (?,?)";

                @Override
                public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                    PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                    ps.setInt(1, parentId);
                    ps.setString(2, folder.getName());
                    return ps;
                }
            });
            folder.setId(id);
        } else if (folder.getId() != 0) {
            ejt.update("update dataPointFolders set parentId=?, name=? where id=?", parentId, folder.getName(), folder.getId());
        }
        // Save the subfolders
        for (PointFolder sf : folder.getSubfolders()) {
            savePointFolder(sf, folder.getId());
        }
        savePointFolder(folder, folder.getId());
        //DODO save points... to new 
    }

    void savePointsInFolder(PointFolder folder) {
        // Save the points in the subfolders
        for (PointFolder sf : folder.getSubfolders()) {
            savePointsInFolder(sf);
        }
    }

    @Deprecated
    public List<PointHistoryCount> getTopPointHistoryCounts() {
        List<PointHistoryCount> counts = ejt.query("select dataPointId, count(*) from pointValues group by dataPointId order by 2 desc", (ResultSet rs, int rowNum) -> {
            PointHistoryCount c = new PointHistoryCount();
            c.setPointId(rs.getInt(1));
            c.setCount(rs.getInt(2));
            return c;
        });

        // Collate in the point names.
        for (PointHistoryCount c : counts) {
            for (DataPointVO point : getDataPoints(false)) {
                if (point.getId() == c.getPointId()) {
                    c.setPointName(point.getName());
                    break;
                }
            }
        }

        return counts;
    }

    @Override
    public void addPointToHierarchy(DataPointVO dp, String... pathToPoint) {
        PointHierarchy ph = getPointHierarchy();
        PointFolder pf = ph.getRoot();
        for (String folderName : pathToPoint) {
            boolean folderFound = false;
            for (PointFolder subFolder : pf.getSubfolders()) {
                if (subFolder.getName().equals(folderName)) {
                    pf = subFolder;
                    folderFound = true;
                    break;
                }
            }
            if (!folderFound) {
                PointFolder newFolder = new PointFolder(folderName);
                pf.addSubfolder(newFolder);
                pf = newFolder;
//                savePointFolder(newFolder, pf.getId());
            }
        }
        pf.addDataPoint(new IntValuePair(dp.getId(), dp.getName()));
        ph.getRoot().removeDataPoint(dp.getId());
//        savePointsInFolder(pf);
        savePointHierarchy(ph.getRoot());
    }

}
