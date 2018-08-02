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

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;

import javax.sql.DataSource;

import org.apache.commons.dbcp.BasicDataSource;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import br.org.scadabr.ShouldNeverHappenException;
import java.util.Properties;
import org.springframework.jdbc.core.JdbcTemplate;

/**
 * @author Matthew Lohbihler
 */
abstract public class BasePooledAccess extends DatabaseAccess {

    private final Log log = LogFactory.getLog(BasePooledAccess.class);
    protected BasicDataSource dataSource;

    public BasePooledAccess(Properties jdbcProperties) {
        super(jdbcProperties);
    }

    @Override
    protected void initializeImpl(String propertyPrefix) {
        log.info("Initializing pooled connection manager");
        dataSource = new BasicDataSource();
        dataSource.setDriverClassName(getDriverClassName());
        dataSource.setUrl(jdbcProperties.getProperty(propertyPrefix + "db.url"));
        dataSource.setUsername(jdbcProperties.getProperty(propertyPrefix + "db.username"));
        dataSource.setPassword(jdbcProperties.getProperty(propertyPrefix+ "db.getPassword"));
        dataSource.setMaxActive(Integer.parseInt(jdbcProperties.getProperty(propertyPrefix + "db.pool.maxActive", "10")));
        dataSource.setMaxIdle(Integer.parseInt(jdbcProperties.getProperty(propertyPrefix + "db.pool.maxIdle", "10")));
    }

    abstract protected String getDriverClassName();

    @Override
    public void runScript(String[] script, OutputStream out) {
        JdbcTemplate ejt = new JdbcTemplate();
        ejt.setDataSource(dataSource);

        StringBuilder statement = new StringBuilder();

        for (String line : script) {
            // Trim whitespace
            line = line.trim();

            // Skip comments
            if (line.startsWith("--")) {
                continue;
            }

            statement.append(line);
            statement.append(" ");
            if (line.endsWith(";")) {
                // Execute the statement
                ejt.execute(statement.toString());
                statement.delete(0, statement.length() - 1);
            }
        }
    }

    protected void createSchema(String scriptFile) {
        BufferedReader in = new BufferedReader(new InputStreamReader(BasePooledAccess.class.getResourceAsStream(scriptFile)));

        List<String> lines = new ArrayList<>();
        try {
            String line;
            while ((line = in.readLine()) != null) {
                lines.add(line);
            }

            String[] script = new String[lines.size()];
            lines.toArray(script);
            runScript(script, null);
        } catch (IOException ioe) {
            throw new ShouldNeverHappenException(ioe);
        } finally {
            try {
                in.close();
            } catch (IOException ioe) {
                log.warn("", ioe);
            }
        }
    }

    @Override
    public void terminate() {
        log.info("Stopping database");
        try {
            dataSource.close();
        } catch (SQLException e) {
            log.warn("", e);
        }
    }

    @Override
    public DataSource getDataSource() {
        return dataSource;
    }

}
