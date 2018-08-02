package br.org.scadabr.db.dao;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.List;

import org.springframework.transaction.TransactionStatus;
import org.springframework.transaction.support.TransactionCallbackWithoutResult;

import br.org.scadabr.vo.scripting.ScriptVO;

import com.serotonin.mango.Common;
import com.serotonin.mango.db.dao.BaseDao;
import br.org.scadabr.util.SerializationHelper;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.Statement;
import javax.inject.Named;
import javax.sql.DataSource;
import org.springframework.dao.EmptyResultDataAccessException;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.jdbc.core.PreparedStatementCreator;
import org.springframework.jdbc.core.RowMapper;

@Named
public class ScriptDao extends BaseDao {

    private static final String SCRIPT_SELECT = "select id, xid, name, script, userId, data from scripts ";

    public ScriptDao() {
        super();
    }
    
   @Deprecated
    private ScriptDao(DataSource dataSource) {
        super(dataSource);
    }

     public static ScriptDao getInstance() {
        return new ScriptDao(Common.ctx.getDatabaseAccess().getDataSource());
    }
    
    public void saveScript(final ScriptVO<?> vo) {
        // Decide whether to insert or update.
        if (vo.getId() == Common.NEW_ID) {
            insertScript(vo);
        } else {
            updateScript(vo);
        }
    }

    private void insertScript(final ScriptVO<?> vo) {
        final int id = doInsert(new PreparedStatementCreator() {

            final static String SQL_INSERT = "insert into scripts (xid, name,  script, userId, data) values (?,?,?,?,?)";

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                ps.setString(1, vo.getXid());
                ps.setString(2, vo.getName());
                ps.setString(3, vo.getScript());
                ps.setInt(4, vo.getUserId());
                ps.setBlob(5, SerializationHelper.writeObject(vo));
                return ps;
            }
        });
        vo.setId(id);
    }

    @SuppressWarnings("unchecked")
    private void updateScript(final ScriptVO<?> vo) {
        ScriptVO<?> old = getScript(vo.getId());
        ejt
                .update(new PreparedStatementCreator() {

                    @Override
                    public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                        final PreparedStatement ps = con.prepareStatement("update scripts set xid=?, name=?, script=?, userId=?, data=? where id=?");
                        ps.setString(1, vo.getXid());
                        ps.setString(2, vo.getName());
                        ps.setString(3, vo.getScript());
                        ps.setInt(4, vo.getUserId());
                        ps.setBlob(5, SerializationHelper.writeObject(vo));
                        ps.setInt(6, vo.getId());
                        return ps;
                    }
                });
    }

    public void deleteScript(final int scriptId) {
        ScriptVO<?> vo = getScript(scriptId);
        final JdbcTemplate ejt2 = ejt;
        if (vo != null) {
            getTransactionTemplate().execute(
                    new TransactionCallbackWithoutResult() {
                        @Override
                        protected void doInTransactionWithoutResult(
                                TransactionStatus status) {
                                    ejt2.update("delete from scripts where id=?",
                                            new Object[]{scriptId});
                                }
                    });
        }
    }

    public ScriptVO<?> getScript(int id) {
        try {
            return ejt.queryForObject(SCRIPT_SELECT + " where id=?", new ScriptRowMapper(), id);
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

    public List<ScriptVO<?>> getScripts() {
        List<ScriptVO<?>> scripts = ejt.query(SCRIPT_SELECT, new ScriptRowMapper());
        return scripts;
    }

    class ScriptRowMapper implements RowMapper<ScriptVO<?>> {

        @Override
        public ScriptVO<?> mapRow(ResultSet rs, int rowNum) throws SQLException {
            ScriptVO<?> script = (ScriptVO<?>) SerializationHelper
                    .readObject(rs.getBlob(6).getBinaryStream());
            script.setId(rs.getInt(1));
            script.setXid(rs.getString(2));
            script.setName(rs.getString(3));
            script.setScript(rs.getString(4));
            script.setUserId(rs.getInt(5));
            return script;
        }
    }

    public String generateUniqueXid() {
        return generateUniqueXid(ScriptVO.XID_PREFIX, "scripts");
    }

    public boolean isXidUnique(String xid, int excludeId) {
        return isXidUnique(xid, excludeId, "scripts");
    }

    public ScriptVO<?> getScript(String xid) {
        try {
            return ejt.queryForObject(SCRIPT_SELECT + " where xid=?", new ScriptRowMapper(), xid);
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

}
