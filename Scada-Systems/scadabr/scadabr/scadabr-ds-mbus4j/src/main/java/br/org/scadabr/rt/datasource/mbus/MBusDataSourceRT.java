/*
 *   Mango - Open Source M2M - http://mango.serotoninsoftware.com
 *   Copyright (C) 2010 Arne Pl\u00f6se
 *   @author Arne Pl\u00f6se
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
package br.org.scadabr.rt.datasource.mbus;

import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.timer.cron.CronExpression;
import br.org.scadabr.timer.cron.CronParser;
import java.io.IOException;

import net.sf.mbus4j.dataframes.datablocks.BigDecimalDataBlock;
import net.sf.mbus4j.dataframes.datablocks.IntegerDataBlock;
import net.sf.mbus4j.dataframes.datablocks.LongDataBlock;
import net.sf.mbus4j.dataframes.datablocks.RealDataBlock;
import net.sf.mbus4j.dataframes.datablocks.ShortDataBlock;
import net.sf.mbus4j.dataframes.datablocks.StringDataBlock;
import br.org.scadabr.vo.datasource.mbus.MBusDataSourceVO;
import net.sf.mbus4j.master.MBusMaster;
import net.sf.mbus4j.master.ValueRequest;
import net.sf.mbus4j.master.ValueRequestPointLocator;

import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.SetPointSource;
import com.serotonin.mango.rt.dataSource.PollingDataSource;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import java.text.ParseException;
import java.util.TimeZone;
import java.util.logging.Level;
import java.util.logging.Logger;
import net.sf.mbus4j.dataframes.datablocks.BcdValue;

/**
 * TODO datatype NUMERIC_INT is missing TODO Starttime for timpepoints ???
 */
public class MBusDataSourceRT extends PollingDataSource<MBusDataSourceVO> {

    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCARABR_DS_MBUS);
    public static final int DATA_SOURCE_EXCEPTION_EVENT = 1;
    public static final int POINT_READ_EXCEPTION_EVENT = 2;
    public static final int POINT_WRITE_EXCEPTION_EVENT = 3;
    private final MBusMaster master = new MBusMaster();

    public MBusDataSourceRT(MBusDataSourceVO vo) {
        super(vo, true);
    }

    public double calcCorretedValue(DataPointRT point, double value) {
        MBusPointLocatorRT mPoint = (MBusPointLocatorRT) point.getPointLocator();
        return mPoint.getVo().getCorrectionFactor() * value;
    }

    @Override
    public synchronized void doPoll(long time) {
        updateChangedPoints();

        boolean pointError = false;
        boolean dsError = false;

        final ValueRequest<DataPointRT> request = new ValueRequest<>();
        for (DataPointRT point : enabledDataPoints.values()) {
            final MBusPointLocatorRT locator = point.getPointLocator();
            request.add(locator.createValueRequestPointLocator(point));
        }

        if (openConnection()) {
            try {
                master.readValues(request);
                for (ValueRequestPointLocator<DataPointRT> vr : request) {
                    try {
                        if (vr.getDb() == null) {

                            pointError = true;

                            raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, time, true,
                                    new LocalizableMessageImpl("event.exception2", vo.getName(),
                                            "No datablock found for: ", vr.getReference().getVo().getName()));

                        } else if ((vr.getDb() instanceof BcdValue) && ((BcdValue) vr.getDb()).isBcdError()) {
                            pointError = true;
                            LOG.log(Level.SEVERE, "BCD Error : {0}", ((BcdValue) vr.getDb()).getBcdError());
                            raiseEvent(POINT_READ_EXCEPTION_EVENT, time, true,
                                    new LocalizableMessageImpl("event.exception2", vo.getName(),
                                            String.format("BCD error %s value: ", vr.getReference().getVo().getName()), ((BcdValue) vr.getDb()).getBcdError()));
                        } else if (vr.getDb() instanceof ShortDataBlock) {
                            vr.getReference().updatePointValue(
                                    new PointValueTime(calcCorretedValue(vr.getReference(), ((ShortDataBlock) vr.getDb()).getValue()), time));
                        } else if (vr.getDb() instanceof IntegerDataBlock) {
                            vr.getReference().updatePointValue(
                                    new PointValueTime(calcCorretedValue(vr.getReference(), ((IntegerDataBlock) vr.getDb()).getValue()), time));
                        } else if (vr.getDb() instanceof LongDataBlock) {
                            vr.getReference().updatePointValue(
                                    new PointValueTime(calcCorretedValue(vr.getReference(), ((LongDataBlock) vr.getDb()).getValue()), time));
                        } else if (vr.getDb() instanceof RealDataBlock) {
                            vr.getReference().updatePointValue(
                                    new PointValueTime(calcCorretedValue(vr.getReference(), ((RealDataBlock) vr.getDb()).getValue()), time));
                        } else if (vr.getDb() instanceof BigDecimalDataBlock) {
                            vr.getReference().updatePointValue(
                                    new PointValueTime(calcCorretedValue(vr.getReference(), ((BigDecimalDataBlock) vr.getDb()).getValue().doubleValue()), time));
                        } else if (vr.getDb() instanceof StringDataBlock) {
                            vr.getReference().updatePointValue(
                                    new PointValueTime(((StringDataBlock) vr.getDb()).getValue(), time));
                        } else {
                            LOG.log(Level.SEVERE, "Dont know how to save: {0}", vr.getReference().getVo().getName());
                            raiseEvent(POINT_READ_EXCEPTION_EVENT, System.currentTimeMillis(), true,
                                    new LocalizableMessageImpl("event.exception2", vo.getName(),
                                            "Dont know how to save: ", vr.getReference().getVo().getName()));

                        }
                    } catch (Exception ex) {
                        LOG.log(Level.SEVERE, "Error during saving: " + vo.getName(), ex);
                        raiseEvent(POINT_READ_EXCEPTION_EVENT, System.currentTimeMillis(), true,
                                new LocalizableMessageImpl("event.exception2", vo.getName(),
                                        "Dont know how to save : ", ex));
                    }

                }

                if (!pointError) {
                    returnToNormal(POINT_READ_EXCEPTION_EVENT, time);
                }

                returnToNormal(DATA_SOURCE_EXCEPTION_EVENT, time);
            } catch (InterruptedException ex) {
                LOG.log(Level.SEVERE, "doPoll() interrupted", ex);
                raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                        "event.exception2", vo.getName(), ex.getMessage(), "doPoll() Interrupted"));
            } catch (IOException ex) {
                LOG.log(Level.SEVERE, "doPoll() IO Ex", ex);
                raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                        "event.exception2", vo.getName(), ex.getMessage(), "doPoll() IO Ex"));
            } finally {
                closeConnection();
            }
        }
    }

    @Override
    public void setPointValue(DataPointRT dataPoint, PointValueTime valueTime, SetPointSource source) {
        // no op
    }

    private boolean openConnection() {
        try {
            LOG.fine("MBus Try open serial port");
            master.setConnection(vo.getConnection());
            master.open();
            return true;
        } catch (IOException ex) {
            LOG.log(Level.SEVERE, "MBus Open serial port exception", ex);
            master.setConnection(null);
            raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                    "event.exception2", vo.getName(), ex.getMessage(), "openConnection() Failed"));
            return false;
        }
    }

    private void closeConnection() {
        try {
            master.close();
        } catch (IOException ex) {
            LOG.log(Level.SEVERE, "Close port", ex);
            raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                    "event.exception2", vo.getName(), ex.getMessage(), "closeConnection() Failed"));
        } finally {
            master.setConnection(null);
        }
    }

    @Override
    protected CronExpression getCronExpression() throws ParseException {
        return new CronParser().parse(vo.getCronPattern(), TimeZone.getTimeZone(vo.getCronTimeZone()));
    }
}
