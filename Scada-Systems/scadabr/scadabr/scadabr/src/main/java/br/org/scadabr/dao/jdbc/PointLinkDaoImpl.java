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

import br.org.scadabr.dao.PointLinkDao;
import br.org.scadabr.vo.event.type.AuditEventKey;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.List;

import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.vo.link.PointLinkVO;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.Statement;
import javax.inject.Named;
import org.springframework.dao.EmptyResultDataAccessException;
import org.springframework.jdbc.core.PreparedStatementCreator;
import org.springframework.jdbc.core.RowMapper;

/**
 * @author Matthew Lohbihler
 */
@Named
public class PointLinkDaoImpl extends BaseDao implements PointLinkDao {

    public PointLinkDaoImpl() {
        super();
    }

    public String generateUniqueXid() {
        return generateUniqueXid(PointLinkVO.XID_PREFIX, "pointLinks");
    }

    public boolean isXidUnique(String xid, int excludeId) {
        return isXidUnique(xid, excludeId, "pointLinks");
    }

    private static final String POINT_LINK_SELECT = "select id, xid, sourcePointId, targetPointId, script, eventType, disabled from pointLinks ";

    public List<PointLinkVO> getPointLinks() {
        return ejt.query(POINT_LINK_SELECT, new PointLinkRowMapper());
    }

    public List<PointLinkVO> getPointLinksForPoint(int dataPointId) {
        return ejt.query(POINT_LINK_SELECT + "where sourcePointId=? or targetPointId=?",
                new PointLinkRowMapper(), dataPointId, dataPointId);
    }

    public PointLinkVO getPointLink(int id) {
        try {
            return ejt.queryForObject(POINT_LINK_SELECT + "where id=?", new PointLinkRowMapper(), id);
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

    public PointLinkVO getPointLink(String xid) {
        try {
            return ejt.queryForObject(POINT_LINK_SELECT + "where xid=?", new PointLinkRowMapper(), xid);
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

    class PointLinkRowMapper implements RowMapper<PointLinkVO> {

        @Override
        public PointLinkVO mapRow(ResultSet rs, int rowNum) throws SQLException {
            PointLinkVO pl = new PointLinkVO();
            int i = 0;
            pl.setId(rs.getInt(++i));
            pl.setXid(rs.getString(++i));
            pl.setSourcePointId(rs.getInt(++i));
            pl.setTargetPointId(rs.getInt(++i));
            pl.setScript(rs.getString(++i));
            pl.setEvent(PointLinkVO.EventType.fromId(rs.getInt(++i)));
            pl.setDisabled(charToBool(rs.getString(++i)));
            return pl;
        }
    }

    public void savePointLink(final PointLinkVO pl) {
        if (pl.isNew()) {
            insertPointLink(pl);
        } else {
            updatePointLink(pl);
        }
    }

    private void insertPointLink(final PointLinkVO pl) {
        final int id = doInsert(new PreparedStatementCreator() {

            final static String SQL_INSERT = "insert into pointLinks (xid, sourcePointId, targetPointId, script, eventType, disabled) "
                    + "values (?,?,?,?,?,?)";

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                ps.setString(1, pl.getXid());
                ps.setInt(2, pl.getSourcePointId());
                ps.setInt(3, pl.getTargetPointId());
                ps.setString(4, pl.getScript());
                ps.setInt(5, pl.getEvent().getId());
                ps.setString(6, boolToChar(pl.isDisabled()));
                return ps;
            }
        });
        pl.setId(id);
        AuditEventType.raiseAddedEvent(AuditEventKey.POINT_LINK, pl);
    }

    private static final String POINT_LINK_UPDATE = "update pointLinks set xid=?, sourcePointId=?, targetPointId=?, script=?, eventType=?, disabled=? "
            + "where id=?";

    private void updatePointLink(PointLinkVO pl) {
        PointLinkVO old = getPointLink(pl.getId());

        ejt.update(POINT_LINK_UPDATE, pl.getXid(), pl.getSourcePointId(), pl.getTargetPointId(), pl.getScript(),
                pl.getEvent(), boolToChar(pl.isDisabled()), pl.getId());

        AuditEventType.raiseChangedEvent(AuditEventKey.POINT_LINK, old, pl);
    }

    public void deletePointLink(final int pointLinkId) {
        PointLinkVO pl = getPointLink(pointLinkId);
        if (pl != null) {
            ejt.update("delete from pointLinks where id=?", pointLinkId);
            AuditEventType.raiseDeletedEvent(AuditEventKey.POINT_LINK, pl);
        }
    }
}
