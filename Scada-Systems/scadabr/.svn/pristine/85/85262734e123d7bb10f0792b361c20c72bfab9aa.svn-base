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

import br.org.scadabr.utils.ImplementMeException;
import java.sql.SQLException;

import org.springframework.dao.DataAccessException;

import java.util.Properties;
import org.springframework.jdbc.core.JdbcTemplate;

public class MySQLAccess extends BasePooledAccess {

    public MySQLAccess(Properties jdbcProperties) {
        super(jdbcProperties);
        throw new ImplementMeException();
        //TODO fix dbUrl from getURL
    }

    @Override
    protected void initializeImpl(String propertyPrefix) {
        super.initializeImpl(propertyPrefix);
        dataSource.setInitialSize(3);
        dataSource.setMaxWait(-1);
        dataSource.setTestWhileIdle(true);
        dataSource.setTimeBetweenEvictionRunsMillis(10000);
        dataSource.setMinEvictableIdleTimeMillis(60000);
    }

    /*
    @Deprecated
    protected String getUrl(String propertyPrefix) {
        String url = super.getUrl(propertyPrefix);
        if (url.indexOf('?') > 0) {
            url += "&";
        } else {
            url += "?";
        }
        url += "useUnicode=yes&characterEncoding=" + Common.UTF8;
        return url;
    }
     */
    @Override
    public DatabaseType getType() {
        return DatabaseType.MYSQL;
    }

    @Override
    protected String getDriverClassName() {
        return "com.mysql.jdbc.Driver";
    }

    @Override
    protected boolean checkDataBaseExists(JdbcTemplate ejt) {
        try {
            ejt.execute("select count(*) from users");
            return true;
        } catch (DataAccessException e) {
            if (e.getCause() instanceof SQLException) {
                SQLException se = (SQLException) e.getCause();
                // This state means a missing table. Assume that the schema needs to be created.
                if ("42S02".equals(se.getSQLState())) {
                    return false;
                } else {
                    throw e;
                }
            }
            throw e;
        }
    }

    @Override
    protected void createDataBase(JdbcTemplate ejt) {
        createSchema("/db/createTables-mysql.sql");
    }

    @Override
    public double applyBounds(double value) {
        if (Double.isNaN(value)) {
            return 0;
        }
        if (value == Double.POSITIVE_INFINITY) {
            return Double.MAX_VALUE;
        }
        if (value == Double.NEGATIVE_INFINITY) {
            return -Double.MAX_VALUE;
        }

        return value;
    }

    @Override
    public void executeCompress(JdbcTemplate ejt) {
        // no op
    }
}
