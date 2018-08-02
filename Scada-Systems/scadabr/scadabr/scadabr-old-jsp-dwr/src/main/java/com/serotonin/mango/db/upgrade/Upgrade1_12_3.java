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

import java.io.OutputStream;
import java.util.HashMap;
import java.util.Map;

import com.serotonin.mango.db.DatabaseAccess;

/**
 * @author Matthew Lohbihler
 */
public class Upgrade1_12_3 extends DBUpgrade {

    @Override
    public void upgrade() throws Exception {
        OutputStream out = createUpdateLogOutputStream("1_12_3");

        // Run the script.
        Map<String, String[]> scripts = new HashMap<String, String[]>();
        scripts.put(DatabaseAccess.DatabaseType.DERBY.name(), derbyScript2);
        scripts.put(DatabaseAccess.DatabaseType.MYSQL.name(), mysqlScript2);
        scripts.put(DatabaseAccess.DatabaseType.MSSQL.name(), mssqlScript2);
        runScript(scripts, out);

        out.flush();
        out.close();
    }

    @Override
    protected String getNewSchemaVersion() {
        return "1.12.4";
    }

    private final String[] derbyScript2 = { //
        "alter table dataSources add column rtdata blob;", //
        "alter table publishers add column rtdata blob;", //
    };

    private final String[] mssqlScript2 = { //
        "alter table dataSources add column rtdata image;", //
        "alter table publishers add column rtdata image;", //
    };

    private final String[] mysqlScript2 = { //
        "alter table dataSources add column rtdata longblob;", //
        "alter table publishers add column rtdata longblob;", //
    };
}
