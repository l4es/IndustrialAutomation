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
package br.org.scadabr.jdbc;

import java.io.OutputStream;
import java.sql.Connection;
import java.sql.SQLException;

import javax.sql.DataSource;

import org.springframework.jdbc.CannotGetJdbcConnectionException;
import org.springframework.jdbc.datasource.DataSourceUtils;

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.dao.jdbc.BaseDao;
import br.org.scadabr.logger.LogUtils;
import java.util.Properties;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.springframework.jdbc.core.ConnectionCallback;
import org.springframework.jdbc.core.JdbcTemplate;

abstract public class DatabaseAccess {

    private final Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_DAO_JDBC);
    protected final Properties jdbcProperties;

    protected DatabaseAccess(Properties jdbcProperties) {
        this.jdbcProperties = jdbcProperties;
    }

    public void initialize(String scadaBrVersion) {
        initializeImpl("");

        JdbcTemplate ejt = new JdbcTemplate();
        ejt.setDataSource(getDataSource());

        try {
            if (!checkDataBaseExists(ejt)) {
                LOG.info("Create new data base");
                createDataBase(ejt);
                if (jdbcProperties.getProperty("convert.db.type", "").isEmpty()) {
                    LOG.info("Populate new data base");
                    populateNewDatabase(ejt, scadaBrVersion);
                } else {
                    LOG.info("Convert existing data base");
                    convertDataBase(jdbcProperties);
                }
            }
            LOG.info("Wire Java Classes");
            wireJavaClassesWithDB(ejt);
            
            // else
            // // The database exists, so let's make its schema version matches
            // // the application version.
            // DBUpgrade.checkUpgrade();
        } catch (CannotGetJdbcConnectionException e) {
            LOG.log(Level.SEVERE, "Unable to connect to database of type "
                    + getType().name(), e);
            throw e;
        }

        postInitialize(ejt);
    }

    abstract public DatabaseType getType();

    abstract public void terminate();

    abstract public DataSource getDataSource();

    abstract public double applyBounds(double value);

    abstract public void executeCompress(JdbcTemplate ejt);

    abstract protected void initializeImpl(String propertyPrefix);

    protected void postInitialize(@SuppressWarnings("unused") JdbcTemplate ejt) {
        // no op - override as necessary
    }

    /**
     * Check if database exists
     * 
     * @param ejt
     * @return true if it exists, otherwise false
     */
    abstract protected boolean checkDataBaseExists(JdbcTemplate ejt);

    abstract protected void createDataBase(JdbcTemplate ejt);

    abstract public void runScript(String[] script, final OutputStream out)
            throws Exception;

    public <T> T doInConnection(ConnectionCallback<T> callback) {
        DataSource dataSource = getDataSource();
        Connection conn = null;
        try {
            conn = DataSourceUtils.getConnection(dataSource);
            conn.setAutoCommit(false);
            final T result = callback.doInConnection(conn);
            conn.commit();
            return result;
        } catch (SQLException | CannotGetJdbcConnectionException e) {
            try {
                if (conn != null) {
                    conn.rollback();
                }
            } catch (SQLException e1) {
                LOG.log(Level.WARNING, "Exception during rollback", e1);
            }

            // Wrap and rethrow
            throw new ShouldNeverHappenException(e);
        } finally {
            if (conn != null) {
                DataSourceUtils.releaseConnection(conn, dataSource);
            }
        }
    }

    /**
     * Convert an existing db to a different db engine or database
     *
     * @param jdbcProperties
     */
    private void convertDataBase(Properties jdbcProperties) {
        // Check if we should convert from another database.
        final String convertTypeStr = jdbcProperties.getProperty("convert.db.type", "");

        DatabaseType convertType = DatabaseType
                .valueOf(convertTypeStr.toUpperCase());
        if (convertType == null) {
            throw new IllegalArgumentException(
                    "Unknown convert database type: " + convertType);
        }

        DatabaseAccess sourceAccess = convertType.getImpl(jdbcProperties);
        sourceAccess.initializeImpl("convert.");

        DBConvert convert = new DBConvert();
        convert.setSource(sourceAccess);
        convert.setTarget(this);
        try {
            convert.execute();
        } catch (SQLException e) {
            throw new ShouldNeverHappenException(e);
        }

        sourceAccess.terminate();
    }

    /**
     * Populate an empty new database with basic objects
     *
     * @param ejt
     * @param scadaBrVersion
     */
    private void populateNewDatabase(JdbcTemplate ejt, String scadaBrVersion) {
        LOG.info("Setup user admin in db");

        // New database. Create a default user.
//TODO        UserDaoImpl.createAdmin(ejt);

        // Record the current version.
//TODO        SystemSettingsDaoImpl.setSetSchemaVersion(ejt, scadaBrVersion);
        LOG.info("database sucessfully created");
        BaseDao.initNodeAndEdgeTypes(ejt, scadaBrVersion);
    }

    private void wireJavaClassesWithDB(JdbcTemplate ejt) {
        BaseDao.wireJavaClassesWithDB(ejt);
    }

}
