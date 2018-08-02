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

/**
 * @author Matthew Lohbihler
 */
public class Upgrade1_11_1 extends DBUpgrade {

    @Override
    public void upgrade() throws Exception {
        OutputStream out = createUpdateLogOutputStream("1_11_1");

        // Run the script.
        runScript(script1, out);

        out.flush();
        out.close();
    }

    @Override
    protected String getNewSchemaVersion() {
        return "1.12.0";
    }

    private static String[] script1 = { //
        "alter table reportInstanceEvents add column alternateAckSource int;", //

        "alter table mangoViewUsers drop foreign key mangoViewUsersFk1;", //
        "alter table mangoViewUsers add constraint mangoViewUsersFk1 foreign key (mangoViewId) references mangoViews(id);", //

        "alter table watchListUsers drop foreign key watchListUsersFk1;", //
        "alter table watchListUsers add constraint watchListUsersFk1 foreign key (watchListId) references watchLists(id);", //
    };
}
