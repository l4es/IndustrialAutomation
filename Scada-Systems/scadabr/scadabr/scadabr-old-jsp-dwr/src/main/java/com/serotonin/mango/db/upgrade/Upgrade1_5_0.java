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
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.List;

import org.springframework.jdbc.core.RowCallbackHandler;

import com.serotonin.mango.Common;
import com.serotonin.mango.db.dao.DataPointDao;
import com.serotonin.mango.db.dao.DataSourceDao;
import com.serotonin.mango.db.dao.ViewDao;
import com.serotonin.mango.view.View;
import com.serotonin.mango.view.component.AnalogGraphicComponent;
import com.serotonin.mango.view.component.BinaryGraphicComponent;
import com.serotonin.mango.view.component.DynamicGraphicComponent;
import com.serotonin.mango.view.component.HtmlComponent;
import com.serotonin.mango.view.component.MultistateGraphicComponent;
import com.serotonin.mango.view.component.PointComponent;
import com.serotonin.mango.view.component.ScriptComponent;
import com.serotonin.mango.view.component.SimpleImageComponent;
import com.serotonin.mango.view.component.SimplePointComponent;
import com.serotonin.mango.view.component.ThumbnailComponent;
import com.serotonin.mango.view.graphic.AnalogImageSetRenderer;
import com.serotonin.mango.view.graphic.BasicImageRenderer;
import com.serotonin.mango.view.graphic.BasicRenderer;
import com.serotonin.mango.view.graphic.BinaryImageSetRenderer;
import com.serotonin.mango.view.graphic.DynamicImageRenderer;
import com.serotonin.mango.view.graphic.GraphicRenderer;
import com.serotonin.mango.view.graphic.MultistateImageSetRenderer;
import com.serotonin.mango.view.graphic.ScriptRenderer;
import com.serotonin.mango.view.graphic.ThumbnailRenderer;
import com.serotonin.mango.vo.event.PointEventDetectorVO;
import br.org.scadabr.util.SerializationHelper;
import javax.sql.DataSource;

/**
 * @author Matthew Lohbihler
 */
@SuppressWarnings("deprecation")
public class Upgrade1_5_0 extends DBUpgrade {

    private Upgrade1_5_0() {
        super();
    }
    
   @Deprecated
    private Upgrade1_5_0(DataSource dataSource) {
        super(dataSource);
    }

     public static Upgrade1_5_0 getInstance() {
        return new Upgrade1_5_0(Common.ctx.getDatabaseAccess().getDataSource());
    }
    
    @Override
    public void upgrade() throws Exception {
        // Run the script.
        try (OutputStream out = createUpdateLogOutputStream("1_5_0")) {
            // Run the script.
            LOG.info("Running script 1");
            runScript(script1, out);
            
            xid();
            viewData();
            eventData();
            
            // Run the script.
            LOG.info("Running script 2");
            runScript(script2, out);
            
            out.flush();
        }
    }

    @Override
    protected String getNewSchemaVersion() {
        return "1.6.0";
    }

    private static String[] script1 = {
        "alter table pointEventDetectors add alias varchar(255);",
        "alter table eventHandlers add alias varchar(255);",
        "alter table dataSources add column xid varchar(20);",
        "alter table dataPoints add column xid varchar(20);",
        "alter table pointEventDetectors add column xid varchar(20);",
        "alter table pointEventDetectors add stateLimit double;",
        "update pointEventDetectors set stateLimit=limit;",
        "alter table pointEventDetectors drop limit;",
        "alter table compoundEventDetectors add conditionText varchar(256);",
        "update compoundEventDetectors set conditionText=condition;",
        "alter table compoundEventDetectors drop condition;",
        "alter table compoundEventDetectors alter conditionText not null;",
        "alter table mangoViewUsers add constraint mangoViewUsersFk1 foreign key (mangoViewId) references mangoViews(id) on delete cascade;",
        "alter table mangoViewUsers add constraint mangoViewUsersFk2 foreign key (userId) references users(id) on delete cascade;",
        "alter table mangoViews add column data blob;",
        "alter table events add rtnTs bigint;",
        "alter table events add rtnCause int;",
        "create table userEvents (",
        "  eventId int not null,",
        "  userId int not null,",
        "  silenced char(1) not null,",
        "  ackTs bigint",
        ");",
        "alter table userEvents add constraint userEventsPk primary key (eventId, userId);",
        "alter table userEvents add constraint userEventsFk1 foreign key (eventId) references events(id) on delete cascade;",
        "alter table userEvents add constraint userEventsFk2 foreign key (userId) references users(id) on delete cascade;",
        "create table reportInstanceEvents (",
        "  eventId int not null,",
        "  reportInstanceId int not null,",
        "  typeId int not null,",
        "  typeRef1 int not null,",
        "  typeRef2 int not null,",
        "  activeTs bigint not null,",
        "  rtnApplicable char(1) not null,",
        "  rtnTs bigint,",
        "  rtnCause int,",
        "  alarmLevel int not null,",
        "  message clob",
        ");",
        "alter table reportInstanceEvents add constraint reportInstanceEventsPk primary key (eventId, reportInstanceId);",
        "alter table reportInstanceEvents add constraint reportInstanceEventsFk1 foreign key (reportInstanceId)",
        "  references reportInstances(id) on delete cascade;",
        "alter table reportInstances add includeEvents int;",
        "update reportInstances set includeEvents=1;",
        "alter table reportInstances alter includeEvents not null;",
        "alter table systemSettings add settingValueOld varchar(255);",
        "update systemSettings set settingValueOld=settingValue;",
        "alter table systemSettings drop settingValue;",
        "alter table systemSettings add settingValue clob;",
        "update systemSettings set settingValue=settingValueOld;",
        "alter table systemSettings drop settingValueOld;",
        "create table watchListUsers (",
        "  watchListId int not null,",
        "  userId int not null,",
        "  accessType int not null",
        ");",
        "alter table watchListUsers add constraint watchListUsersPk primary key (watchListId, userId);",
        "alter table watchListUsers add constraint watchListUsersFk1 foreign key (watchListId) references watchLists(id) on delete cascade;",
        "alter table watchListUsers add constraint watchListUsersFk2 foreign key (userId) references users(id) on delete cascade;",
        "alter table users add homeUrl varchar(255);",};

    private static String[] script2 = {"alter table dataSources alter xid not null;",
        "alter table dataSources add constraint dataSourcesUn1 unique (xid);",
        "alter table dataPoints alter xid not null;",
        "alter table dataPoints add constraint dataPointsUn1 unique (xid);",
        "alter table pointEventDetectors alter xid not null;",
        "alter table pointEventDetectors add constraint pointEventDetectorsUn1 unique (xid, dataPointId);",
        "alter table mangoViews alter data not null;", "drop table pointViews;", "drop table staticViews;",
        "alter table events drop inactiveTs;", "alter table events drop inactiveCause;",
        "alter table events drop inactiveCauseRef;",};

    private void xid() {
        // Default the xid values.
        DataSourceDao dataSourceDao = DataSourceDao.getInstance();
        List<Integer> dsids = ejt.queryForList("select id from dataSources", Integer.class);
        for (Integer dsid : dsids) {
            ejt.update("update dataSources set xid=? where id=?", new Object[]{dataSourceDao.generateUniqueXid(),
                dsid});
        }

        DataPointDao dataPointDao = DataPointDao.getInstance();
        List<Integer> dpids = ejt.queryForList("select id from dataPoints", Integer.class);
        for (Integer dpid : dpids) {
            ejt.update("update dataPoints set xid=? where id=?",
                    new Object[]{dataPointDao.generateUniqueXid(), dpid});
        }

        List<Integer> pedids = ejt.queryForList("select id from pointEventDetectors", Integer.class);
        for (Integer pedid : pedids) {
            ejt.update("update pointEventDetectors set xid=? where id=?", new Object[]{
                Common.generateXid(PointEventDetectorVO.XID_PREFIX), pedid});
        }
    }

    private void viewData() {
        ViewDao viewDao = ViewDao.getInstance();
        final DataPointDao dataPointDao = DataPointDao.getInstance();
        List<View> views = viewDao.getViews();

        // Pull all point/static view data from these tables and convert to view component versions.
        for (View view : views) {
            final View finalView = view;

            ejt.query("select x, y, content from staticViews where mangoViewId=?", new Object[]{view.getId()},
                    new RowCallbackHandler() {
                        @Override
                        public void processRow(ResultSet rs) throws SQLException {
                            HtmlComponent html = new HtmlComponent();
                            html.setLocation(rs.getInt(1), rs.getInt(2));
                            html.setContent(rs.getString(3));
                            finalView.addViewComponent(html);
                        }
                    });

            ejt.query("select x, y, dataPointId, nameOverride, settableOverride, bkgdColorOverride, displayControls, "
                    + "  grData " + "from pointViews where mangoViewId=?", new Object[]{view.getId()},
                    new RowCallbackHandler() {
                        @SuppressWarnings("synthetic-access")
                        @Override
                        public void processRow(ResultSet rs) throws SQLException {
                            GraphicRenderer gr = (GraphicRenderer) SerializationHelper.readObject(rs.getBlob(8)
                                    .getBinaryStream());

                            PointComponent pc = null;
                            if (gr instanceof AnalogImageSetRenderer) {
                                AnalogImageSetRenderer f = (AnalogImageSetRenderer) gr;
                                AnalogGraphicComponent t = new AnalogGraphicComponent();
                                t.tsetImageSet(f.getImageSet());
                                t.setMin(f.getMin());
                                t.setMax(f.getMax());
                                t.setDisplayText(f.isDisplayText());
                                pc = t;
                            } else if (gr instanceof BasicImageRenderer) {
                                pc = new SimpleImageComponent();
                            } else if (gr instanceof BasicRenderer) {
                                pc = new SimplePointComponent();
                            } else if (gr instanceof BinaryImageSetRenderer) {
                                BinaryImageSetRenderer f = (BinaryImageSetRenderer) gr;
                                BinaryGraphicComponent t = new BinaryGraphicComponent();
                                t.tsetImageSet(f.getImageSet());
                                t.setZeroImage(f.getZeroImage());
                                t.setOneImage(f.getOneImage());
                                t.setDisplayText(f.isDisplayText());
                                pc = t;
                            } else if (gr instanceof DynamicImageRenderer) {
                                DynamicImageRenderer f = (DynamicImageRenderer) gr;
                                DynamicGraphicComponent t = new DynamicGraphicComponent();
                                t.tsetDynamicImage(f.getDynamicImage());
                                t.setMin(f.getMin());
                                t.setMax(f.getMax());
                                t.setDisplayText(f.isDisplayText());
                                pc = t;
                            } else if (gr instanceof MultistateImageSetRenderer) {
                                MultistateImageSetRenderer f = (MultistateImageSetRenderer) gr;
                                MultistateGraphicComponent t = new MultistateGraphicComponent();
                                t.tsetImageSet(f.getImageSet());
                                t.setImageStateList(f.getImageStateList());
                                t.setDefaultImage(f.getDefaultImage());
                                t.setDisplayText(f.isDisplayText());
                                pc = t;
                            } else if (gr instanceof ScriptRenderer) {
                                ScriptRenderer f = (ScriptRenderer) gr;
                                ScriptComponent t = new ScriptComponent();
                                t.setScript(f.getScript());
                                pc = t;
                            } else if (gr instanceof ThumbnailRenderer) {
                                ThumbnailRenderer f = (ThumbnailRenderer) gr;
                                ThumbnailComponent t = new ThumbnailComponent();
                                t.setScalePercent(f.getScalePercent());
                                pc = t;
                            }

                            if (pc != null) {
                                pc.setLocation(rs.getInt(1), rs.getInt(2));
                                pc.tsetDataPoint(dataPointDao.getDataPoint(rs.getInt(3)));
                                pc.setNameOverride(rs.getString(4));
                                pc.setSettableOverride(charToBool(rs.getString(5)));
                                pc.setBkgdColorOverride(rs.getString(6));
                                pc.setDisplayControls(charToBool(rs.getString(7)));

                                finalView.addViewComponent(pc);
                            }
                        }
                    });

            viewDao.saveView(view);
        }
    }

    private void eventData() {
        ejt.update("update events set rtnTs=inactiveTs, rtnCause=inactiveCause where rtnApplicable='Y'");
        ejt.update("insert into userEvents (eventId, userId, silenced) ("
                + "  select e.id, u.id, 'N' from events e, users u " + "  where u.admin='Y' and u.disabled='N'"
                + "    and e.inactiveTs is null" + ")");
        ejt.update("insert into userEvents (eventId, userId, silenced, ackTs) ("
                + "  select e.id, u.id, 'Y', e.inactiveTs from events e, users u "
                + "  where u.admin='Y' and u.disabled='N'" + "    and e.inactiveTs is not null" + ")");
    }
}
