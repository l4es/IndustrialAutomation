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
import br.org.scadabr.dao.WatchListDao;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.view.SharedUserAcess;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.List;

import org.springframework.jdbc.core.BatchPreparedStatementSetter;
import org.springframework.transaction.TransactionStatus;
import org.springframework.transaction.support.TransactionCallbackWithoutResult;

import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.WatchList;
import java.sql.Connection;
import java.sql.Statement;
import javax.inject.Inject;
import javax.inject.Named;
import org.springframework.dao.EmptyResultDataAccessException;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.jdbc.core.PreparedStatementCreator;
import org.springframework.jdbc.core.RowMapper;

/**
 * @author Matthew Lohbihler
 */
@Named
public class WatchListDaoImpl extends BaseDao implements WatchListDao {

    @Inject
    private DataPointDao dataPointDao;
    
    public WatchListDaoImpl() {
        super();
    }
    
    public String generateUniqueXid() {
        return generateUniqueXid(WatchList.XID_PREFIX, "watchLists");
    }

    public boolean isXidUnique(String xid, int watchListId) {
        return isXidUnique(xid, watchListId, "watchLists");
    }

    /**
     * Note: this method only returns basic watchlist information. No data
     * points or share users.
     */
    public List<WatchList> getWatchLists(final int userId) {
        return ejt.query("select id, xid, userId, name from watchLists " //
                + "where userId=? or id in (select watchListId from watchListUsers where userId=?) " //
                + "order by name", new WatchListRowMapper(), userId, userId);
    }

    /**
     * Note: this method only returns basic watchlist information. No data
     * points or share users.
     */
    public List<WatchList> getWatchLists() {
        return ejt.query("select id, xid, userId, name from watchLists", new WatchListRowMapper());
    }

    public WatchList getWatchList(int watchListId) {
        // Get the watch lists.
        WatchList watchList = ejt.queryForObject("select id, xid, userId, name from watchLists where id=?",
                new WatchListRowMapper(), watchListId);
        populateWatchlistData(watchList);
        return watchList;
    }

    public void populateWatchlistData(List<WatchList> watchLists) {
        for (WatchList watchList : watchLists) {
            populateWatchlistData(watchList);
        }
    }

    public void populateWatchlistData(WatchList watchList) {
        if (watchList == null) {
            return;
        }

        // Get the points for each of the watch lists.
        List<Integer> pointIds = ejt.queryForList(
                "select dataPointId from watchListPoints where watchListId=? order by sortOrder",
                new Object[]{watchList.getId()}, Integer.class);
        final List<DataPointVO> points = watchList.getPointList();
        for (Integer pointId : pointIds) {
            points.add(dataPointDao.getDataPoint(pointId));
        }
        setWatchListUsers(watchList);
    }

    /**
     * Note: this method only returns basic watchlist information. No data
     * points or share users.
     */
    public WatchList getWatchList(String xid) {
        try {
            return ejt.queryForObject("select id, xid, userId, name from watchLists where xid=?", new WatchListRowMapper(), xid);
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

    class WatchListRowMapper implements RowMapper<WatchList> {

        @Override
        public WatchList mapRow(ResultSet rs, int rowNum) throws SQLException {
            WatchList wl = new WatchList();
            wl.setId(rs.getInt(1));
            wl.setXid(rs.getString(2));
            wl.setUserId(rs.getInt(3));
            wl.setName(rs.getString(4));
            return wl;
        }
    }

    public void saveSelectedWatchList(int userId, int watchListId) {
        ejt.update("update users set selectedWatchList=? where id=?", new Object[]{watchListId, userId});
    }

    public WatchList createNewWatchList(final WatchList watchList, final int userId) {
        watchList.setUserId(userId);
        watchList.setXid(generateUniqueXid());
        final int id = doInsert(new PreparedStatementCreator() {

            final static String SQL_INSERT = "insert into watchLists (xid, userId, name) values (?,?,?)";

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                ps.setString(1, watchList.getXid());
                ps.setInt(2, userId);
                ps.setString(3, watchList.getName());
                return ps;
            }
        });
        watchList.setId(id);
        return watchList;
    }

    public void saveWatchList(final WatchList watchList) {
        final JdbcTemplate ejt2 = ejt;
        getTransactionTemplate().execute(new TransactionCallbackWithoutResult() {
            @SuppressWarnings("synthetic-access")
            @Override
            protected void doInTransactionWithoutResult(TransactionStatus status) {
                if (watchList.isNew()) {
                    final int id = doInsert(new PreparedStatementCreator() {

                        final static String SQL_INSERT = "insert into watchLists (xid, name, userId) values (?,?,?)";

                        @Override
                        public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                            PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                            ps.setString(1, watchList.getXid());
                            ps.setString(2, watchList.getName());
                            ps.setInt(3, watchList.getUserId());
                            return ps;
                        }
                    });
                    watchList.setId(id);
                } else {
                    ejt2.update("update watchLists set xid=?, name=? where id=?", new Object[]{watchList.getXid(),
                        watchList.getName(), watchList.getId()});
                }
                ejt2.update("delete from watchListPoints where watchListId=?", new Object[]{watchList.getId()});
                ejt2.batchUpdate("insert into watchListPoints values (?,?,?)", new BatchPreparedStatementSetter() {
                    @Override
                    public int getBatchSize() {
                        return watchList.getPointList().size();
                    }

                    @Override
                    public void setValues(PreparedStatement ps, int i) throws SQLException {
                        ps.setInt(1, watchList.getId());
                        ps.setInt(2, watchList.getPointList().get(i).getId());
                        ps.setInt(3, i);
                    }
                });

                saveWatchListUsers(watchList);
            }
        });
    }

    public void deleteWatchList(final int watchListId) {
        final JdbcTemplate ejt2 = ejt;
        getTransactionTemplate().execute(new TransactionCallbackWithoutResult() {
            @Override
            protected void doInTransactionWithoutResult(TransactionStatus status) {
                deleteWatchListUsers(watchListId);
                ejt2.update("delete from watchListPoints where watchListId=?", new Object[]{watchListId});
                ejt2.update("delete from watchLists where id=?", new Object[]{watchListId});
            }
        });
    }

    //
    //
    // Watch list users
    //
    private void setWatchListUsers(WatchList watchList) {
        throw new ImplementMeException();
        /*
        watchList.setWatchListUsers(ejt.query("select userId, accessType from watchListUsers where watchListId=?",
                new WatchListUserRowMapper(), watchList.getId()));
                */
    }

    class WatchListUserRowMapper implements RowMapper<SharedUserAcess> {

        @Override
        public SharedUserAcess mapRow(ResultSet rs, int rowNum) throws SQLException {
                    throw new ImplementMeException();
        /*

            ShareUser wlu = new ShareUser();
            wlu.setUserId(rs.getInt(1));
            wlu.setAccessType(rs.getInt(2));
            return wlu;
                    */
        }
    }

    void deleteWatchListUsers(int watchListId) {
        ejt.update("delete from watchListUsers where watchListId=?", new Object[]{watchListId});
    }

    void saveWatchListUsers(final WatchList watchList) {
        throw new ImplementMeException();
        /*

                // Delete anything that is currently there.
        deleteWatchListUsers(watchList.getId());

        // Add in all of the entries.
        ejt.batchUpdate("insert into watchListUsers values (?,?,?)", new BatchPreparedStatementSetter() {
            @Override
            public int getBatchSize() {
                return watchList.getWatchListUsers().size();
            }

            @Override
            public void setValues(PreparedStatement ps, int i) throws SQLException {
                ShareUser wlu = watchList.getWatchListUsers().get(i);
                ps.setInt(1, watchList.getId());
                ps.setInt(2, wlu.getUserId());
                ps.setInt(3, wlu.getAccessType());
            }
        });
                */
    }

    public void removeUserFromWatchList(int watchListId, int userId) {
        ejt.update("delete from watchListUsers where watchListId=? and userId=?", new Object[]{watchListId, userId});
    }
}
