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
import com.serotonin.mango.db.DatabaseAccess;
import com.serotonin.mango.db.dao.WatchListDao;
import java.io.OutputStream;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import javax.sql.DataSource;

/**
 * @author Matthew Lohbihler
 */
public class Upgrade1_10_0 extends DBUpgrade {

    private Upgrade1_10_0() {
        super();
    }
    
   @Deprecated
    private Upgrade1_10_0(DataSource dataSource) {
        super(dataSource);
    }

     public static Upgrade1_10_0 getInstance() {
        return new Upgrade1_10_0(Common.ctx.getDatabaseAccess().getDataSource());
    }
    
    @Override
    public void upgrade() throws Exception {
        // Run the first script.
        try (OutputStream out = createUpdateLogOutputStream("1_10_0")) {
            // Run the first script.
            runScript(script1, out);
            
            xid();
            
            // Run the script.
            Map<String, String[]> scripts = new HashMap<>();
            scripts.put(DatabaseAccess.DatabaseType.DERBY.name(), derbyScript2);
            scripts.put(DatabaseAccess.DatabaseType.MYSQL.name(), mysqlScript2);
            runScript(scripts, out);
            
            out.flush();
        }
    }

    @Override
    protected String getNewSchemaVersion() {
        return "1.11.0";
    }

    private static String[] script1 = {
        "alter table watchLists add column xid varchar(50);",
        "alter table watchLists drop foreign key watchListsFk1;",
        "alter table watchLists add constraint watchListsFk1 foreign key (userId) references users(id) on delete cascade;",
        "alter table watchListPoints drop foreign key watchListPointsFk1;",
        "alter table watchListPoints add constraint watchListPointsFk1 foreign key (watchListId) references watchLists(id) on delete cascade;",
        "alter table dataSourceUsers drop foreign key dataSourceUsersFk2;",
        "alter table dataSourceUsers add constraint dataSourceUsersFk2 foreign key (userId) references users(id) on delete cascade;",
        "alter table dataPointUsers drop foreign key dataPointUsersFk2;",
        "alter table dataPointUsers add constraint dataPointUsersFk2 foreign key (userId) references users(id) on delete cascade;",
        "alter table reports drop foreign key reportsFk1;",
        "alter table reports add constraint reportsFk1 foreign key (userId) references users(id) on delete cascade;",
        "alter table reportInstances drop foreign key reportInstancesFk1;",
        "alter table reportInstances add constraint reportInstancesFk1 foreign key (userId) references users(id) on delete cascade;",
        "alter table mangoViewUsers drop foreign key mangoViewUsersFk2;",
        "alter table mangoViewUsers add constraint mangoViewUsersFk2 foreign key (userId) references users(id) on delete cascade;",
        "alter table events add column alternateAckSource int;", //
    };

    private static String[] derbyScript2 = {
        "alter table watchLists alter xid not null;",
        "alter table watchLists add constraint watchListsUn1 unique (xid);",
        "create table userCommentsBK (", //
        "  userId int not null,", //
        "  commentType int not null,", //
        "  typeKey int not null,", //
        "  ts bigint not null,", //
        "  commentText varchar(1024) not null", //
        ");", //
        "insert into userCommentsBK select userId, commentType, typeKey, ts, commentText from userComments;", //
        "drop table userComments;", //
        "create table userComments (", //
        "  userId int,", //
        "  commentType int not null,", //
        "  typeKey int not null,", //
        "  ts bigint not null,", //
        "  commentText varchar(1024) not null", //
        ");", //
        "insert into userComments select userId, commentType, typeKey, ts, commentText from userCommentsBK;", //
        "alter table userComments add constraint userCommentsFk1 foreign key (userId) references users(id);", //
        "drop table userCommentsBK;", //

        "create table reportInstanceUserCommentsBK (", //
        "  reportInstanceId int not null,", //
        "  username varchar(40) not null,", //
        "  commentType int not null,", //
        "  typeKey int not null,", //
        "  ts bigint not null,", //
        "  commentText varchar(1024) not null", //
        ");", //
        "insert into reportInstanceUserCommentsBK select reportInstanceId, username, commentType, typeKey, ts, commentText from reportInstanceUserComments;", //
        "drop table reportInstanceUserComments;", //
        "create table reportInstanceUserComments (", //
        "  reportInstanceId int not null,", //
        "  username varchar(40),", //
        "  commentType int not null,", //
        "  typeKey int not null,", //
        "  ts bigint not null,", //
        "  commentText varchar(1024) not null", //
        ");", //
        "insert into reportInstanceUserComments select reportInstanceId, username, commentType, typeKey, ts, commentText from reportInstanceUserCommentsBK;", //
        "alter table reportInstanceUserComments add constraint reportInstanceUserCommentsFk1 foreign key (reportInstanceId) references reportInstances(id) on delete cascade;", //
        "drop table reportInstanceUserCommentsBK;", //

        "create table maintenanceEvents (", //
        "  id int not null generated by default as identity (start with 1, increment by 1),", //
        "  xid varchar(50) not null,", //
        "  dataSourceId int not null,", //
        "  alias varchar(255),", //
        "  alarmLevel int not null,", //
        "  scheduleType int not null,", //
        "  disabled char(1) not null,", //
        "  activeYear int,", //
        "  activeMonth int,", //
        "  activeDay int,", //
        "  activeHour int,", //
        "  activeMinute int,", //
        "  activeSecond int,", //
        "  activeCron varchar(25),", //
        "  inactiveYear int,", //
        "  inactiveMonth int,", //
        "  inactiveDay int,", //
        "  inactiveHour int,", //
        "  inactiveMinute int,", //
        "  inactiveSecond int,", //
        "  inactiveCron varchar(25)", //
        ");", //
        "alter table maintenanceEvents add constraint maintenanceEventsPk primary key (id);", //
        "alter table maintenanceEvents add constraint maintenanceEventsUn1 unique (xid);", //
        "alter table maintenanceEvents add constraint maintenanceEventsFk1 foreign key (dataSourceId) references dataSources(id);", //
    };

    private static String[] mysqlScript2 = {"alter table watchLists modify xid varchar(50) not null;",
        "alter table watchLists add constraint watchListsUn1 unique (xid);",
        "alter table userComments modify userId int;",
        "alter table reportInstanceUserComments modify username varchar(40);",
        "create table maintenanceEvents (", //
        "  id int not null auto_increment,", //
        "  xid varchar(50) not null,", //
        "  dataSourceId int not null,", //
        "  alias varchar(255),", //
        "  alarmLevel int not null,", //
        "  scheduleType int not null,", //
        "  disabled char(1) not null,", //
        "  activeYear int,", //
        "  activeMonth int,", //
        "  activeDay int,", //
        "  activeHour int,", //
        "  activeMinute int,", //
        "  activeSecond int,", //
        "  activeCron varchar(25),", //
        "  inactiveYear int,", //
        "  inactiveMonth int,", //
        "  inactiveDay int,", //
        "  inactiveHour int,", //
        "  inactiveMinute int,", //
        "  inactiveSecond int,", //
        "  inactiveCron varchar(25),", //
        "  primary key (id)", //
        ") engine=InnoDB;", //
        "alter table maintenanceEvents add constraint maintenanceEventsUn1 unique (xid);", //
        "alter table maintenanceEvents add constraint maintenanceEventsFk1 foreign key (dataSourceId) references dataSources(id);", //
};

    private void xid() {
        // Default the xid values.
        WatchListDao watchListDao = WatchListDao.getInstance();
        List<Integer> ids = ejt.queryForList("select id from watchLists", Integer.class);
        for (Integer id : ids) {
            ejt.update("update watchLists set xid=? where id=?", new Object[]{watchListDao.generateUniqueXid(), id});
        }
    }
}
