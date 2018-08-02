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
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.List;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.springframework.jdbc.core.PreparedStatementSetter;
import org.springframework.jdbc.core.RowCallbackHandler;

import br.org.scadabr.ShouldNeverHappenException;
import com.serotonin.mango.Common;
import com.serotonin.mango.view.chart.ChartRenderer;
import com.serotonin.mango.view.chart.ImageChartRenderer;
import com.serotonin.mango.view.chart.StatisticsChartRenderer;
import com.serotonin.mango.view.chart.TableChartRenderer;
import com.serotonin.mango.view.text.AnalogRenderer;
import com.serotonin.mango.view.text.BinaryTextRenderer;
import com.serotonin.mango.view.text.MultistateRenderer;
import com.serotonin.mango.view.text.PlainRenderer;
import com.serotonin.mango.view.text.RangeRenderer;
import com.serotonin.mango.view.text.TextRenderer;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
//import com.serotonin.mango.vo.dataSource.modbus.ModbusIpDataSourceVO;
//import com.serotonin.mango.vo.dataSource.modbus.ModbusPointLocatorVO;
//import com.serotonin.mango.vo.dataSource.modbus.ModbusSerialDataSourceVO;
import com.serotonin.mango.vo.dataSource.virtual.VirtualDataSourceVO;
import com.serotonin.mango.vo.dataSource.virtual.VirtualPointLocatorVO;
import com.serotonin.modbus4j.code.DataType;
import com.serotonin.modbus4j.code.RegisterRange;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.util.queue.IntQueue;
import br.org.scadabr.utils.TimePeriods;
import br.org.scadabr.vo.LoggingTypes;
import org.springframework.jdbc.core.RowMapper;

/**
 * @author Matthew Lohbihler
 */
public class Upgrade0_7_0 extends DBUpgrade {

    @Override
    protected void upgrade() throws Exception {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    protected String getNewSchemaVersion() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }
/*
    private static final Log log = LogFactory.getLog(Upgrade0_7_0.class);

    @Override
    public void upgrade() throws Exception {
        // Get the data sources from the field mapping version.
        try (OutputStream out = createUpdateLogOutputStream("0_7_0")) {
            // Get the data sources from the field mapping version.
            List<DataSourceVO<?>> dataSources = ejt.query(DATA_SOURCE_SELECT, new DataSourceRowMapper());
            log.info("Retrieved " + dataSources.size() + " data sources");

            // Get the data points from the field mapping version.
            List<DataPointVO> dataPoints = getDataPoints();
            log.info("Retrieved " + dataPoints.size() + " data points");

            // Run the first script.
            log.info("Running script 1");
            runScript(script1, out);

            // Save the data sources to BLOBs.
            for (DataSourceVO<?> ds : dataSources) {
                log.info("Saved data source " + ds.getId());
                insertDataSource(ds);
            }

            // Save the data points to BLOBs.
            for (DataPointVO dp : dataPoints) {
                log.info("Saved data point " + dp.getId());
                insertDataPoint(dp);
            }

            // Run the second script.
            log.info("Running script 2");
            runScript(script2, out);

            out.flush();
        }
    }

    @Override
    protected String getNewSchemaVersion() {
        return "0.8.0";
    }

    private static final String[] script1 = {
        // Drop foreign keys
        "alter table dataSourceUsers drop foreign key dataSourceUsersFk1;",
        "alter table dataPointUsers drop foreign key dataPointUsersFk1;",
        "alter table pointViews drop foreign key pointViewsFk2;",
        "alter table pointValues drop foreign key pointValuesFk1;",
        "alter table userWatchList drop foreign key userWatchListFk2;",
        "alter table pointEventDetectors drop foreign key pointEventDetectorsFk1;",
        // Drop the field mapping data point tables.
        "drop table locatorsMbx;",
        "drop table locatorVrtValues;",
        "drop table locatorsVrt;",
        "drop table dataPointRangeValues;",
        "drop table dataPointMultistateValues;",
        "drop table dataPoints;",
        // Drop the field mapping data source tables.
        "drop table dataSourceMbs;",
        "drop table dataSourceMbi;",
        "drop table dataSourceVrt;",
        "drop table dataSources;",
        // Recreate the data sources table.
        "create table dataSources (",
        "  id int not null generated by default as identity (start with 1, increment by 1),",
        "  name varchar(40) not null,",
        "  dataSourceType int not null,",
        "  data blob not null",
        ");",
        "alter table dataSources add constraint dataSourcesPk primary key (id);",
        // Recreate the data points table.
        "create table dataPoints (",
        "  id int not null generated by default as identity (start with 1, increment by 1),",
        "  dataSourceId int not null,",
        "  data blob not null",
        ");",
        "alter table dataPoints add constraint dataPointsPk primary key (id);",
        "alter table dataPoints add constraint dataPointsFk1 foreign key (dataSourceId) references dataSources(id);",};

    private static final String[] script2 = {
        // Recreate the foreign keys
        "alter table dataSourceUsers add constraint dataSourceUsersFk1 foreign key (dataSourceId) references dataSources(id);",
        "alter table dataPointUsers add constraint dataPointUsersFk1 foreign key (dataPointId) references dataPoints(id);",
        "alter table pointViews add constraint pointViewsFk2 foreign key (dataPointId) references dataPoints(id);",
        "alter table pointValues add constraint pointValuesFk1 foreign key (dataPointId) references dataPoints(id);",
        "alter table userWatchList add constraint userWatchListFk2 foreign key (dataPointId) references dataPoints(id);",
        "alter table pointEventDetectors add constraint pointEventDetectorsFk1 foreign key (dataPointId) references dataPoints(id);",
        // Create the scheduled events table.
        "create table scheduledEvents (",
        "  id int not null generated by default as identity (start with 1, increment by 1),",
        "  alarmLevel int not null,", "  scheduleType int not null,", "  returnToNormal char(1) not null,",
        "  disabled char(1) not null,", "  activeYear int,", "  activeMonth int,", "  activeDay int,",
        "  activeHour int,", "  activeMinute int,", "  activeSecond int,", "  activeCron varchar(25),",
        "  inactiveYear int,", "  inactiveMonth int,", "  inactiveDay int,", "  inactiveHour int,",
        "  inactiveMinute int,", "  inactiveSecond int,",
        "  inactiveCron varchar(25)",
        ");",
        "alter table scheduledEvents add constraint scheduledEventsPk primary key (id);",
        // Create the point hierarchy table
        "create table pointHierarchy (",
        "  id int not null generated by default as identity (start with 1, increment by 1),", "  parentId int,",
        "  name varchar(100)", ");",
        "alter table pointHierarchy add constraint pointHierarchyPk primary key (id);",};

    //
    // / Data sources.
    //
    private static final String DATA_SOURCE_SELECT = "select ds.id, ds.name, ds.dataSourceType, ds.enabled, "
            + "  vrt.updateSeconds, "
            + "  mbs.updateSeconds, mbs.commPortId, mbs.baudRate, mbs.flowControlIn, mbs.flowControlOut, mbs.dataBits, "
            + "  mbs.stopBits, mbs.parity, mbs.encoding, mbs.echo, "
            + "  mbi.updateSeconds, mbi.transportType, mbi.host, mbi.port " + "from dataSources ds "
            + "  left join dataSourceVrt vrt on vrt.dataSourceId = ds.id"
            + "  left join dataSourceMbs mbs on mbs.dataSourceId = ds.id"
            + "  left join dataSourceMbi mbi on mbi.dataSourceId = ds.id";

    class DataSourceRowMapper implements RowMapper<DataSourceVO<?>> {

        @SuppressWarnings("synthetic-access")
        @Override
        public DataSourceVO<?> mapRow(ResultSet rs, int rowNum) throws SQLException {
            int id = rs.getInt(1);
            String name = rs.getString(2);
            int typeId = rs.getInt(3);
            boolean enabled = charToBool(rs.getString(4));

            DataSourceVO<?> ds = null;
            if (typeId == 1) {
                VirtualDataSourceVO d = new VirtualDataSourceVO();
                d.setUpdatePeriods(rs.getInt(5));
                d.setUpdatePeriodType(TimePeriods.SECONDS);
                ds = d;
            } else if (typeId == 2) {
                ModbusSerialDataSourceVO d = new ModbusSerialDataSourceVO();
                d.setUpdatePeriods(rs.getInt(6));
                d.setUpdatePeriodType(TimePeriods.SECONDS);
                d.setCommPortId(rs.getString(7));
                d.setBaudRate(rs.getInt(8));
                d.setFlowControlIn(rs.getInt(9));
                d.setFlowControlOut(rs.getInt(10));
                d.setDataBits(rs.getInt(11));
                d.setStopBits(rs.getInt(12));
                d.setParity(rs.getInt(13));
                d.setEncodingStr(rs.getString(14));
                d.setEcho(charToBool(rs.getString(15)));
                ds = d;
            } else if (typeId == 3) {
                ModbusIpDataSourceVO d = new ModbusIpDataSourceVO();
                d.setUpdatePeriods(rs.getInt(16));
                d.setUpdatePeriodType(TimePeriods.SECONDS);
                d.setTransportTypeStr(rs.getString(17));
                d.setHost(rs.getString(18));
                d.setPort(rs.getInt(19));
                ds = d;
            } else {
                throw new ShouldNeverHappenException("Unknown data source type: " + typeId + ", id=" + id);
            }

            ds.setId(id);
            ds.setName(name);
            ds.setEnabled(enabled);
            return ds;
        }
    }

    private void insertDataSource(final DataSourceVO<?> vo) {
        ejt.update("insert into dataSources (id, name, dataSourceType, data) values (?,?,?,?)",
                new PreparedStatementSetter() {
                    @Override
                    public void setValues(PreparedStatement ps) throws SQLException {
                        ps.setInt(1, vo.getId());
                        ps.setString(2, vo.getName());
                        ps.setInt(3, vo.getType().getId());
                        ps.setBlob(4, SerializationHelper.writeObject(vo));
                    }
                });
    }

    //
    // / Data points.
    //
    private static final String DATA_POINT_SELECT = "select dp.id, dp.name, dp.dataSourceId, dp.enabled, "
            + "  dp.loggingType, dp.tolerance, dp.purgeType, dp.purgePeriod, "
            + "  dp.trType, dp.trAnalogFormat, dp.trAnalogSuffix, dp.trBinaryZeroLabel, dp.trBinaryZeroColour, "
            + "    dp.trBinaryOneLabel, dp.trBinaryOneColour, dp.trPlainSuffix, dp.trRangeFormat, "
            + "  dp.crType, dp.crTableLimit, dp.crImageTimePeriod, dp.crImageNumberOfPeriods, dp.crStatsTimePeriod, "
            + "    dp.crStatsNumberOfPeriods, " + "  ds.name, ds.dataSourceType, "
            + "  vrt.dataTypeId, vrt.changeTypeId, vrt.settable, vrt.startValue, vrt.type2Min, vrt.type2Max, "
            + "  vrt.type2MaxChange, vrt.type3Min, vrt.type3Max, vrt.type3Change, vrt.type3Roll, vrt.type4Roll, "
            + "  vrt.type6Min, vrt.type6Max, vrt.type9MaxChange, vrt.type9Volatility, vrt.type9AttractionPointId, "
            + "  mbx.registerRange, mbx.modbusDataType, mbx.slaveId, mbx.offset, mbx.bitOffset, mbx.multiplier, "
            + "  mbx.additive " + "from dataPoints dp " + "  left join locatorsVrt vrt on vrt.dataPointId = dp.id"
            + "  left join locatorsMbx mbx on mbx.dataPointId = dp.id"
            + "  join dataSources ds on ds.id = dp.dataSourceId ";

    public List<DataPointVO> getDataPoints() {
        List<DataPointVO> dps = ejt.query(DATA_POINT_SELECT, new DataPointRowMapper());
        for (DataPointVO dp : dps) {
            setRelationalData(dp);
        }
        return dps;
    }

    class DataPointRowMapper implements RowMapper<DataPointVO> {

        @SuppressWarnings("synthetic-access")
        @Override
        public DataPointVO mapRow(ResultSet rs, int rowNum) throws SQLException {
            DataPointVO dp = new DataPointVO();

            // Set common data point properties.
            int i = 0;
            dp.setId(rs.getInt(++i));
            dp.setName(rs.getString(++i));
            dp.setDataSourceId(rs.getInt(++i));
            dp.setEnabled(charToBool(rs.getString(++i)));
            dp.setLoggingType(LoggingTypes.fromMangoDbId(rs.getInt(++i)));
            dp.setTolerance(rs.getDouble(++i));
            dp.setPurgeType(TimePeriods.fromMangoDbId(rs.getInt(++i)));
            dp.setPurgePeriod(rs.getInt(++i));

            // Set the text renderer.
            int trType = rs.getInt(++i);
            String analogFormat = rs.getString(++i);
            String analogSuffix = rs.getString(++i);
            String binaryZeroLabel = rs.getString(++i);
            String binaryZeroColour = rs.getString(++i);
            String binaryOneLabel = rs.getString(++i);
            String binaryOneColour = rs.getString(++i);
            String plainSuffix = rs.getString(++i);
            String rangeFormat = rs.getString(++i);

            TextRenderer textRenderer;
            switch (trType) {
                case 1:
                    textRenderer = new AnalogRenderer(analogFormat, analogSuffix);
                    break;
                case 2:
                    textRenderer = new BinaryTextRenderer(binaryZeroLabel, binaryZeroColour, binaryOneLabel,
                            binaryOneColour);
                    break;
                case 3:
                    textRenderer = new MultistateRenderer();
                    break;
                case 4:
                    textRenderer = new PlainRenderer(plainSuffix);
                    break;
                case 5:
                    textRenderer = new RangeRenderer(rangeFormat);
                    break;
                default:
                    throw new ShouldNeverHappenException("Unknown text renderer type: " + trType);
            }
            dp.setTextRenderer(textRenderer);

            // Set the chart renderer.
            int crType = rs.getInt(++i);
            int tableLimit = rs.getInt(++i);
            TimePeriods imageTimePeriod = TimePeriods.fromMangoDbId(rs.getInt(++i));
            int imageNumberOfPeriods = rs.getInt(++i);
            TimePeriods statsTimePeriod = TimePeriods.fromMangoDbId(rs.getInt(++i));
            int statsNumberOfPeriods = rs.getInt(++i);

            ChartRenderer chartRenderer;
            switch (crType) {
                case 1:
                    chartRenderer = null;
                    break;
                case 2:
                    chartRenderer = new TableChartRenderer(tableLimit);
                    break;
                case 3:
                    chartRenderer = new ImageChartRenderer(imageTimePeriod, imageNumberOfPeriods);
                    break;
                case 4:
                    chartRenderer = new StatisticsChartRenderer(statsTimePeriod, statsNumberOfPeriods, true);
                    break;
                default:
                    throw new ShouldNeverHappenException("Unknown chart renderer type: " + crType);
            }
            dp.setChartRenderer(chartRenderer);

            // Data source information.
            dp.setDataSourceName(rs.getString(++i));
            dp.setDataSourceTypeId(rs.getInt(++i));

            // Create the appropriate point locator.
            switch (dp.getDataSourceTypeId()) {
                case 1:
                    VirtualPointLocatorVO pl1 = new VirtualPointLocatorVO();
                    dp.setPointLocator(pl1);
                    pl1.setDataType(br.org.scadabr.DataType.fromMangoDbId(rs.getInt(i + 1)));
                    pl1.setChangeTypeId(rs.getInt(i + 2));
                    pl1.setSettable(charToBool(rs.getString(i + 3)));

                    // Create the appropriate change type
                    switch (pl1.getChangeTypeId()) {
                        case 1:
                            pl1.getAlternateBooleanChange().setStartValue(rs.getString(i + 4));
                            break;
                        case 2:
                            pl1.getBrownianChange().setStartValue(rs.getString(i + 4));
                            pl1.getBrownianChange().setMin(rs.getDouble(i + 5));
                            pl1.getBrownianChange().setMax(rs.getDouble(i + 6));
                            pl1.getBrownianChange().setMaxChange(rs.getDouble(i + 7));
                            break;
                        case 3:
                            pl1.getIncrementAnalogChange().setStartValue(rs.getString(i + 4));
                            pl1.getIncrementAnalogChange().setMin(rs.getDouble(i + 8));
                            pl1.getIncrementAnalogChange().setMax(rs.getDouble(i + 9));
                            pl1.getIncrementAnalogChange().setChange(rs.getDouble(i + 10));
                            pl1.getIncrementAnalogChange().setRoll(charToBool(rs.getString(i + 11)));
                            break;
                        case 4:
                            pl1.getIncrementMultistateChange().setStartValue(rs.getString(i + 4));
                            pl1.getIncrementMultistateChange().setRoll(charToBool(rs.getString(i + 12)));
                            break;
                        case 5:
                            pl1.getNoChange().setStartValue(rs.getString(i + 4));
                            break;
                        case 6:
                            pl1.getRandomAnalogChange().setStartValue(rs.getString(i + 4));
                            pl1.getRandomAnalogChange().setMin(rs.getDouble(i + 13));
                            pl1.getRandomAnalogChange().setMax(rs.getDouble(i + 14));
                            break;
                        case 7:
                            pl1.getRandomBooleanChange().setStartValue(rs.getString(i + 4));
                            break;
                        case 8:
                            pl1.getRandomMultistateChange().setStartValue(rs.getString(i + 4));
                            break;
                        case 9:
                            pl1.getAnalogAttractorChange().setStartValue(rs.getString(i + 4));
                            pl1.getAnalogAttractorChange().setMaxChange(rs.getDouble(i + 15));
                            pl1.getAnalogAttractorChange().setVolatility(rs.getDouble(i + 16));
                            pl1.getAnalogAttractorChange().setAttractionPointId(rs.getInt(i + 17));
                            break;
                        default:
                            throw new ShouldNeverHappenException("Unknown Virtual Locator Change Type: "
                                    + pl1.getChangeTypeId());
                    }
                    break;

                case 2:
                case 3:
                    ModbusPointLocatorVO pl2 = new ModbusPointLocatorVO();
                    dp.setPointLocator(pl2);

                    // pl2.setRange(RegisterRange.valueOf(rs.getString(i+18)));
                    // pl2.setModbusDataType(DataType.fromMangoDbId(rs.getString(i+19)));
                    pl2.setRange(RegisterRange.COIL_STATUS);
                    pl2.setModbusDataType(DataType.BINARY);
                    pl2.setSlaveId(rs.getInt(i + 20));
                    pl2.setOffset(rs.getInt(i + 21));
                    pl2.setBit((byte) rs.getInt(i + 22));
                    pl2.setMultiplier(rs.getDouble(i + 23));
                    pl2.setAdditive(rs.getDouble(i + 24));
                    break;
            }

            return dp;
        }
    }

    private void setRelationalData(DataPointVO dp) {
        if (dp.getDataSourceTypeId() == DataSourceVO.Type.VIRTUAL.getId()) {
            VirtualPointLocatorVO tpl = dp.getPointLocator();
            int changeType = tpl.getChangeTypeId();
            if (changeType == 4 || changeType == 8) {

                // Extract the values from the database.
                final IntQueue queue = new IntQueue();
                ejt.query("select multistateValue from locatorVrtValues where dataPointId=? order by valueOrder",
                        new Object[]{dp.getId()}, new RowCallbackHandler() {
                            @Override
                            public void processRow(ResultSet rs) throws SQLException {
                                queue.push(rs.getInt(1));
                            }
                        });

                // Put the results into the appropriate locator.
                if (changeType == 4) {
                    tpl.getIncrementMultistateChange().setValues(queue.popAll());
                } else if (changeType == 8) {
                    tpl.getRandomMultistateChange().setValues(queue.popAll());
                }
            }
        }

        // Get values for multistate and range text renderers.
        if (dp.getTextRenderer() instanceof MultistateRenderer) {
            final MultistateRenderer dr = (MultistateRenderer) dp.getTextRenderer();
            // Extract the values from the database.
            ejt.query("select multistateKey, multistateValue, multistateColour from dataPointMultistateValues "
                    + "where dataPointId=? order by valueOrder", new Object[]{dp.getId()}, new RowCallbackHandler() {
                        @Override
                        public void processRow(ResultSet rs) throws SQLException {
                            dr.addMultistateValue(rs.getInt(1), rs.getString(2), rs.getString(3));
                        }
                    });
        } else if (dp.getTextRenderer() instanceof RangeRenderer) {
            final RangeRenderer rr = (RangeRenderer) dp.getTextRenderer();
            // Extract the values from the database.
            ejt.query("select rangeFrom, rangeTo, rangeValue, rangeColour from dataPointRangeValues "
                    + "where dataPointId=? order by valueOrder", new Object[]{dp.getId()}, new RowCallbackHandler() {
                        @Override
                        public void processRow(ResultSet rs) throws SQLException {
                            rr.addRangeValues(rs.getDouble(1), rs.getDouble(2), rs.getString(3), rs.getString(4));
                        }
                    });
        }
    }

    private void insertDataPoint(final DataPointVO vo) {
        ejt.update("insert into dataPoints (id, dataSourceId, data) values (?,?,?)", new PreparedStatementSetter() {
            @Override
            public void setValues(PreparedStatement ps) throws SQLException {
                ps.setInt(1, vo.getId());
                ps.setInt(2, vo.getDataSourceId());
                ps.setBlob(3, SerializationHelper.writeObject(vo));
            }
        });
    }
    */
}
