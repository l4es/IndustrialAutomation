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

import br.org.scadabr.vo.EdgeType;
import br.org.scadabr.vo.NodeType;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.List;
import java.util.Random;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.jdbc.core.PreparedStatementCreator;
import org.springframework.jdbc.support.GeneratedKeyHolder;

public class BaseDao extends DaoUtils {

    public static void initNodeAndEdgeTypes(JdbcTemplate ejt, String scadaBrVersion) {
        for (NodeType nt : NodeType.values()) {
            saveNodeType(ejt, nt);
        }
        for (EdgeType et : EdgeType.values()) {
            saveEdgeType(ejt, et);
        }
    }

    public static void wireJavaClassesWithDB(JdbcTemplate ejt) {
        for (NodeType nt : NodeType.values()) {
            nt.setId(getNodeTypeByName(nt.name(), ejt));
        }
        for (EdgeType et : EdgeType.values()) {
            et.setId(getEdgeTypeByName(et.name(), ejt));
        }
    }

    //TODO duplicate code one static cleanup???
    public static void saveNodeType(JdbcTemplate ejt, final NodeType nodeType) {
        final GeneratedKeyHolder gkh = new GeneratedKeyHolder();
        final PreparedStatementCreator psc = new PreparedStatementCreator() {

            final static String SQL_INSERT = "insert into nodeTypes (typeName) values (?)";

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                ps.setString(1, nodeType.name());
                return ps;
            }
        };
        ejt.update(psc, gkh);
        nodeType.setId(gkh.getKey().intValue());
    }

    public static void saveEdgeType(JdbcTemplate ejt, final EdgeType edgeType) {
        final GeneratedKeyHolder gkh = new GeneratedKeyHolder();
        final PreparedStatementCreator psc = new PreparedStatementCreator() {

            final static String SQL_INSERT = "insert into edgeTypes (typeName) values (?)";

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                ps.setString(1, edgeType.name());
                return ps;
            }
        };
        ejt.update(psc, gkh);
        edgeType.setId(gkh.getKey().intValue());
    }

    private static Integer getNodeTypeByName(String name, JdbcTemplate ejt) {
        final Integer[] result = new Integer[1];
        ejt.query("select\n"
                + " id\n"
                + "from\n"
                + " nodeTypes\n"
                + "where\n"
                + " typeName =?", (ResultSet rs) -> {
                    result[0] = rs.getInt(1);
                }, name);
        return result[0];
    }

    private static Integer getEdgeTypeByName(String name, JdbcTemplate ejt) {
        final Integer[] result = new Integer[1];
        ejt.query("select\n"
                + " id\n"
                + "from\n"
                + " edgeTypes\n"
                + "where\n"
                + " typeName =?", (ResultSet rs) -> {
                    result[0] = rs.getInt(1);
                }, name);
        return result[0];
    }

    public void saveNodeType(final NodeType nodeType) {
        // Save the folder.
        final int id = doInsert(new PreparedStatementCreator() {

            final static String SQL_INSERT = "insert into nodeTypes (typeName) values (?)";

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                ps.setString(1, nodeType.name());
                return ps;
            }
        });
        nodeType.setId(id);
    }

    public void saveEdgeType(final EdgeType edgeType) {
        // Save the folder.
        final int id = doInsert(new PreparedStatementCreator() {

            final static String SQL_INSERT = "insert into edgeTypes (typeName) values (?)";

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                ps.setString(1, edgeType.name());
                return ps;
            }
        });
        edgeType.setId(id);
    }

    
    
    
    public final Random random = new Random();

    /**
     * Public constructor for code that needs to get stuff from the database.
     */
    public BaseDao() {
        super();
    }

    //
    // Convenience methods for storage of booleans.
    //
    @Deprecated
    protected String boolToChar(boolean b) {
        return b ? "Y" : "N";
    }

    @Deprecated
    protected boolean charToBool(String s) {
        return "Y".equals(s);
    }

    protected void deleteInChunks(String sql, List<Integer> ids) {
        int chunk = 1000;
        for (int i = 0; i < ids.size(); i += chunk) {
            String idStr = createDelimitedList(ids, i, i + chunk, ",", null);
            ejt.update(sql + " (" + idStr + ")");
        }
    }

    //
    // XID convenience methods
    //
    protected String generateUniqueXid(String prefix, String tableName) {
        String xid = generateXid(prefix);
        while (!isXidUnique(xid, tableName)) {
            xid = generateXid(prefix);
        }
        return xid;
    }

    protected boolean isXidUnique(String xid, int excludeId, String tableName) {
        return ejt.queryForObject(
                "select count(*) from " + tableName + " where xid=? and id<>?",
                Integer.class,
                xid, excludeId) == 0;
    }

    protected boolean isXidUnique(String xid, String tableName) {
        return ejt.queryForObject(
                "select count(*) from " + tableName + " where xid=?",
                Integer.class,
                xid) == 0;
    }

    public String generateXid(String prefix) {
        return prefix + generateRandomString(6, "0123456789");
    }

    public String generateRandomString(int length, String charSet) {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < length; i++) {
            sb.append(charSet.charAt(random.nextInt(charSet.length())));
        }
        return sb.toString();
    }

}
