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

import br.org.scadabr.ShouldNeverHappenException;
import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.Reader;
import java.io.UnsupportedEncodingException;
import java.sql.CallableStatement;
import java.sql.Connection;
import java.sql.SQLException;
import java.util.Collections;
import java.util.Properties;
import javax.sql.DataSource;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.derby.jdbc.EmbeddedXADataSource;
import org.apache.derby.tools.ij;
import org.springframework.jdbc.CannotGetJdbcConnectionException;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.jdbc.datasource.DataSourceUtils;

public class DerbyAccess extends DatabaseAccess {

    private final Log log = LogFactory.getLog(DerbyAccess.class);

    private static final double LARGEST_POSITIVE = 1.79769E+308;
    private static final double SMALLEST_POSITIVE = 2.225E-307;
    private static final double LARGEST_NEGATIVE = -2.225E-307;
    private static final double SMALLEST_NEGATIVE = -1.79769E+308;

    private EmbeddedXADataSource dataSource;

    public DerbyAccess(Properties jdbcProperties) {
        super(jdbcProperties);
    }

    @Override
    public DatabaseType getType() {
        return DatabaseType.DERBY;
    }

    @Override
    protected void initializeImpl(String propertyPrefix) {
        log.info("Initializing derby connection manager");
        dataSource = new EmbeddedXADataSource();
        dataSource.setCreateDatabase("create");

        dataSource.setDatabaseName(jdbcProperties.getProperty(propertyPrefix + "db.url"));
        dataSource.setDataSourceName("scadaBrDataSource");

        // Creation of a connection will optionally create the database.
        Connection c = DataSourceUtils.getConnection(dataSource);
        DataSourceUtils.releaseConnection(c, dataSource);
    }

    @Override
    public void terminate() {
        log.info("Stopping database");
        dataSource.setDatabaseName("");
        dataSource.setShutdownDatabase("shutdown");
        Connection conn = null;
        try {
            conn = DataSourceUtils.getConnection(dataSource);
        } catch (CannotGetJdbcConnectionException e) {
            final SQLException se = (SQLException) e.getCause();
            if ("XJ015".equals(se.getSQLState())) {
                log.debug("Stopped database");
                // A SQL code indicating that the system was successfully shut down. We can ignore this.
            } else {
                throw e;
            }
        }
        DataSourceUtils.releaseConnection(conn, dataSource);
    }

    @Override
    public DataSource getDataSource() {
        return dataSource;
    }

    @Override
    protected boolean checkDataBaseExists(JdbcTemplate ejt) {
        return 1 == ejt.queryForObject("select count(1) from sys.systables where tablename='USERS'", Integer.class);
    }

    @Override
    protected void createDataBase(JdbcTemplate ejt) {
        // The users table wasn't found, so assume that this is a new Mango instance.
        // Create the tables
        try {
            try (FileOutputStream out = new FileOutputStream("createTables.log")) {
                Connection conn = DataSourceUtils.getConnection(dataSource);
                org.apache.derby.tools.ij.runScript(conn,
                        DerbyAccess.class.getResourceAsStream("/db/createTables-derby.sql"), "ASCII", out, "UTF-8");
                DataSourceUtils.releaseConnection(conn, dataSource);
                out.flush();
            }
        } catch (IOException | CannotGetJdbcConnectionException e) {
            StringBuilder sb = new StringBuilder();
            try (Reader fr = new FileReader("createTables.log")) {
                try (BufferedReader br = new BufferedReader(fr)) {
                    br.lines().forEach((String line) -> sb.append(line));
                }
                log.fatal("Create DB Errior: \n" + sb.toString());
            } catch (IOException ioe) {

            }
            log.fatal(e);
            // Should never happen, so just wrap in a runtime exception and rethrow.
            throw new ShouldNeverHappenException(e);
        }

    }

    @Override
    public void runScript(String[] script, final OutputStream out) throws Exception {
        StringBuilder sb = new StringBuilder();
        for (String line : script) {
            sb.append(line).append("\r\n");
        }
        final InputStream in = new ByteArrayInputStream(sb.toString().getBytes("ASCII"));

        doInConnection((Connection conn) -> {
            try {
                ij.runScript(conn, in, "ASCII", out, "UTF-8");
                return true;
            } catch (UnsupportedEncodingException e) {
                throw new ShouldNeverHappenException(e);
            }
        });
    }

    @Override
    public double applyBounds(double value) {
        if (value > 0) {
            if (value < SMALLEST_POSITIVE) {
                value = SMALLEST_POSITIVE;
            } else if (value > LARGEST_POSITIVE) {
                value = LARGEST_POSITIVE;
            }
        } else if (value < 0) {
            if (value < SMALLEST_NEGATIVE) {
                value = SMALLEST_NEGATIVE;
            } else if (value > LARGEST_NEGATIVE) {
                value = LARGEST_NEGATIVE;
            }
        } else if (Double.isNaN(value)) {
            value = 0;
        }

        return value;
    }

    @Override
    public void executeCompress(JdbcTemplate ejt) {
        compressTable(ejt, "pointValues");
        compressTable(ejt, "pointValueAnnotations");
        compressTable(ejt, "events");
        compressTable(ejt, "reportInstanceData");
        compressTable(ejt, "reportInstanceDataAnnotations");
        compressTable(ejt, "reportInstanceEvents");
        compressTable(ejt, "reportInstanceUserComments");
    }

    private void compressTable(JdbcTemplate ejt, final String tableName) {
        ejt.call((Connection conn) -> {
            CallableStatement cs = conn.prepareCall("call SYSCS_UTIL.SYSCS_COMPRESS_TABLE(?, ?, ?)");
            cs.setString(1, "APP");
            cs.setString(2, tableName.toUpperCase());
            cs.setShort(3, (short) 0);
            return cs;
        }, Collections.EMPTY_LIST);
    }
}
