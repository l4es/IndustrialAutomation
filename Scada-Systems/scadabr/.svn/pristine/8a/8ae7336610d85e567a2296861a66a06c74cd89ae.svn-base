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

import br.org.scadabr.dao.UserDao;
import br.org.scadabr.rt.UserRT;
import br.org.scadabr.util.StringUtils;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.vo.event.AlarmLevel;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.List;

import org.springframework.jdbc.core.BatchPreparedStatementSetter;
import org.springframework.transaction.TransactionStatus;
import org.springframework.transaction.support.TransactionCallbackWithoutResult;

import com.serotonin.mango.Common;
import com.serotonin.mango.rt.event.AlternateAcknowledgementSources;
import com.serotonin.mango.vo.UserComment;
import com.serotonin.mango.vo.permission.DataPointAccess;
import java.sql.Connection;
import java.sql.Statement;
import java.util.Collection;
import javax.annotation.PostConstruct;
import javax.inject.Named;
import org.springframework.dao.EmptyResultDataAccessException;
import org.springframework.jdbc.core.PreparedStatementCreator;
import org.springframework.jdbc.core.RowMapper;

@Named
@Deprecated //TODO move to node / edge dao
public class UserDaoImpl extends BaseDao implements UserDao {

    private static final String USER_SELECT = "select id, username, password, email, phone, admin, disabled, selectedWatchList, homeUrl, lastLogin, "
            + "  receiveAlarmEmails, receiveOwnAuditEvents " + "from users ";

    public UserDaoImpl() {
        super();
    }

    @PostConstruct
    @Override
    public void init() {
        super.init();
        if (getUser("admin") == null) {
            LOG.severe("No user admin ... will create");
        ejt.execute(String.format("insert into users (username, password, email, phone, admin, disabled, homeUrl, receiveAlarmEmails, receiveOwnAuditEvents)"
                + "values ('admin', '%s', 'admin@yourScadaBRDomain.com', '', 'Y', 'N', 'events', 0, 'N')", Common.encrypt("admin")));
        }
            LOG.severe("User admin ... created");
    }
    
    @Override
    public UserRT getUser(int id) {
        throw new ImplementMeException();
        /*
        try {
            UserRT user = ejt.queryForObject(USER_SELECT + "where id=?", new UserRowMapper(), id);
            populateUserPermissions(user);
            return user;
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
*/
    }

    @Override
    public UserRT getUser(String username) {
        throw new ImplementMeException();
        /*
        try {
            UserRT user = ejt.queryForObject(USER_SELECT + "where lower(username)=?", new UserRowMapper(), username.toLowerCase());
            populateUserPermissions(user);
            return user;
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
*/    }

    class UserRowMapper implements RowMapper<UserRT> {

        @Override
        public UserRT mapRow(ResultSet rs, int rowNum) throws SQLException {
        throw new ImplementMeException();
        /*
            UserVO user = new UserVO();
            int i = 0;
            user.setId(rs.getInt(++i));
            user.setUsername(rs.getString(++i));
            user.setPassword(rs.getString(++i));
            user.setEmail(rs.getString(++i));
            user.setPhone(rs.getString(++i));
            user.setAdmin(charToBool(rs.getString(++i)));
            user.setDisabled(charToBool(rs.getString(++i)));
            user.setSelectedWatchList(rs.getInt(++i));
            user.setHomeUrl(rs.getString(++i));
            user.setLastLogin(rs.getLong(++i));
            AlarmLevel l = AlarmLevel.values()[rs.getInt(++i)];
            user.setReceiveAlarmEmails(l == AlarmLevel.NONE ? null : l);
            user.setReceiveOwnAuditEvents(charToBool(rs.getString(++i)));
            return user;
*/
        }
    }

    @Override
    public Collection<UserRT> getUsers() {
      throw new ImplementMeException();
        /*
          List<UserVO> users = ejt.query(USER_SELECT + "order by username", new Object[0], new UserRowMapper());
        populateUserPermissions(users);
        return users;
*/
    }

    @Override
    public Collection<UserRT> getActiveUsers() {
      throw new ImplementMeException();
        /*
          List<UserVO> users = ejt.query(USER_SELECT + "where disabled=?", new Object[]{boolToChar(false)},
                new UserRowMapper());
        populateUserPermissions(users);
        return users;
        */
      }

    private void populateUserPermissions(List<UserRT> users) {
      throw new ImplementMeException();
        /*
          users.stream().forEach((user) -> {
            populateUserPermissions(user);
        });
*/
    }

    private static final String SELECT_DATA_SOURCE_PERMISSIONS = "select dataSourceId from dataSourceUsers where userId=?";
    private static final String SELECT_DATA_POINT_PERMISSIONS = "select dataPointId, permission from dataPointUsers where userId=?";

    public void populateUserPermissions(UserRT user) {
      throw new ImplementMeException();
        /*
  
        user.setDataSourcePermissions(ejt.queryForList(SELECT_DATA_SOURCE_PERMISSIONS, new Object[]{user.getId()},
                Integer.class));
        user.setDataPointPermissions(ejt.query(SELECT_DATA_POINT_PERMISSIONS, (ResultSet rs, int rowNum) -> {
            DataPointAccess a = new DataPointAccess();
            a.setDataPointId(rs.getInt(1));
            a.setPermission(rs.getInt(2));
            return a;
        }, user.getId()));
*/
    }

    public void saveUser(final UserRT user) {
      throw new ImplementMeException();
        /*
          getTransactionTemplate().execute(new TransactionCallbackWithoutResult() {
            @Override
            protected void doInTransactionWithoutResult(TransactionStatus status) {
                if (user.isNew()) {
                    insertUser(user);
                } else {
                    updateUser(user);
                }
            }
        });
*/
    }

    private void insertUser(final UserRT user) {
      throw new ImplementMeException();
        /*
          final int id = doInsert(new PreparedStatementCreator() {

            final static String SQL_INSERT = "insert into users ("
                    + "  username, password, email, phone, admin, disabled, homeUrl, receiveAlarmEmails, receiveOwnAuditEvents) "
                    + "values (?,?,?,?,?,?,?,?,?)";

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                ps.setString(1, user.getUsername());
                ps.setString(2, user.getPassword());
                ps.setString(3, user.getEmail());
                ps.setString(4, user.getPhone());
                ps.setString(5, boolToChar(user.isAdmin()));
                ps.setString(6, boolToChar(user.isDisabled()));
                ps.setString(7, user.getHomeUrl());
                ps.setInt(8, user.getReceiveAlarmEmails().ordinal());
                ps.setString(9, boolToChar(user.isReceiveOwnAuditEvents()));
                return ps;
            }
        });
        user.setId(id);
        saveRelationalData(user);
*/    }

    private static final String USER_UPDATE = "update users set "
            + "  username=?, password=?, email=?, phone=?, admin=?, disabled=?, homeUrl=?, receiveAlarmEmails=?, "
            + "  receiveOwnAuditEvents=? " + "where id=?";

    void updateUser(UserRT user) {
      throw new ImplementMeException();
        /*
          ejt.update(
                USER_UPDATE,
                new Object[]{user.getUsername(), user.getPassword(), user.getEmail(), user.getPhone(),
                    boolToChar(user.isAdmin()), boolToChar(user.isDisabled()), user.getHomeUrl(),
                    user.getReceiveAlarmEmails(), boolToChar(user.isReceiveOwnAuditEvents()), user.getId()});
        saveRelationalData(user);
*/    }

    private void saveRelationalData(final UserRT user) {
      throw new ImplementMeException();
        /*
          // Delete existing permissions.
        ejt.update("delete from dataSourceUsers where userId=?", new Object[]{user.getId()});
        ejt.update("delete from dataPointUsers where userId=?", new Object[]{user.getId()});

        // Save the new ones.
        ejt.batchUpdate("insert into dataSourceUsers (dataSourceId, userId) values (?,?)",
                new BatchPreparedStatementSetter() {
                    @Override
                    public int getBatchSize() {
                        return user.getDataSourcePermissions().size();
                    }

                    @Override
                    public void setValues(PreparedStatement ps, int i) throws SQLException {
                        ps.setInt(1, user.getDataSourcePermissions().get(i));
                        ps.setInt(2, user.getId());
                    }
                });
        ejt.batchUpdate("insert into dataPointUsers (dataPointId, userId, permission) values (?,?,?)",
                new BatchPreparedStatementSetter() {
                    @Override
                    public int getBatchSize() {
                        return user.getDataPointPermissions().size();
                    }

                    @Override
                    public void setValues(PreparedStatement ps, int i) throws SQLException {
                        ps.setInt(1, user.getDataPointPermissions().get(i).getDataPointId());
                        ps.setInt(2, user.getId());
                        ps.setInt(3, user.getDataPointPermissions().get(i).getPermission());
                    }
                });
*/    }

    public void deleteUser(final int userId) {
      throw new ImplementMeException();
        /*
          getTransactionTemplate().execute(new TransactionCallbackWithoutResult() {
            @SuppressWarnings("synthetic-access")
            @Override
            protected void doInTransactionWithoutResult(TransactionStatus status) {
                Object[] args = new Object[]{userId};
                ejt.update("update userComments set userId=null where userId=?", args);
                ejt.update("delete from mailingListMembers where userId=?", args);
                ejt.update("update pointValueAnnotations set sourceId=null where sourceId=? and sourceType="
                        + SetPointSource.Type.USER, args);
                ejt.update("delete from userEvents where userId=?", args);
                ejt.update("update events set ackUserId=null, alternateAckSource="
                        + AlternateAcknowledgementSources.DELETED_USER + " where ackUserId=?", args);
                ejt.update("delete from users where id=?", args);
            }
        });
*/    }

    @Override
    public void recordLogin(int userId) {
        ejt.update("update users set lastLogin=? where id=?", new Object[]{System.currentTimeMillis(), userId});
    }

    @Override
    public void saveHomeUrl(int userId, String homeUrl) {
        ejt.update("update users set homeUrl=? where id=?", new Object[]{homeUrl, userId});
    }

    //
    //
    // User comments
    //
    private static final String USER_COMMENT_INSERT = "insert into userComments (userId, commentType, typeKey, ts, commentText) "
            + "values (?,?,?,?,?)";

    @Override
    public void insertUserComment(int typeId, int referenceId, UserComment comment) {
        comment.setComment(StringUtils.truncate(comment.getComment(), 1024));
        ejt.update(USER_COMMENT_INSERT, new Object[]{comment.getUserId(), typeId, referenceId, comment.getTs(),
            comment.getComment()});
    }
}
