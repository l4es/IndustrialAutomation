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

import br.org.scadabr.dao.PublisherDao;
import br.org.scadabr.rt.event.type.EventSources;
import java.io.Serializable;
import java.sql.Blob;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import org.springframework.dao.DataAccessException;
import org.springframework.transaction.TransactionStatus;
import org.springframework.transaction.support.TransactionCallbackWithoutResult;

import com.serotonin.mango.vo.publish.PublishedPointVO;
import com.serotonin.mango.vo.publish.PublisherVO;
import br.org.scadabr.util.SerializationHelper;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.Statement;
import java.util.Map;
import javax.inject.Named;
import org.springframework.dao.EmptyResultDataAccessException;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.jdbc.core.PreparedStatementCreator;
import org.springframework.jdbc.core.ResultSetExtractor;
import org.springframework.jdbc.core.RowMapper;

/**
 * @author Matthew Lohbihler
 */
@Named
public class PublisherDaoImpl extends BaseDao implements PublisherDao {

    public PublisherDaoImpl() {
        super();
    }
    
    public String generateUniqueXid() {
        return generateUniqueXid(PublisherVO.XID_PREFIX, "publishers");
    }

    @Override
    public boolean isXidUnique(String xid, int excludeId) {
        return isXidUnique(xid, excludeId, "publishers");
    }

    private static final String PUBLISHER_SELECT = "select id, xid, data from publishers ";

    @Override
    public List<PublisherVO<? extends PublishedPointVO>> getPublishers() {
        return ejt.query(PUBLISHER_SELECT, new PublisherRowMapper());
    }

    public List<PublisherVO<? extends PublishedPointVO>> getPublishers(Comparator<PublisherVO<?>> comparator) {
        List<PublisherVO<? extends PublishedPointVO>> result = getPublishers();
        Collections.sort(result, comparator);
        return result;
    }

    public static class PublisherNameComparator implements Comparator<PublisherVO<?>> {

        @Override
        public int compare(PublisherVO<?> p1, PublisherVO<?> p2) {
            return p1.getName() == null ? -1 : p1.getName().compareTo(p2.getName());
        }
    }

    @Override
    public PublisherVO<? extends PublishedPointVO> getPublisher(int id) {
        try {
            return ejt.queryForObject(PUBLISHER_SELECT + " where id=?", new PublisherRowMapper(), id);
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

    public PublisherVO<? extends PublishedPointVO> getPublisher(String xid) {
        try {
            return ejt.queryForObject(PUBLISHER_SELECT + " where xid=?", new PublisherRowMapper(), xid);
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

    class PublisherRowMapper implements RowMapper<PublisherVO<? extends PublishedPointVO>> {

        @SuppressWarnings("unchecked")
        @Override
        public PublisherVO<? extends PublishedPointVO> mapRow(ResultSet rs, int rowNum) throws SQLException {
            PublisherVO<? extends PublishedPointVO> p = (PublisherVO<? extends PublishedPointVO>) SerializationHelper
                    .readObject(rs.getBlob(3).getBinaryStream());
            p.setId(rs.getInt(1));
            p.setXid(rs.getString(2));
            return p;
        }
    }

    @Override
    public void savePublisher(final PublisherVO<? extends PublishedPointVO> vo) {
        // Decide whether to insert or update.
        if (vo.isNew()) {
            final int id = doInsert(new PreparedStatementCreator() {

                final static String SQL_INSERT = "insert into publishers (xid, data) values (?,?)";

                @Override
                public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                    PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                    ps.setString(1, vo.getXid());
                    ps.setBlob(2, SerializationHelper.writeObject(vo));
                    return ps;
                }
            });
            vo.setId(id);
        } else {
            ejt.update(new PreparedStatementCreator() {

                @Override
                public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                    final PreparedStatement ps = con.prepareStatement("update publishers set xid=?, data=? where id=?");
                    ps.setString(1, vo.getXid());
                    ps.setBlob(2, SerializationHelper.writeObject(vo));
                    ps.setInt(3, vo.getId());
                    return ps;
                }
            });
        }
    }

    @Override
    public void deletePublisher(final int publisherId) {
        final JdbcTemplate ejt2 = ejt;
        getTransactionTemplate().execute(new TransactionCallbackWithoutResult() {
            @Override
            protected void doInTransactionWithoutResult(TransactionStatus status) {
                ejt2.update("delete from eventHandlers where eventTypeId=" + EventSources.PUBLISHER.mangoDbId
                        + " and eventTypeRef1=?", new Object[]{publisherId});
                ejt2.update("delete from publishers where id=?", new Object[]{publisherId});
            }
        });
    }

    @Override
    public Map<String, Serializable> getPersistentData(final PublisherVO vo) {
        return ejt.query("select rtdata from publishers where id=?", new Object[]{vo.getId()},
                new ResultSetExtractor<Map<String, Serializable>>() {
                    @Override
                    public Map<String, Serializable> extractData(ResultSet rs) throws SQLException, DataAccessException {
                        if (!rs.next()) {
                            return null;
                        }

                        Blob blob = rs.getBlob(1);
                        if (blob == null) {
                            return null;
                        }

                        return (Map<String, Serializable>) SerializationHelper.readObject(blob.getBinaryStream());
                    }
                });
    }

    @Override
    public void savePersistentData(final PublisherVO vo, final Map<String, Serializable> data) {
        ejt.update(new PreparedStatementCreator() {

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                final PreparedStatement ps = con.prepareStatement("update publishers set rtdata=? where id=?");
                ps.setBlob(1, SerializationHelper.writeObject(data));
                ps.setInt(2, vo.getId());
                return ps;
            }
        });
    }
}
