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

import br.org.scadabr.dao.ViewDao;
import java.sql.Blob;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.List;

import org.springframework.jdbc.core.BatchPreparedStatementSetter;
import org.springframework.transaction.TransactionStatus;
import org.springframework.transaction.support.TransactionCallbackWithoutResult;

import br.org.scadabr.db.IntValuePair;
import br.org.scadabr.db.spring.IntValuePairRowMapper;
import com.serotonin.mango.view.ShareUser;
import com.serotonin.mango.view.View;
import br.org.scadabr.util.SerializationHelper;
import java.sql.Connection;
import java.sql.Statement;
import javax.inject.Named;
import org.springframework.dao.EmptyResultDataAccessException;
import org.springframework.jdbc.core.PreparedStatementCreator;
import org.springframework.jdbc.core.RowMapper;

@Named
public class ViewDaoImpl extends BaseDao implements ViewDao {

    //
    // /
    // / Views
    // /
    //
    private static final String VIEW_SELECT = "select data, id, xid, name, background, userId, anonymousAccess from mangoViews";
    private static final String USER_ID_COND = " where userId=? or id in (select mangoViewId from mangoViewUsers where userId=?)";

    public ViewDaoImpl() {
        super();
    }
    
    public List<View> getViews() {
        List<View> views = ejt.query(VIEW_SELECT, new ViewRowMapper());
        setViewUsers(views);
        return views;
    }

    public List<View> getViews(int userId) {
        List<View> views = ejt.query(VIEW_SELECT + USER_ID_COND, new ViewRowMapper(), userId, userId);
        setViewUsers(views);
        return views;
    }

    public List<IntValuePair> getViewNames(int userId) {
        return ejt.query("select id, name from mangoViews" + USER_ID_COND, new IntValuePairRowMapper(), userId, userId);
    }

    private void setViewUsers(List<View> views) {
        for (View view : views) {
            setViewUsers(view);
        }
    }

    public View getView(int id) {
        return getSingleView(VIEW_SELECT + " where id=?", id);
    }

    public View getViewByXid(String xid) {
        return getSingleView(VIEW_SELECT + " where xid=?", xid);
    }

    public View getView(String name) {
        return getSingleView(VIEW_SELECT + " where name=?", name);
    }

    private View getSingleView(String sql, Object... params) {
        try {
            View view = ejt.queryForObject(sql, new ViewRowMapper(), params);
            setViewUsers(view);
            return view;
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

    class ViewRowMapper implements RowMapper<View> {

        @Override
        public View mapRow(ResultSet rs, int rowNum) throws SQLException {
            View v;
            Blob blob = rs.getBlob(1);
            if (blob == null) // This can happen during upgrade
            {
                v = new View();
            } else {
                v = (View) SerializationHelper.readObject(blob.getBinaryStream());
            }

            v.setId(rs.getInt(2));
            v.setXid(rs.getString(3));
            v.setName(rs.getString(4));
            v.setBackgroundFilename(rs.getString(5));
            v.setUserId(rs.getInt(6));
            v.setAnonymousAccess(rs.getInt(7));

            return v;
        }
    }

    class ViewNameRowMapper implements RowMapper<View> {

        @Override
        public View mapRow(ResultSet rs, int rowNum) throws SQLException {
            View v = new View();
            v.setId(rs.getInt(1));
            v.setName(rs.getString(2));
            v.setUserId(rs.getInt(3));

            return v;
        }
    }

    public String generateUniqueXid() {
        return generateUniqueXid(View.XID_PREFIX, "mangoViews");
    }

    public boolean isXidUnique(String xid, int excludeId) {
        return isXidUnique(xid, excludeId, "mangoViews");
    }

    public void saveView(final View view) {
        getTransactionTemplate().execute(new TransactionCallbackWithoutResult() {
            @Override
            protected void doInTransactionWithoutResult(TransactionStatus status) {
                // Decide whether to insert or update.
                if (view.isNew()) {
                    insertView(view);
                } else {
                    updateView(view);
                }

                saveViewUsers(view);
            }
        });
    }

    void insertView(final View view) {
        final int id = doInsert(new PreparedStatementCreator() {

            final static String SQL_INSERT = "insert into mangoViews (xid, name, background, userId, anonymousAccess, data) values (?,?,?,?,?,?)";

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                ps.setString(1, view.getXid());
                ps.setString(2, view.getName());
                ps.setString(3, view.getBackgroundFilename());
                ps.setInt(4, view.getUserId());
                ps.setInt(5, view.getAnonymousAccess());
                ps.setBlob(6, SerializationHelper.writeObject(view));
                return ps;
            }
        });
        view.setId(id);
    }

    void updateView(final View view) {
        ejt.update(new PreparedStatementCreator() {

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareStatement("update mangoViews set xid=?, name=?, background=?, anonymousAccess=?, data=? where id=?");
                ps.setString(1, view.getXid());
                ps.setString(2, view.getName());
                ps.setString(3, view.getBackgroundFilename());
                ps.setInt(4, view.getAnonymousAccess());
                ps.setBlob(5, SerializationHelper.writeObject(view));
                ps.setInt(6, view.getId());
                return ps;
            }
        });
    }

    public void removeView(final int viewId) {
        deleteViewUsers(viewId);
        ejt.update("delete from mangoViews where id=?", new Object[]{viewId});
    }

    //
    // /
    // / View users
    // /
    //
    private void setViewUsers(View view) {
        view.setViewUsers(ejt.query("select userId, accessType from mangoViewUsers where mangoViewId=?",
                new ViewUserRowMapper(), view.getId()));
    }

    class ViewUserRowMapper implements RowMapper<ShareUser> {

        @Override
        public ShareUser mapRow(ResultSet rs, int rowNum) throws SQLException {
            ShareUser vu = new ShareUser();
            vu.setUserId(rs.getInt(1));
            vu.setAccessType(rs.getInt(2));
            return vu;
        }
    }

    private void deleteViewUsers(int viewId) {
        ejt.update("delete from mangoViewUsers where mangoViewId=?", viewId);
    }

    void saveViewUsers(final View view) {
        // Delete anything that is currently there.
        deleteViewUsers(view.getId());

        // Add in all of the entries.
        ejt.batchUpdate("insert into mangoViewUsers values (?,?,?)", new BatchPreparedStatementSetter() {
            @Override
            public int getBatchSize() {
                return view.getViewUsers().size();
            }

            @Override
            public void setValues(PreparedStatement ps, int i) throws SQLException {
                ShareUser vu = view.getViewUsers().get(i);
                ps.setInt(1, view.getId());
                ps.setInt(2, vu.getUserId());
                ps.setInt(3, vu.getAccessType());
            }
        });
    }

    public void removeUserFromView(int viewId, int userId) {
        ejt.update("delete from mangoViewUsers where mangoViewId=? and userId=?", viewId, userId);
    }
}
