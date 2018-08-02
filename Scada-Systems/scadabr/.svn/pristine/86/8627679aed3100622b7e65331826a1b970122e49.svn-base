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
public class Upgrade1_9_0 extends DBUpgrade {

    private Upgrade1_9_0() {
        super();
    }
    
   @Deprecated
    private Upgrade1_9_0(DataSource dataSource) {
        super(dataSource);
    }

     public static Upgrade1_9_0 getInstance() {
        return new Upgrade1_9_0(Common.ctx.getDatabaseAccess().getDataSource());
    }
    
    @Override
    public void upgrade() throws Exception {
        // Run the script.
        try (OutputStream out = createUpdateLogOutputStream("1_9_0")) {
            // Run the script.
            LOG.info("Running script");
            Map<String, String[]> scripts = new HashMap<>();
            scripts.put(DatabaseAccess.DatabaseType.DERBY.name(), derbyScript);
            scripts.put(DatabaseAccess.DatabaseType.MYSQL.name(), mysqlScript);
            runScript(scripts, out);
            
            out.flush();
        }
    }

    @Override
    protected String getNewSchemaVersion() {
        return "1.10.0";
    }

    private static String[] derbyScript = {
        "create table reportInstanceDataAnnotationsBK (",
        "  pointValueId int not null,",
        "  reportInstancePointId int not null,",
        "  textPointValueShort varchar(128),",
        "  textPointValueLong clob,",
        "  sourceValue varchar(128)",
        ");",
        "insert into reportInstanceDataAnnotationsBK select pointValueId, reportInstancePointId, textPointValueShort, textPointValueLong, sourceValue from reportInstanceDataAnnotations;",
        "drop table reportInstanceDataAnnotations;",
        "create table reportInstanceDataAnnotations (",
        "  pointValueId bigint not null,",
        "  reportInstancePointId int not null,",
        "  textPointValueShort varchar(128),",
        "  textPointValueLong clob,",
        "  sourceValue varchar(128)",
        ");",
        "insert into reportInstanceDataAnnotations select pointValueId, reportInstancePointId, textPointValueShort, textPointValueLong, sourceValue from reportInstanceDataAnnotationsBK;",
        "drop table reportInstanceDataAnnotationsBK;",
        "",
        "create table reportInstanceDataBK (",
        "  pointValueId int not null,",
        "  reportInstancePointId int not null,",
        "  pointValue double,",
        "  ts bigint not null",
        ");",
        "insert into reportInstanceDataBK select pointValueId, reportInstancePointId, pointValue, ts from reportInstanceData;",
        "drop table reportInstanceData;",
        "create table reportInstanceData (",
        "  pointValueId bigint not null,",
        "  reportInstancePointId int not null,",
        "  pointValue double,",
        "  ts bigint not null",
        ");",
        "alter table reportInstanceData add constraint reportInstanceDataPk primary key (pointValueId, reportInstancePointId);",
        "alter table reportInstanceData add constraint reportInstanceDataFk1 foreign key (reportInstancePointId) ",
        "  references reportInstancePoints(id) on delete cascade;",
        "insert into reportInstanceData select pointValueId, reportInstancePointId, pointValue, ts from reportInstanceDataBK;",
        "drop table reportInstanceDataBK;",
        "alter table reportInstanceDataAnnotations add constraint reportInstanceDataAnnotationsPk",
        "  primary key (pointValueId, reportInstancePointId);",
        "alter table reportInstanceDataAnnotations add constraint reportInstanceDataAnnotationsFk1",
        "  foreign key (pointValueId, reportInstancePointId) references reportInstanceData(pointValueId, reportInstancePointId)",
        "  on delete cascade;",
        "create table pointValueAnnotationsBK (",
        "  pointValueId int not null,",
        "  textPointValueShort varchar(128),",
        "  textPointValueLong clob,",
        "  sourceType smallint,",
        "  sourceId int",
        ");",
        "insert into pointValueAnnotationsBK select pointValueId, textPointValueShort, textPointValueLong, sourceType, sourceId from pointValueAnnotations;",
        "drop table pointValueAnnotations;",
        "create table pointValueAnnotations (",
        "  pointValueId bigint not null,",
        "  textPointValueShort varchar(128),",
        "  textPointValueLong clob,",
        "  sourceType smallint,",
        "  sourceId int",
        ");",
        "insert into pointValueAnnotations select pointValueId, textPointValueShort, textPointValueLong, sourceType, sourceId from pointValueAnnotationsBK;",
        "drop table pointValueAnnotationsBK;",
        "create table pointValuesBK (",
        "  id int not null generated by default as identity (start with 1, increment by 1),",
        "  dataPointId int not null,",
        "  dataType int not null,",
        "  pointValue double,",
        "  ts bigint not null",
        ");",
        "insert into pointValuesBK select id, dataPointId, dataType, pointValue, ts from pointValues;",
        "drop table pointValues;",
        "create table pointValues (",
        "  id bigint not null generated by default as identity (start with 1, increment by 1),",
        "  dataPointId int not null,",
        "  dataType int not null,",
        "  pointValue double,",
        "  ts bigint not null",
        ");",
        "insert into pointValues select id, dataPointId, dataType, pointValue, ts from pointValuesBK;",
        "alter table pointValues add constraint pointValuesPk primary key (id);",
        "alter table pointValues add constraint pointValuesFk1 foreign key (dataPointId) references dataPoints(id) on delete cascade;",
        "create index pointValuesIdx1 on pointValues (ts, dataPointId);",
        "create index pointValuesIdx2 on pointValues (dataPointId, ts);", //
        "drop table pointValuesBK;",
        "alter table pointValueAnnotations add constraint pointValueAnnotationsFk1 foreign key (pointValueId)",
        "  references pointValues(id) on delete cascade;",//

        "alter table reportInstancePoints add column dataSourceName varchar(40);", //
        "update reportInstancePoints set dataSourceName='';", //
        "alter table reportInstancePoints alter dataSourceName not null;", //
        "alter table reportInstancePoints add column colour varchar(6);", //
    };

    private static String[] mysqlScript = {
        "alter table pointValueAnnotations drop foreign key pointValueAnnotationsFk1;",
        "alter table pointValueAnnotations modify pointValueId bigint not null;",
        "alter table pointValues modify id bigint not null auto_increment;",
        "alter table pointValueAnnotations add constraint pointValueAnnotationsFk1 foreign key (pointValueId) references pointValues(id) on delete cascade;",
        "alter table reportInstanceDataAnnotations drop foreign key reportInstanceDataAnnotationsFk1;",
        "alter table reportInstanceDataAnnotations modify pointValueId bigint not null;",
        "alter table reportInstanceData modify pointValueId bigint not null;",
        "alter table reportInstanceDataAnnotations add constraint reportInstanceDataAnnotationsFk1 foreign key (pointValueId, reportInstancePointId) references reportInstanceData(pointValueId, reportInstancePointId) on delete cascade;",//

        "alter table reportInstancePoints add column dataSourceName varchar(40);", //
        "update reportInstancePoints set dataSourceName='';", //
        "alter table reportInstancePoints modify dataSourceName varchar(40) not null;", //
        "alter table reportInstancePoints add column colour varchar(6);", //
    };
}
