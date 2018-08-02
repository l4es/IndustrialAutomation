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
package com.serotonin.mango.db.upgrade;

import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.util.Map;

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.logger.LogUtils;
import com.serotonin.mango.Common;
import com.serotonin.mango.db.DatabaseAccess;
import com.serotonin.mango.db.dao.BaseDao;
import com.serotonin.mango.db.dao.SystemSettingsDao;
import br.org.scadabr.util.StringUtils;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.sql.DataSource;

/**
 * Base class for instances that perform database upgrades. The naming of
 * subclasses follows the convention 'Upgrade[maj]_[min]_[mic]', where
 * '[maj]_[min]_[mic]' is the version that the class upgrades from. The subclass
 * must be in this package.
 *
 * @author Matthew Lohbihler
 */
abstract public class DBUpgrade extends BaseDao {

    protected static final Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_DAO);
    protected static final String DEFAULT_DATABASE_TYPE = "*";

    public DBUpgrade() {
        super();
    }

    @Deprecated
    protected DBUpgrade(DataSource dataSource) {
        super(dataSource);
    }

    public static void checkUpgrade() {
        // If this is a very old version of the system, there may be multiple
        // upgrades to run, so start a loop.
        while (true) {
            // Get the current schema version.
            String schemaVersion = SystemSettingsDao
                    .getValue(SystemSettingsDao.DATABASE_SCHEMA_VERSION);

            // Convert the schema version to the class name convention. This
            // simply means replacing dots with
            // underscores and prefixing 'Upgrade' and this package.
            String upgradeClassname = DBUpgrade.class.getPackage().getName()
                    + ".Upgrade" + schemaVersion.replace('.', '_');

            // See if there is a class with this name.
            Class<?> clazz = null;
            DBUpgrade upgrade = null;
            try {
                clazz = Class.forName(upgradeClassname);
            } catch (ClassNotFoundException e) {
                // no op
            }

            if (clazz != null) {
                try {
                    upgrade = (DBUpgrade) clazz.newInstance();
                } catch (Throwable e) {
                    // Should never happen so wrap in a runtime and rethrow.
                    throw new ShouldNeverHappenException(e);
                }
            }

            if (upgrade == null) {
                LOG.log(Level.INFO, "Starting instance with version {0}", schemaVersion);
                break;
            }

            try {
                LOG.log(Level.WARNING, "Upgrading instance from {0} to {1}", new Object[]{schemaVersion, upgrade.getNewSchemaVersion()});
                upgrade.upgrade();
                SystemSettingsDao.getInstance().setValue(SystemSettingsDao.DATABASE_SCHEMA_VERSION, upgrade.getNewSchemaVersion());
            } catch (Exception e) {
                throw new ShouldNeverHappenException(e);
            }
        }
    }

    abstract protected void upgrade() throws Exception;

    abstract protected String getNewSchemaVersion();

    /**
     * Convenience method for subclasses
     *
     * @param script the array of script lines to run
     * @param out the stream to which to direct output from running the script
     * @throws Exception if something bad happens
     */
    protected void runScript(String[] script, OutputStream out)
            throws Exception {
        Common.ctx.getDatabaseAccess().runScript(script, out);
    }

    protected void runScript(Map<String, String[]> scripts,
            final OutputStream out) throws Exception {
        DatabaseAccess da = Common.ctx.getDatabaseAccess();
        String[] script = scripts.get(da.getType().name());
        if (script == null) {
            script = scripts.get(DEFAULT_DATABASE_TYPE);
        }
        runScript(script, out);
    }

    protected OutputStream createUpdateLogOutputStream(String version) {
        String dir = Common.getEnvironmentString("db.update.log.dir", "");
        dir = StringUtils.replaceMacros(dir, System.getProperties());

        File logDir = new File(dir);
        File logFile = new File(logDir, "Update" + version + ".log");
        LOG.log(Level.INFO, "Writing upgrade log to {0}", logFile.getAbsolutePath());

        try {
            if (logDir.isDirectory() && logDir.canWrite()) {
                return new FileOutputStream(logFile);
            }
        } catch (Throwable e) {
            LOG.log(Level.SEVERE, "Failed to create database upgrade log file.", e);
        }

        LOG.severe("Failing over to console for printing database upgrade messages");
        return System.out;
    }

    public static boolean isUpgradeNeeded(String schemaVersion) {
        String upgradeClassname = DBUpgrade.class.getPackage().getName()
                + ".Upgrade" + schemaVersion.replace('.', '_');

        Class<?> clazz = null;
        try {
            clazz = Class.forName(upgradeClassname);
            return true;
        } catch (ClassNotFoundException e) {
            return false;
        }
    }
}
