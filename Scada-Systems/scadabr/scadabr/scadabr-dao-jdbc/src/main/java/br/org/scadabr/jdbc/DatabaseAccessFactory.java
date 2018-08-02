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

import br.org.scadabr.ScadaBrVersionBean;
import br.org.scadabr.logger.LogUtils;
import java.util.Properties;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.annotation.PostConstruct;
import javax.annotation.PreDestroy;
import javax.inject.Inject;
import javax.inject.Named;
import javax.sql.DataSource;
import org.springframework.jdbc.core.JdbcTemplate;

@Named
public class DatabaseAccessFactory {
    
    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_DAO_JDBC);

    private DatabaseAccess databaseAccess;

    private String jdbcPropertiesName = "/jdbc.properties";

    @Inject
    private ScadaBrVersionBean scadaBrVersionBean;


    public DatabaseAccessFactory() {

    }

    public DatabaseAccess createDatabaseAccess(Properties jdbcProperties) {
        String type = jdbcProperties.getProperty("db.type", "derby");
        DatabaseType dt = DatabaseType.valueOf(type.toUpperCase());

        if (dt == null) {
            throw new IllegalArgumentException("Unknown database type: " + type);
        }

        return dt.getImpl(jdbcProperties);
    }


    @PostConstruct
    public void startDB() {
        LOG.info("Start DB called");
        if (databaseAccess == null) {
            try {
                final Properties jdbcPropertiers = new Properties();
                jdbcPropertiers.load(DatabaseAccess.class.getResourceAsStream(jdbcPropertiesName));
                databaseAccess = createDatabaseAccess(jdbcPropertiers);
                databaseAccess.initialize(scadaBrVersionBean.getVersionNumber());
                LOG.info("DB Running");
            } catch (Exception e) {
                LOG.log(Level.SEVERE, "Error during DB startup!", e);
                throw new RuntimeException(e);
            }
        } else {
            LOG.severe("DB already started, do not restart");
        }
    }

    @PreDestroy
    public void stopDB() {
        LOG.info("Stop DB called");
        if (databaseAccess != null) {
            databaseAccess.terminate();
            databaseAccess = null;
            LOG.info("DB Stopped");
        } else {
            LOG.severe("DB already stopped, do not stop");
        }
    }

    /**
     * The name of the resource
     * @return the jdbcPropertiesName
     */
    public String getJdbcPropertiesName() {
        return jdbcPropertiesName;
    }

    /**
     * @param jdbcPropertiesName the jdbcPropertiesName to set
     */
    public void setJdbcPropertiesName(String jdbcPropertiesName) {
        this.jdbcPropertiesName = jdbcPropertiesName;
    }

    public DatabaseType getDatabaseType() {
        return databaseAccess.getType();
    }

    public DataSource getDataSource() {
        return databaseAccess.getDataSource();
    }

    public void executeCompress(JdbcTemplate ejt) {
        databaseAccess.executeCompress(ejt);
    }

}
