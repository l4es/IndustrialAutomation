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
import java.util.List;
import java.util.Map;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.serotonin.mango.db.DatabaseAccess;
import com.serotonin.mango.db.dao.CompoundEventDetectorDao;
import com.serotonin.mango.db.dao.EventDao;
import com.serotonin.mango.db.dao.MailingListDao;
import com.serotonin.mango.db.dao.PublisherDao;
import com.serotonin.mango.db.dao.ScheduledEventDao;

/**
 * @author Matthew Lohbihler
 */
public class Upgrade1_8_3 extends DBUpgrade {

    private final Log log = LogFactory.getLog(getClass());

    @Override
    public void upgrade() throws Exception {
        OutputStream out = createUpdateLogOutputStream("1_8_3");

        // Run the script.
        log.info("Running script 1");
        runScript(script1, out);

        xid();

        // Run the script.
        log.info("Running script 2");
        Map<String, String[]> scripts = new HashMap<String, String[]>();
        scripts.put(DatabaseAccess.DatabaseType.DERBY.name(), derbyScript2);
        scripts.put(DatabaseAccess.DatabaseType.MYSQL.name(), mysqlScript2);
        runScript(scripts, out);

        // Run the MySQL fix script
        log.info("Running MySQL fix script");
        scripts.clear();
        scripts.put(DatabaseAccess.DatabaseType.DERBY.name(), new String[0]);
        scripts.put(DatabaseAccess.DatabaseType.MYSQL.name(), mysqlScript3);
        try {
            runScript(scripts, out);
        } catch (Exception e) {
            // Ignore. The FKs likely already existed.
        }

        out.flush();
        out.close();
    }

    @Override
    protected String getNewSchemaVersion() {
        return "1.9.0";
    }

    private static String[] script1 = {"alter table scheduledEvents add column xid varchar(50);",
        "alter table compoundEventDetectors add column xid varchar(50);",
        "alter table mailingLists add column xid varchar(50);",
        "alter table publishers add column xid varchar(50);",
        "alter table eventHandlers add column xid varchar(50);",};

    private static String[] derbyScript2 = {"alter table scheduledEvents alter xid not null;",
        "alter table scheduledEvents add constraint scheduledEventsUn1 unique (xid);",
        "alter table compoundEventDetectors alter xid not null;",
        "alter table compoundEventDetectors add constraint compoundEventDetectorsUn1 unique (xid);",
        "alter table mailingLists alter xid not null;",
        "alter table mailingLists add constraint mailingListsUn1 unique (xid);",
        "alter table publishers alter xid not null;",
        "alter table publishers add constraint publishersUn1 unique (xid);",
        "alter table eventHandlers alter xid not null;",
        "alter table eventHandlers add constraint eventHandlersUn1 unique (xid);",};

    private static String[] mysqlScript2 = {"alter table scheduledEvents modify xid varchar(50) not null;",
        "alter table scheduledEvents add constraint scheduledEventsUn1 unique (xid);",
        "alter table compoundEventDetectors modify xid varchar(50) not null;",
        "alter table compoundEventDetectors add constraint compoundEventDetectorsUn1 unique (xid);",
        "alter table mailingLists modify xid varchar(50) not null;",
        "alter table mailingLists add constraint mailingListsUn1 unique (xid);",
        "alter table publishers modify xid varchar(50) not null;",
        "alter table publishers add constraint publishersUn1 unique (xid);",
        "alter table eventHandlers modify xid varchar(50) not null;",
        "alter table eventHandlers add constraint eventHandlersUn1 unique (xid);",};

    private static String[] mysqlScript3 = {
        "alter table userEvents engine=InnoDB;",
        "delete from userEvents where eventId not in (select id from events);",
        "delete from userEvents where userId not in (select id from users);",
        "alter table userEvents add constraint userEventsFk1 foreign key (eventId) references events(id) on delete cascade;",
        "alter table userEvents add constraint userEventsFk2 foreign key (userId) references users(id) on delete cascade;",};

    private void xid() {
        // Default the xid values.
        ScheduledEventDao scheduledEventDao = ScheduledEventDao.getInstance();
        List<Integer> ids = ejt.queryForList("select id from scheduledEvents", Integer.class);
        for (Integer id : ids) {
            ejt.update("update scheduledEvents set xid=? where id=?", new Object[]{
                scheduledEventDao.generateUniqueXid(), id});
        }

        CompoundEventDetectorDao compoundEventDetectorDao = CompoundEventDetectorDao.getInstance();
        ids = ejt.queryForList("select id from compoundEventDetectors", Integer.class);
        for (Integer id : ids) {
            ejt.update("update compoundEventDetectors set xid=? where id=?", new Object[]{
                compoundEventDetectorDao.generateUniqueXid(), id});
        }

        MailingListDao mailingListDao = MailingListDao.getInstance();
        ids = ejt.queryForList("select id from mailingLists", Integer.class);
        for (Integer id : ids) {
            ejt.update("update mailingLists set xid=? where id=?", new Object[]{mailingListDao.generateUniqueXid(),
                id});
        }

        PublisherDao publisherDao = PublisherDao.getInstance();
        ids = ejt.queryForList("select id from publishers", Integer.class);
        for (Integer id : ids) {
            ejt.update("update publishers set xid=? where id=?", new Object[]{publisherDao.generateUniqueXid(), id});
        }

        EventDao eventDao = EventDao.getInstance();
        ids = ejt.queryForList("select id from eventHandlers", Integer.class);
        for (Integer id : ids) {
            ejt.update("update eventHandlers set xid=? where id=?", new Object[]{eventDao.generateUniqueXid(), id});
        }
    }
}
