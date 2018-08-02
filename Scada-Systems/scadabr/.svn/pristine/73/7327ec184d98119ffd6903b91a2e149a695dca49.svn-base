/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.dao.jdbc;

import br.org.scadabr.dao.NodeEdgeDao;
import br.org.scadabr.vo.EdgeConsumer;
import br.org.scadabr.vo.EdgeType;
import br.org.scadabr.vo.NodeType;
import br.org.scadabr.vo.VO;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import java.io.IOException;
import java.io.InputStream;
import java.io.StringReader;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.function.Consumer;
import javax.annotation.PostConstruct;
import javax.inject.Inject;
import javax.inject.Named;
import javax.validation.ValidationException;
import org.springframework.jdbc.core.PreparedStatementCreator;
import org.springframework.transaction.TransactionStatus;
import org.springframework.transaction.support.TransactionCallbackWithoutResult;

/**
 *
 * @author aploese
 */
@Named
public class NodeEdgeDaoImpl<T extends VO<T>> extends BaseDao implements NodeEdgeDao<T> {

    public StringReader write(Object o) {
        try {
            return new StringReader(objectMapper.writer().writeValueAsString(o));
        } catch (JsonProcessingException ex) {
            throw new RuntimeException(ex);
        }
    }

    public Object read(InputStream is) {
        try {
            return objectMapper.readValue(is, VO.class);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
    
    @Inject
    private ObjectMapper objectMapper;
    
    @PostConstruct
    @Override
    public void init() {
        super.init();
        //TODO Quick and dirty insert types move to appropirate location
//        objectMapper.registerSubtypes(PointFolderVO.class, DoubleDataPointVO.class, MetaDataSourceVO.class);
    }

    @Override
    public <U extends VO<U>> U saveNode(final U node) {
        if (node.isNew()) {
            final int id = doInsert(new PreparedStatementCreator() {

                final static String SQL_INSERT = "insert into nodes (nodeTypeId, jsonFields) values (?, ?)";

                @Override
                public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                    PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                    ps.setInt(1, node.getNodeType().getId());
                    ps.setClob(2, write(node));
                    return ps;
                }
            });
            node.setId(id);
            saveNode(node); //todo workarount to write the id to json....
        } else {

            ejt.update(new PreparedStatementCreator() {

                final static String SQL_UPDATE = "update nodes set jsonFields=? where id=?";

                @Override
                public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                    PreparedStatement ps = con.prepareStatement(SQL_UPDATE);
                    ps.setClob(1, write(node));
                    ps.setInt(2, node.getId());
                    return ps;
                }
            });
        }
        return node;
    }

    @Override
    public T getNodeById(int id) {
        return ejt.queryForObject("select\n"
                + " nodeTypeId,\n"
                + " jsonFields\n"
                + "from\n"
                + " nodes\n"
                + "where\n"
                + " id =?", (rs, rowNum) -> {
                    T result = (T) read(rs.getClob(2).getAsciiStream()); 
                    if (!result.getNodeType().getId().equals(rs.getInt(1))) {
                        throw new ValidationException("Wrong nodetype of: " + id);
                    }
                    if (!result.getId().equals(id)) {
                        throw new ValidationException("Wrong id of: " + id);
                    }
                    return result;
                }, id);
    }

    @Override
    public boolean isNodeExisting(int id) {
        return 1 == ejt.queryForObject("select count(1) from nodes where id=?", Integer.class, id);
    }

    @Override
    public boolean deleteNode(int id) {
        getTransactionTemplate().execute(new TransactionCallbackWithoutResult() {
            @Override
            protected void doInTransactionWithoutResult(TransactionStatus status) {
                ejt.update("delete from nodes where id=?", id);
            }
        });
        return true;
    }

    @Override
    public long countNodes() {
        return ejt.queryForObject("select count(1) from nodes", Long.class);
    }

    @Override
    public long countNodes(NodeType nodeType) {
        return ejt.queryForObject("select count(1) from nodes where nodeTypeId=?", Long.class, nodeType.getId());
    }

    @Override
    public void wipeDB() {
        ejt.execute("delete from edges");
        ejt.execute("delete from nodes");
    }

    @Override
    public void forEachNode(Consumer<T> action) {
        ejt.query("select\n"
                + " id,\n"
                + " nodeTypeId,\n"
                + " jsonFields\n"
                + "from\n"
                + " nodes\n", (resultSet) -> {
                    T result = (T) read(resultSet.getClob(3).getAsciiStream());
                    if (!result.getNodeType().getId().equals(resultSet.getInt(2))) {
                        throw new RuntimeException("Wrong nodetype of: " + resultSet.getInt(1));
                    }
                    if (!result.getId().equals(resultSet.getInt(1))) {
                        throw new ValidationException("Wrong id of: " + resultSet.getInt(1));
                    }
                    action.accept(result);
                });
    }

    @Override
    public void saveEdge(int srcNodeId, int destNodeId, EdgeType edgeType) {
        final int id = ejt.update(new PreparedStatementCreator() {

            final static String SQL_INSERT = "insert into edges (srcNodeId, destNodeId, edgeTypeId) values (?, ?, ?)";

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareStatement(SQL_INSERT);
                ps.setInt(1, srcNodeId);
                ps.setInt(2, destNodeId);
                ps.setInt(3, edgeType.getId());
                return ps;
            }
        });
    }

    @Override
    public boolean isEdgeExisting(int srcNodeId, int destNodeId, EdgeType edgeType) {
        return 1 == ejt.queryForObject("select\n"
                + " count(1)\n"
                + "from\n"
                + " edges\n"
                + "where\n"
                + " srcNodeId=? and destNodeId=? and edgeTypeId=?", Integer.class, srcNodeId, destNodeId, edgeType.getId());
    }

    @Override
    public boolean deleteEdge(int srcNodeId, int destNodeId, EdgeType edgeType) {
        getTransactionTemplate().execute(new TransactionCallbackWithoutResult() {
            @Override
            protected void doInTransactionWithoutResult(TransactionStatus status) {
                ejt.update("delete\n"
                        + "from\n"
                        + " edges\n"
                        + "where\n"
                        + " srcNodeId=? and destNodeId=? and edgeTypeId=?", srcNodeId, destNodeId, edgeType.getId());
            }
        });
        return true;
    }

    @Override
    public void forEachEdge(EdgeConsumer action) {
        ejt.query("select\n"
                + " srcNodeId,\n"
                + " destNodeId,\n"
                + " edgeTypeId\n"
                + "from\n"
                + " edges\n", (rs) -> {
                    action.accept(rs.getInt(1), rs.getInt(2), EdgeType.fromId(rs.getInt(3)));
                });
    }

    @Override
    public long countEdges() {
        return ejt.queryForObject("select count(1) from edges", Long.class);
    }

    @Override
    public long countEdges(EdgeType edgeType) {
        return ejt.queryForObject("select count(1) from edges where edgeTypeId=?", Long.class, edgeType.getId());
    }
}
