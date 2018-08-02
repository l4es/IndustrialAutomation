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

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.serotonin.mango.db.dao.DataPointDao;
import com.serotonin.mango.db.dao.PointValueDao;
import com.serotonin.mango.vo.DataPointVO;

/**
 * @author Matthew Lohbihler
 */
public class Upgrade0_9_2 extends DBUpgrade {

    private final Log log = LogFactory.getLog(getClass());

    @Override
    public void upgrade() throws Exception {
        OutputStream out = createUpdateLogOutputStream("0_9_2");

        // Run the script.
        log.info("Running script");
        runScript(script, out);

        out.flush();
        out.close();

        // Clean up mismatched point values.
        PointValueDao pointValueDao = PointValueDao.getInstance();
        for (DataPointVO dp : DataPointDao.getInstance().getDataPoints(null, false)) {
            pointValueDao.deletePointValuesWithMismatchedType(dp.getId(), dp.getDataType());
        }
    }

    @Override
    protected String getNewSchemaVersion() {
        return "0.10.0";
    }

    private static String[] script = {
        "alter table mangoViews alter name set data type varchar(100);",
        "alter table events alter message set data type varchar(1024);",
        "create table reports (",
        "  id int not null generated by default as identity (start with 1, increment by 1),",
        "  userId int not null,",
        "  name varchar(100) not null,",
        "  data blob not null",
        ");",
        "alter table reports add constraint reportsPk primary key (id);",
        "alter table reports add constraint reportsFk1 foreign key (userId) references users(id);",
        "create table reportInstances (",
        "  id int not null generated by default as identity (start with 1, increment by 1),",
        "  userId int not null,",
        "  name varchar(100) not null,",
        "  reportStartTime bigint not null,",
        "  reportEndTime bigint not null,",
        "  runStartTime bigint,",
        "  runEndTime bigint,",
        "  recordCount int,",
        "  preventPurge char(1)",
        ");",
        "alter table reportInstances add constraint reportInstancesPk primary key (id);",
        "alter table reportInstances add constraint reportInstancesFk1 foreign key (userId) references users(id);",
        "create table reportInstancePoints (",
        "  id int not null generated by default as identity (start with 1, increment by 1),",
        "  reportInstanceId int not null,",
        "  pointName varchar(100) not null,",
        "  dataType int not null,",
        "  startValue varchar(4096),",
        "  textRenderer blob",
        ");",
        "alter table reportInstancePoints add constraint reportInstancePointsPk primary key (id);",
        "alter table reportInstancePoints add constraint reportInstancePointsFk1 foreign key (reportInstanceId) references reportInstances(id) on delete cascade;",
        "create table reportInstanceData (",
        "  pointValueId int not null,",
        "  reportInstancePointId int not null,",
        "  pointValue double,",
        "  ts bigint not null",
        ");",
        "alter table reportInstanceData add constraint reportInstanceDataPk primary key (pointValueId, reportInstancePointId);",
        "alter table reportInstanceData add constraint reportInstanceDataFk1 foreign key (reportInstancePointId) references reportInstancePoints(id) on delete cascade;",
        "create table reportInstanceDataAnnotations (",
        "  pointValueId int not null,",
        "  reportInstancePointId int not null,",
        "  textPointValue varchar(4096),",
        "  sourceValue varchar(128)",
        ");",
        "alter table reportInstanceDataAnnotations add constraint reportInstanceDataAnnotationsPk primary key (pointValueId, reportInstancePointId);",
        "alter table reportInstanceDataAnnotations add constraint reportInstanceDataAnnotationsFk1 foreign key (pointValueId, reportInstancePointId) references reportInstanceData(pointValueId, reportInstancePointId) on delete cascade;",
        "alter table emailHandlerRecipients drop constraint emailHandlerRecipientsFk1;",
        "delete from emailHandlerRecipients where eventHandlerId not in (select id from eventHandlers);",
        "alter table emailHandlerRecipients add constraint emailHandlerRecipientsFk1 foreign key (eventHandlerId)",
        "  references eventHandlers(id) on delete cascade;",
        "create table mailingListInactive ( ",
        "  mailingListId int not null, ",
        "  inactiveInterval int not null ",
        "); ",
        "alter table mailingListInactive add constraint mailingListInactiveFk1 foreign key (mailingListId) references mailingLists(id) on delete cascade;",
        "alter table mailingListMembers drop constraint mailingListMembersFk1;",
        "alter table mailingListMembers add constraint mailingListMembersFk1 foreign key (mailingListId) references mailingLists(id) on delete cascade;",
        "create table publishers ( ",
        "  id int not null generated by default as identity (start with 1, increment by 1), ",
        "  data blob not null ", "); ", "alter table publishers add constraint publishersPk primary key (id); ",};
}