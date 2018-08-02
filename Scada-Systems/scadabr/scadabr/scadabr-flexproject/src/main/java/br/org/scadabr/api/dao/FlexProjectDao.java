package br.org.scadabr.api.dao;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Types;
import java.util.List;

import org.springframework.transaction.TransactionStatus;
import org.springframework.transaction.support.TransactionCallbackWithoutResult;

import br.org.scadabr.api.vo.FlexProject;

import com.serotonin.mango.Common;
import com.serotonin.mango.db.dao.BaseDao;
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
public class FlexProjectDao extends BaseDao {

    private static final String FLEX_PROJECT_SELECT = "select id, name, description, xmlConfig from flexProjects ";

    public FlexProjectDao() {
        super();
    }
    
   @Deprecated
    private FlexProjectDao(DataSource dataSource) {
        super(dataSource);
    }

     public static FlexProjectDao getInstance() {
        return new FlexProjectDao(Common.ctx.getDatabaseAccess().getDataSource());
    }
    
    public int saveFlexProject(int id, String name, String description,
            String xmlConfig) {
        if (id == Common.NEW_ID) {
            return insertFlexProject(name, description, xmlConfig);
        } else {
            return updateFlexProject(id, name, description, xmlConfig);
        }
    }

    private int insertFlexProject(final String name, final String description, final String xmlConfig) {
        return doInsert(new PreparedStatementCreator() {

            final static String SQL_INSERT = "insert into flexProjects (name,  description, xmlConfig) values (?,?,?)";

            @Override
            public PreparedStatement createPreparedStatement(Connection con) throws SQLException {
                PreparedStatement ps = con.prepareStatement(SQL_INSERT, Statement.RETURN_GENERATED_KEYS);
                ps.setString(1, name);
                ps.setString(2, description);
                ps.setString(3, xmlConfig);
                return ps;
            }
        });
    }

    private int updateFlexProject(int id, String name, String description,
            String xmlConfig) {
        ejt
                .update(
                        "update flexProjects set name=?, description=?, xmlConfig=? where id=?",
                        new Object[]{name, description, xmlConfig, id},
                        new int[]{Types.VARCHAR, Types.VARCHAR,
                            Types.VARCHAR, Types.INTEGER});
        return id;

    }

    public void deleteFlexProject(final int flexProjectId) {
        final JdbcTemplate ejt2 = ejt;
        getTransactionTemplate().execute(
                new TransactionCallbackWithoutResult() {
                    @Override
                    protected void doInTransactionWithoutResult(
                            TransactionStatus status) {
                                ejt2.update("delete from flexProjects where id=?",
                                        new Object[]{flexProjectId});
                            }
                });
    }

    public FlexProject getFlexProject(int id) {
        try {
            return ejt.queryForObject(FLEX_PROJECT_SELECT + " where id=?", new FlexProjectRowMapper(), id);
        } catch (EmptyResultDataAccessException e) {
            return null;
        }
    }

    public List<FlexProject> getFlexProjects() {
        List<FlexProject> flexProjects = ejt.query(FLEX_PROJECT_SELECT,
                new FlexProjectRowMapper());
        return flexProjects;
    }

    class FlexProjectRowMapper implements RowMapper<FlexProject> {

        @Override
        public FlexProject mapRow(ResultSet rs, int rowNum) throws SQLException {
            FlexProject project = new FlexProject();
            project.setId(rs.getInt(1));
            project.setName(rs.getString(2));
            project.setDescription(rs.getString(3));
            project.setXmlConfig(rs.getString(4));
            return project;
        }
    }

}
