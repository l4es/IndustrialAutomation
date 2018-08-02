/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.jdbc;

import java.util.Properties;

/**
 *
 * @author aploese
 */
public enum DatabaseType {

    DERBY {
                @Override
                DatabaseAccess getImpl(Properties jdbcProperties) {
                    return new DerbyAccess(jdbcProperties);
                }
            },
    MSSQL {
                @Override
                DatabaseAccess getImpl(Properties jdbcProperties) {
                    return new MSSQLAccess(jdbcProperties);
                }
            },
    MYSQL {
                @Override
                DatabaseAccess getImpl(Properties jdbcProperties) {
                    return new MySQLAccess(jdbcProperties);
                }
            };

    abstract DatabaseAccess getImpl(Properties jdbcProperties);

}
