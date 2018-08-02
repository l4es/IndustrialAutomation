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
import java.util.HashMap;
import java.util.Map;

import com.serotonin.mango.db.DatabaseAccess;
import javax.sql.DataSource;

/**
 * @author Matthew Lohbihler
 */
public class Upgrade1_7_0 extends DBUpgrade {


    private Upgrade1_7_0() {
        super();
    }
    
   @Deprecated
    private Upgrade1_7_0(DataSource dataSource) {
        super(dataSource);
    }

     public static Upgrade1_7_0 getInstance() {
        return new Upgrade1_7_0(Common.ctx.getDatabaseAccess().getDataSource());
    }
    
    @Override
    public void upgrade() throws Exception {
        try (OutputStream out = createUpdateLogOutputStream("1_7_0")) {
            LOG.info("Running script 1");
            runScript(script1, out);
            
            LOG.info("Running script 2");
            Map<String, String[]> script2 = new HashMap<String, String[]>();
            script2.put(DatabaseAccess.DatabaseType.DERBY.name(), derbyScript2);
            script2.put(DatabaseAccess.DatabaseType.MYSQL.name(), mysqlScript2);
            runScript(script2, out);
            
            out.flush();
        }
    }

    @Override
    protected String getNewSchemaVersion() {
        return "1.8.0";
    }

    private static String[] script1 = { //
        "alter table events add column ackTs bigint;",//
        "alter table events add column ackUserId int;",//
        "alter table events add constraint eventsFk1 foreign key (ackUserId) references users(id);",//

        "update events e, (",//
        "  select ue1.eventId, ue1.userId, ue1.ackTs", //
        "  from userEvents ue1 join (",//
        "    select eventId, max(ackTs) as ackTs ",//
        "    from userEvents ",//
        "    where ackTs is not null ",//
        "    group by eventId",//
        "  ) ue2 on ue1.eventId=ue2.eventId and ue1.ackTs=ue2.ackTs", ") u ",//
        "set e.ackTs=u.ackTs, e.ackUserId=u.userId ", //
        "where e.id=u.eventId;",//

        "update userEvents ue, events e set ue.silenced='Y' ",//
        "where e.id=ue.eventId ",//
        "  and e.ackTs is not null;",//

        "alter table userEvents drop column ackTs;",//

        "alter table reportInstanceEvents add column ackTs bigint;",//
        "alter table reportInstanceEvents add column ackUsername varchar(40);",//

        "alter table reportInstances add includeUserComments char(1);",//
        "update reportInstances set includeUserComments='N';",//
    };

    private static String[] derbyScript2 = { //
        "create table reportInstanceUserComments (",//
        "  reportInstanceId int not null,", //
        "  username varchar(40) not null,",//
        "  commentType int not null,", //
        "  typeKey int not null,",//
        "  ts bigint not null,", //
        "  commentText varchar(1024) not null",//
        ");",//
        "alter table reportInstanceUserComments add constraint reportInstanceUserCommentsFk1",//
        "  foreign key (reportInstanceId) references reportInstances(id) on delete cascade;",//

        "alter table reportInstances alter includeUserComments not null;",//
        "alter table dataSources alter xid varchar(50) not null;",//
        "alter table dataPoints alter xid varchar(50) not null;",//
        "alter table mangoViews alter xid varchar(50) not null;",//
        "alter table pointEventDetectors alter xid varchar(50) not null;",//
        "alter table pointLinks alter xid varchar(50) not null;", //
    };

    private static String[] mysqlScript2 = {//
        "create table reportInstanceUserComments (",//
        "  reportInstanceId int not null,",//
        "  username varchar(40) not null,",//
        "  commentType int not null,", //
        "  typeKey int not null,",//
        "  ts bigint not null,", //
        "  commentText varchar(1024) not null",//
        ") engine=InnoDB;",//
        "alter table reportInstanceUserComments add constraint reportInstanceUserCommentsFk1",//
        "  foreign key (reportInstanceId) references reportInstances(id) on delete cascade;",//

        "alter table reportInstances modify includeUserComments char(1) not null;",
        "alter table dataSources modify xid varchar(50) not null;",
        "alter table dataPoints modify xid varchar(50) not null;",
        "alter table mangoViews modify xid varchar(50) not null;",
        "alter table pointEventDetectors modify xid varchar(50) not null;",
        "alter table pointLinks modify xid varchar(50) not null;",};
}
