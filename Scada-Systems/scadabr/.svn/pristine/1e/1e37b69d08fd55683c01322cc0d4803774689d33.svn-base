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

import com.serotonin.mango.Common;
import java.io.OutputStream;
import javax.sql.DataSource;

/**
 * @author Matthew Lohbihler
 */
public class Upgrade1_6_1 extends DBUpgrade {

    private Upgrade1_6_1() {
        super();
    }
    
   @Deprecated
    private Upgrade1_6_1(DataSource dataSource) {
        super(dataSource);
    }

     public static Upgrade1_6_1 getInstance() {
        return new Upgrade1_6_1(Common.ctx.getDatabaseAccess().getDataSource());
    }
    
    @Override
    public void upgrade() throws Exception {
        try (OutputStream out = createUpdateLogOutputStream("1_6_1")) {
            LOG.info("Running script 1");
            runScript(script1, out);
            
            out.flush();
        }
    }

    @Override
    protected String getNewSchemaVersion() {
        return "1.6.2";
    }

    private static String[] script1 = {
        "alter table pointValues drop foreign key pointValuesFk1;",
        "alter table pointValues add constraint pointValuesFk1 foreign key (dataPointId) references dataPoints(id) on delete cascade;",
        "alter table mangoViews drop foreign key mangoViewsFk1;",
        "alter table mangoViews add constraint mangoViewsFk1 foreign key (userId) references users(id) on delete cascade;",
        "alter table mangoViewUsers drop foreign key mangoViewUsersFk1;",
        "alter table mangoViewUsers add constraint mangoViewUsersFk1 foreign key (mangoViewId) references mangoViews(id) on delete cascade;"};
}
