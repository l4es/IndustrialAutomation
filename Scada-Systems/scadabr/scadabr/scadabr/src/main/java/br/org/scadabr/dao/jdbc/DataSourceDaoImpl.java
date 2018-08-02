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

import br.org.scadabr.dao.DataPointDao;
import br.org.scadabr.dao.DataSourceDao;
import br.org.scadabr.dao.MaintenanceEventDao;
import br.org.scadabr.l10n.AbstractLocalizer;
import br.org.scadabr.rt.event.type.EventSources;
import br.org.scadabr.util.StringUtils;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.vo.datasource.PointLocatorFolderVO;
import br.org.scadabr.vo.datasource.PointLocatorVO;
import br.org.scadabr.vo.event.type.AuditEventKey;
import br.org.scadabr.vo.event.type.DataSourceEventKey;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.rt.event.type.DataSourceEventType;
import br.org.scadabr.util.ChangeComparable;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.DoubleDataPointVO;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.event.DoublePointEventDetectorVO;
import java.io.Serializable;
import java.sql.Blob;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.ResourceBundle;
import java.util.Set;
import javax.inject.Inject;
import javax.inject.Named;
import org.springframework.dao.DataAccessException;
import org.springframework.dao.EmptyResultDataAccessException;
import org.springframework.jdbc.core.BatchPreparedStatementSetter;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.jdbc.core.PreparedStatementCreator;
import org.springframework.jdbc.core.ResultSetExtractor;
import org.springframework.jdbc.core.RowMapper;
import org.springframework.transaction.TransactionStatus;
import org.springframework.transaction.support.TransactionCallback;
import org.springframework.transaction.support.TransactionCallbackWithoutResult;

@Named
public class DataSourceDaoImpl extends BaseDao implements DataSourceDao {

    private static final String DATA_SOURCE_SELECT = "select\n"
            + " id, name, clazzName, xid, enabled, jsonFields\n"
            + "from\n"
            + " dataSources\n";
    private static final String POINT_LOCATOR_SELECT = "select\n"
            + " id, dataSourceId, pointLocatorFolderId, clazz, name, jsonFields\n"
            + "from\n"
            + " pointLocators\n";
    private static final String CHILD_FOLDER_SELECT = "select\n"
            + " id, dataSourceId, parentId, clazz, name, jsonFields\n"
            + "from\n"
            + " pointLocatorFolders\n";

    @Inject
    @Deprecated //TODO switch to PointLocator
    private DataPointDao dataPointDao;
    @Inject
    private MaintenanceEventDao maintenanceEventDao;

    Map<Integer, Map<Integer, DataSourceEventType>> dataSourceEventTypes = new HashMap<>();

    public DataSourceDaoImpl() {
        super();
    }

    @Override
    public Iterable<DataSourceVO<?>> getDataSources() {
        List<DataSourceVO<?>> dss = ejt.query(DATA_SOURCE_SELECT + " order by name asc", new DataSourceRowMapper());
        return dss;
    }

    @Override
    public DataSourceVO<?> getDataSource(int id) {
        try {
            return ejt.queryForObject(DATA_SOURCE_SELECT + " where id=?", new DataSourceRowMapper(), id);
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

    public DataSourceVO<?> getDataSource(String xid) {
        try {
            return ejt.queryForObject(DATA_SOURCE_SELECT + " where xid=?", new DataSourceRowMapper(), xid);
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

    @Override
    public DataSourceEventType getEventType(int dsId, int eventKeyId) {
        Map<Integer, DataSourceEventType> eventTypes = dataSourceEventTypes.get(dsId);
        if (eventTypes == null) {
            final DataSourceVO dsVo = getDataSource(dsId);
            eventTypes = new HashMap<>();
            for (DataSourceEventKey key : (Set<DataSourceEventKey>) dsVo.createEventKeySet()) {
                eventTypes.put(key.getId(), dsVo.getEventType(key));
            }
            dataSourceEventTypes.put(dsId, eventTypes);
        }
        return eventTypes.get(eventKeyId);
    }

    @Override
    public PointLocatorFolderVO getPointLocatorFolder(int id) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void savePointLocatorFolder(PointLocatorFolderVO plfVo) {
        // Decide whether to insert or update.
        if (plfVo.isNew()) {
            insertPointLocatorFolder(plfVo);
        } else {
            updatePointLocatorFolder(plfVo);
        }
    }

    private void insertPointLocatorFolder(final PointLocatorFolderVO vo) {
        final int id = doInsert(new PreparedStatementCreator() {

            final static String SQL_INSERT = "insert into\n"
                    + "pointLocatorFolders\n"
                    + " (dataSourceId, parentId, name, clazz, jsonFields)\n"
                    + "values\n"
                    + " (?,?,?,?,?)";

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                ps.setInt(1, vo.getDataSourceId());
                ps.setObject(2, vo.getParentFolderId());
                ps.setString(3, vo.getName());
                ps.setString(4, vo.getClass().getName());
//                ps.setClob(5, jsonMapperFactory.write(vo));
                return ps;
            }
        });
        vo.setId(id);
//TODO        AuditEventType.raiseAddedEvent(AuditEventKey.DATA_SOURCE, vo);
    }

    @SuppressWarnings("unchecked")
    private void updatePointLocatorFolder(final PointLocatorFolderVO vo) {
        PointLocatorVO old = getPointLocator(vo.getId());

        ejt.update(new PreparedStatementCreator() {

            final static String SQL_UPDATE = "update\n"
                    + " pointLocatorFolders\n"
                    + "set\n"
                    + " dataSourceId=?, parentId=?, name=?\n, clazz=?, jsonFields=?"
                    + "where\n"
                    + " id=?";

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareStatement(SQL_UPDATE);
                ps.setInt(1, vo.getDataSourceId());
                ps.setObject(2, vo.getParentFolderId());
                ps.setString(3, vo.getName());
                ps.setString(4, vo.getClass().getName());
//                ps.setClob(5, jsonMapperFactory.write(vo));
                return ps;
            }
        });
//TODO        AuditEventType.raiseChangedEvent(AuditEventKey.DATA_SOURCE, old, (ChangeComparable<PointLocatorVO<?>>) vo);
    }

    @Override
    public void deletePointLocatorFolder(int plfId) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public Iterable<PointLocatorVO> getPointLocatorsByParent(int dataSourceId, int parentFolderId) {
        try {
            return ejt.query(POINT_LOCATOR_SELECT 
                    + "where\n"
                    + " pointLocatorFolderId=? and dataSourceId=?", new PointLocatorRowMapper(), parentFolderId, dataSourceId);
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

    @Override
    public Iterable<PointLocatorFolderVO> getPointLocatorsFolderByParent(int dataSourceId, int parentFolderId) {
        try {
            return ejt.query(CHILD_FOLDER_SELECT 
                    + "where\n"
                    + " parentId=? and dataSourceId=?", 
                    new PointLocatorFolderRowMapper(), 
                    parentFolderId, dataSourceId);
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

    @Override
    public PointLocatorFolderVO getRootPointLocatorFolder(int dataSourceId) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public Iterable<PointLocatorVO> getPointLocatorsByParent(int parentFolderId) {
        try {
            return ejt.query(POINT_LOCATOR_SELECT 
                    + "where\n"
                    + " pointLocatorFolderId=?", new PointLocatorRowMapper(), parentFolderId);
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

    @Override
    public Iterable<PointLocatorFolderVO> getPointLocatorsFolderByParent(int parentFolderId) {
        try {
            return ejt.query(CHILD_FOLDER_SELECT 
                    + "where\n"
                    + " parentId=?", 
                    new PointLocatorFolderRowMapper(), 
                    parentFolderId);
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

    class DataSourceRowMapper implements RowMapper<DataSourceVO<?>> {

        @Override
        public DataSourceVO<?> mapRow(ResultSet rs, int rowNum) throws SQLException {
            DataSourceVO<?> ds = null; //(DataSourceVO<?>) jsonMapperFactory.read(rs.getClob(6).getAsciiStream(), rs.getString(3));
            ds.setId(rs.getInt(1));
            ds.setName(rs.getString(2));
            ds.setXid(rs.getString(4));
            ds.setEnabled(rs.getBoolean(5));
            return ds;
        }
    }

    public String generateUniqueXid() {
        return generateUniqueXid(DataSourceVO.XID_PREFIX, "dataSources");
    }

    @Override
    public boolean isXidUnique(String xid) {
        return isXidUnique(xid, "dataSources");
    }

    @Override
    public boolean isXidUnique(DataSourceVO dsVo) {
        return isXidUnique(dsVo.getXid(), dsVo.getId(), "dataSources");
    }

    @Override
    public void saveDataSource(final DataSourceVO<?> vo) {
        // Decide whether to insert or update.
        if (vo.isNew()) {
            insertDataSource(vo);
        } else {
            updateDataSource(vo);
        }
    }

    private void insertDataSource(final DataSourceVO<?> vo) {
        if (vo.getXid() == null || vo.getXid().isEmpty()) {
            vo.setXid(generateUniqueXid());
        }
        final int id = doInsert(new PreparedStatementCreator() {

            final static String SQL_INSERT = "insert into dataSources (name, clazzName, xid, enabled, jsonFields) values (?,?,?,?,?)";

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                ps.setString(1, vo.getName());
                ps.setString(2, vo.getClass().getName());
                ps.setString(3, vo.getXid());
                ps.setBoolean(4, vo.isEnabled());
//                ps.setClob(5, jsonMapperFactory.write(vo));
                return ps;
            }
        });
        vo.setId(id);
        AuditEventType.raiseAddedEvent(AuditEventKey.DATA_SOURCE, vo);
    }

    @SuppressWarnings("unchecked")
    private void updateDataSource(final DataSourceVO<?> vo) {
        DataSourceVO<?> old = getDataSource(vo.getId());

        ejt.update(new PreparedStatementCreator() {

            final static String SQL_UPDATE = "update dataSources set name=?, clazzName=?, xid=?, enabled=?, jsonFields=? where id=?";

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareStatement(SQL_UPDATE);
                ps.setString(1, vo.getName());
                ps.setString(2, vo.getClass().getName());
                ps.setString(3, vo.getXid());
                ps.setBoolean(4, vo.isEnabled());
//                ps.setClob(5, jsonMapperFactory.write(vo));
                ps.setInt(6, vo.getId());
                return ps;
            }
        });
        AuditEventType.raiseChangedEvent(AuditEventKey.DATA_SOURCE, old, (ChangeComparable<DataSourceVO<?>>) vo);
    }

    @Override
    public void deleteDataSource(final int dataSourceId) {
        DataSourceVO<?> vo = getDataSource(dataSourceId);
        final JdbcTemplate ejt2 = ejt;

        if (vo != null) {
            getTransactionTemplate().execute(new TransactionCallbackWithoutResult() {
                @Override
                protected void doInTransactionWithoutResult(TransactionStatus status) {
                    maintenanceEventDao.deleteMaintenanceEventsForDataSource(dataSourceId);
                    ejt2.update("delete from eventHandlers where eventTypeId=" + EventSources.DATA_SOURCE.ordinal()
                            + " and eventTypeRef1=?", new Object[]{dataSourceId});
                    ejt2.update("delete from dataSourceUsers where dataSourceId=?", new Object[]{dataSourceId});
                    ejt2.update("delete from dataSources where id=?", new Object[]{dataSourceId});
                }
            });

            AuditEventType.raiseDeletedEvent(AuditEventKey.DATA_SOURCE, vo);
        }
    }

    public void copyPermissions(final int fromDataSourceId, final int toDataSourceId) {
        final List<Integer> userIds = ejt.queryForList("select userId from dataSourceUsers where dataSourceId=?", Integer.class, fromDataSourceId);

        ejt.batchUpdate("insert into dataSourceUsers values (?,?)", new BatchPreparedStatementSetter() {
            @Override
            public int getBatchSize() {
                return userIds.size();
            }

            @Override
            public void setValues(PreparedStatement ps, int i) throws SQLException {
                ps.setInt(1, toDataSourceId);
                ps.setInt(2, userIds.get(i));
            }
        });
    }

    public int copyDataSource(final int dataSourceId, final ResourceBundle bundle) {
        return getTransactionTemplate().execute(new TransactionCallback<Integer>() {
            @Override
            public Integer doInTransaction(TransactionStatus status) {

                DataSourceVO<?> dataSource = getDataSource(dataSourceId);

                // Copy the data source.
                DataSourceVO<?> dataSourceCopy = dataSource.copy();
                dataSourceCopy.setId(null);
                dataSourceCopy.setXid(generateUniqueXid());
                dataSourceCopy.setEnabled(false);
                dataSourceCopy.setName(StringUtils.truncate(
                        AbstractLocalizer.localizeI18nKey("common.copyPrefix", bundle, dataSource.getName()), 40));
                saveDataSource(dataSourceCopy);

                // Copy permissions.
                copyPermissions(dataSource.getId(), dataSourceCopy.getId());

                // Copy the points.
                for (DataPointVO<?, ?> dataPoint : dataPointDao.getDataPoints(dataSourceId)) {
                    DataPointVO<?, ?> dataPointCopy = dataPoint.copy();
                    dataPointCopy.setId(null);
                    dataPointCopy.setName(dataPoint.getName());
                    dataPointCopy.getComments().clear();

                    // Copy the event detectors
                    for (DoublePointEventDetectorVO ped : dataPointCopy.getEventDetectors()) {
                        ped.setId(null);
                        ped.njbSetDataPoint((DoubleDataPointVO) dataPointCopy);
                    }

                    dataPointDao.saveDataPoint(dataPointCopy);

                    // Copy permissions
                    dataPointDao.copyPermissions(dataPoint.getId(), dataPointCopy.getId());
                }

                return dataSourceCopy.getId();
            }
        });
    }

    @Override
    public Object getPersistentData(DataSourceVO dsVo) {
        return ejt.query("select rtdata from dataSources where id=?", new Object[]{dsVo.getId()},
                new ResultSetExtractor<Serializable>() {
            @Override
            public Serializable extractData(ResultSet rs) throws SQLException, DataAccessException {
                if (!rs.next()) {
                    return null;
                }

                Blob blob = rs.getBlob(1);
                if (blob == null) {
                    return null;
                }
                throw new ImplementMeException();
//                        return (Serializable) SerializationHelper.readObject(blob.getBinaryStream());
            }
        });
    }

    @Override
    public void savePersistentData(final DataSourceVO dsVO, final Object data) {
        ejt.update(new PreparedStatementCreator() {

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                final PreparedStatement ps = con.prepareStatement("update dataSources set rtdata=? where id=?");
                if (true) {
                    throw new ImplementMeException();
                }
                //ps.setBlob(1, SerializationHelper.writeObject(data));
                ps.setInt(2, dsVO.getId());
                return ps;
            }
        });
    }

    class PointLocatorRowMapper implements RowMapper<PointLocatorVO> {

        @Override
        public PointLocatorVO mapRow(ResultSet rs, int rowNum) throws SQLException {
            final PointLocatorVO dp = null; //(PointLocatorVO) jsonMapperFactory.read(rs.getClob(6).getAsciiStream(), rs.getString(4));
            dp.setId(rs.getInt(1));
            dp.setDataSourceId(rs.getInt(2));
            dp.setPointLocatorFolderId(rs.getInt(3));
            dp.setName(rs.getString(5));
            return dp;
        }
    }

    class PointLocatorFolderRowMapper implements RowMapper<PointLocatorFolderVO> {

        @Override
        public PointLocatorFolderVO mapRow(ResultSet rs, int rowNum) throws SQLException {
            final PointLocatorFolderVO dp = null; //(PointLocatorFolderVO) jsonMapperFactory.read(rs.getClob(5).getAsciiStream(), rs.getString(4));
            dp.setId(rs.getInt(1));
            dp.setDataSourceId(rs.getInt(2));
            dp.setName(rs.getString(3));
            return dp;
        }
    }

    @Override
    public PointLocatorVO getPointLocator(int id) {
        try {
            return ejt.queryForObject(POINT_LOCATOR_SELECT + " where id=?", new PointLocatorRowMapper(), id);
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

    @Override
    public void savePointLocator(PointLocatorVO<?> plVo) {
        // Decide whether to insert or update.
        if (plVo.isNew()) {
            insertPointLocator(plVo);
        } else {
            updatePointLocator(plVo);
        }
    }

    private void insertPointLocator(final PointLocatorVO vo) {
        final int id = doInsert(new PreparedStatementCreator() {

            final static String SQL_INSERT = "insert into pointLocators (dataSourceId, pointLocatorFolderId, clazz, name, jsonFields) values (?,?,?,?, ?)";

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                ps.setInt(1, vo.getDataSourceId());
                ps.setObject(2, vo.getPointLocatorFolderId());
                ps.setString(3, vo.getClass().getName());
                ps.setString(4, vo.getName());
//                ps.setClob(5, jsonMapperFactory.write(vo));
                return ps;
            }
        });
        vo.setId(id);
//TODO        AuditEventType.raiseAddedEvent(AuditEventKey.DATA_SOURCE, vo);
    }

    @SuppressWarnings("unchecked")
    private void updatePointLocator(final PointLocatorVO vo) {
        PointLocatorVO old = getPointLocator(vo.getId());

        ejt.update(new PreparedStatementCreator() {

            final static String SQL_UPDATE = "update\n"
                    + " pointLocators\n"
                    + "set\n"
                    + " dataSourceId=?, pointLocatorFolderId=?, clazz=?, name=?, jsonFields=?\n"
                    + "where\n"
                    + " id=?";

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareStatement(SQL_UPDATE);
                ps.setInt(1, vo.getDataSourceId());
                ps.setObject(2, vo.getPointLocatorFolderId());
                ps.setString(3, vo.getClass().getName());
                ps.setString(4, vo.getName());
//                ps.setClob(5, jsonMapperFactory.write(vo));
                ps.setInt(6, vo.getId());
                return ps;
            }
        });
//TODO        AuditEventType.raiseChangedEvent(AuditEventKey.DATA_SOURCE, old, (ChangeComparable<PointLocatorVO<?>>) vo);
    }

    @Override
    public void deletePointLocator(int plId) {
        PointLocatorVO<?> vo = getPointLocator(plId);
        if (vo != null) {
            getTransactionTemplate().execute(new TransactionCallbackWithoutResult() {
                @Override
                protected void doInTransactionWithoutResult(TransactionStatus status) {
                    ejt.update("delete from pointLocators where id=?", plId);
                }
            });

//TODO            AuditEventType.raiseDeletedEvent(AuditEventKey.DATA_SOURCE, vo);
        }
    }

}
