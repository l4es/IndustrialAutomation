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
package com.serotonin.mango.rt.dataSource.modbus;

import br.org.scadabr.DataType;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.utils.i18n.LocalizableException;
import java.net.ConnectException;
import java.nio.charset.Charset;
import java.util.HashMap;
import java.util.Map;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import br.org.scadabr.ShouldNeverHappenException;
import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.SetPointSource;
import com.serotonin.mango.rt.dataSource.DataSourceRT;
import com.serotonin.mango.rt.dataSource.PollingDataSource;
import com.serotonin.mango.vo.dataSource.modbus.ModbusDataSourceVO;
import com.serotonin.mango.vo.dataSource.modbus.ModbusPointLocatorVO;
import com.serotonin.messaging.MessagingExceptionHandler;
import com.serotonin.messaging.TimeoutException;
import com.serotonin.modbus4j.BatchRead;
import com.serotonin.modbus4j.BatchResults;
import com.serotonin.modbus4j.ExceptionResult;
import com.serotonin.modbus4j.ModbusMaster;
import com.serotonin.modbus4j.exception.ErrorResponseException;
import com.serotonin.modbus4j.exception.ModbusTransportException;
import com.serotonin.modbus4j.locator.BaseLocator;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;

abstract public class ModbusDataSource<T extends ModbusDataSourceVO<T>> extends PollingDataSource<T> implements MessagingExceptionHandler {

    private final Log LOG = LogFactory.getLog(ModbusDataSource.class);

    public static final int POINT_READ_EXCEPTION_EVENT = 1;
    public static final int POINT_WRITE_EXCEPTION_EVENT = 2;
    public static final int DATA_SOURCE_EXCEPTION_EVENT = 3;

    private ModbusMaster modbusMaster;
    private BatchRead<ModbusPointLocatorRT> batchRead;
    private final Map<Integer, DataPointRT> slaveMonitors = new HashMap<>();

    public ModbusDataSource(T vo, boolean doCache) {
        super(vo, doCache);
        setPollingPeriod(vo.getUpdatePeriodType(), vo.getUpdatePeriods(), vo.isQuantize());
    }

    /*
     @Override
     public void addDataPoint(DataPointRT dataPoint) {
     super.addDataPoint(dataPoint);

     // Mark the point as unreliable.
     ModbusPointLocatorVO locatorVO = dataPoint.getVo().getPointLocator();
     if (!locatorVO.isSlaveMonitor()) {
     dataPoint.setAttribute(ATTR_UNRELIABLE_KEY, true);
     }

     // Slave monitor points.
     if (vo.isCreateSlaveMonitorPoints()) {
     int slaveId = locatorVO.getSlaveId();

     if (locatorVO.isSlaveMonitor()) // The monitor for this slave. Set it in the map.
     {
     slaveMonitors.put(slaveId, dataPoint);
     } else if (!slaveMonitors.containsKey(slaveId)) {
     // A new slave. Add null to the map to ensure we don't do this check again.
     slaveMonitors.put(slaveId, null);

     // Check if a monitor point already exists.
     DataPointDao dataPointDao = new DataPointDao();
     boolean found = false;

     List<DataPointVO> points = dataPointDao.getDataPoints(vo.getId(), null);
     for (DataPointVO dp : points) {
     ModbusPointLocatorVO loc = dp.getPointLocator();
     if (loc.getSlaveId() == slaveId && loc.isSlaveMonitor()) {
     found = true;
     break;
     }
     }

     if (!found) {
     // A monitor was not found, so create one
     DataPointVO dp = new DataPointVO();
     dp.setXid(dataPointDao.generateUniqueXid());
     dp.setName(Common.getMessage("dsEdit.modbus.monitorPointName", slaveId));
     dp.setDataSourceId(vo.getId());
     dp.setEnabled(true);
     dp.setLoggingType(LoggingTypes.ON_CHANGE);
     dp.setEventDetectors(new ArrayList<PointEventDetectorVO>());

     ModbusPointLocatorVO locator = new ModbusPointLocatorVO();
     locator.setSlaveId(slaveId);
     locator.setSlaveMonitor(true);
     dp.setPointLocator(locator);

     Common.ctx.getRuntimeManager().saveDataPoint(dp);
     LOG.info("Monitor point added: " + dp.getXid());
     }
     }
     }
     }

     @Override
     public void removeDataPoint(DataPointRT dataPoint) {
     synchronized (pointListChangeLock) {
     super.removeDataPoint(dataPoint);

     // If this is a slave monitor point being removed, also remove it from the map.
     ModbusPointLocatorVO locatorVO = dataPoint.getVo().getPointLocator();
     if (locatorVO.isSlaveMonitor()) {
     slaveMonitors.put(locatorVO.getSlaveId(), null);
     }
     }
     }
     */
    @Override
    public void doPoll(long time) {
        updateChangedPoints();

        if (!modbusMaster.isInitialized()) {
            if (vo.isCreateSlaveMonitorPoints()) {
                // Set the slave monitors to offline
                for (DataPointRT monitor : slaveMonitors.values()) {
                    if (monitor != null) {
                        PointValueTime oldValue = monitor.getPointValue();
                        if (oldValue == null || oldValue.getBooleanValue()) {
                            monitor.setPointValue(new PointValueTime(false, time), null);
                        }
                    }
                }
            }

            return;
        }

        ModbusPointLocatorRT locator;
        BaseLocator<?> modbusLocator;
        Object result;

        if (batchRead == null) {
            batchRead = new BatchRead<>();
            batchRead.setContiguousRequests(vo.isContiguousBatches());
            batchRead.setErrorsInResults(true);
            batchRead.setExceptionsInResults(true);

            for (DataPointRT dataPoint : enabledDataPoints.values()) {
                locator = dataPoint.getPointLocator();
                if (!locator.getVo().isSlaveMonitor()) {
                    modbusLocator = createModbusLocator(locator.getVo());
                    batchRead.addLocator(locator, modbusLocator);
                }
            }
        }

        try {
            BatchResults<ModbusPointLocatorRT> results = modbusMaster.send(batchRead);

            Map<Integer, Boolean> slaveStatuses = new HashMap<>();
            boolean dataSourceExceptions = false;

            for (DataPointRT dataPoint : enabledDataPoints.values()) {
                locator = dataPoint.getPointLocator();
                if (locator.getVo().isSlaveMonitor()) {
                    continue;
                }

                result = results.getValue(locator);

                if (result instanceof ExceptionResult) {
                    ExceptionResult exceptionResult = (ExceptionResult) result;

                    // Raise an event.
                    raiseEvent(POINT_READ_EXCEPTION_EVENT, time, true, new LocalizableMessageImpl("event.exception2",
                            dataPoint.getVo().getName(), exceptionResult.getExceptionMessage()));

                    dataPoint.setAttribute(ATTR_UNRELIABLE_KEY, true);

                    // A response, albeit an undesirable one, was received from the slave, so it is online.
                    slaveStatuses.put(locator.getVo().getSlaveId(), true);
                } else if (result instanceof ModbusTransportException) {
                    ModbusTransportException e = (ModbusTransportException) result;

                    // Update the slave status. Only set to false if it is not true already.
                    if (!slaveStatuses.containsKey(locator.getVo().getSlaveId())) {
                        slaveStatuses.put(locator.getVo().getSlaveId(), false);
                    }

                    // Raise an event.
                    raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, time, true, wrapException(e));
                    dataSourceExceptions = true;

                    dataPoint.setAttribute(ATTR_UNRELIABLE_KEY, true);
                } else {
                    // Deactivate any existing event.
                    returnToNormal(POINT_READ_EXCEPTION_EVENT, time);
                    dataPoint.setAttribute(ATTR_UNRELIABLE_KEY, false);
                    updatePointValue(dataPoint, locator, result, time);
                    slaveStatuses.put(locator.getVo().getSlaveId(), true);
                }
            }

            if (vo.isCreateSlaveMonitorPoints()) {
                for (Map.Entry<Integer, Boolean> status : slaveStatuses.entrySet()) {
                    DataPointRT monitor = slaveMonitors.get(status.getKey());
                    if (monitor != null) {
                        boolean oldOnline;
                        boolean newOnline = status.getValue();

                        PointValueTime oldValue = monitor.getPointValue();
                        if (oldValue != null) {
                            oldOnline = oldValue.getBooleanValue();
                        } else // Make sure it gets set.
                        {
                            oldOnline = !newOnline;
                        }

                        if (oldOnline != newOnline) {
                            monitor.setPointValue(new PointValueTime(newOnline, time), null);
                        }
                    }
                }
            }

            if (!dataSourceExceptions) // Deactivate any existing event.
            {
                returnToNormal(DATA_SOURCE_EXCEPTION_EVENT, time);
            }
        } catch (ErrorResponseException e) {
            // Should never happen because we set "errorsInResults" to true.
            throw new ShouldNeverHappenException(e);
        } catch (ModbusTransportException e) {
            // Should never happen because we set "exceptionsInResults" to true.
            throw new ShouldNeverHappenException(e);
        }
    }

    protected void initialize(ModbusMaster modbusMaster) {
        this.modbusMaster = modbusMaster;
        modbusMaster.setTimeout(vo.getTimeout());
        modbusMaster.setRetries(vo.getRetries());
        modbusMaster.setMaxReadBitCount(vo.getMaxReadBitCount());
        modbusMaster.setMaxReadRegisterCount(vo.getMaxReadRegisterCount());
        modbusMaster.setMaxWriteRegisterCount(vo.getMaxWriteRegisterCount());

        // Add this as a listener to exceptions that occur in the implementation.
        modbusMaster.setExceptionHandler(this);

        try {
            modbusMaster.init();

            // Deactivate any existing event.
            returnToNormal(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis());
        } catch (Exception e) {
            raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis(), true, wrapException(e));
            LOG.debug("Error while initializing data source", e);
            return;
        }

        super.initialize();
    }

    @Override
    public void forcePointRead(DataPointRT dataPoint) {
        throw new ImplementMeException();
        /*
         ModbusPointLocatorRT pl = dataPoint.getPointLocator();
         if (pl.getVo().isSlaveMonitor()) // Nothing to do
         {
         return;
         }

         BaseLocator<?> ml = createModbusLocator(pl.getVo());
         long time = System.currentTimeMillis();

         synchronized (pointListChangeLock) {
         try {
         Object value = modbusMaster.getValue(ml);

         returnToNormal(POINT_READ_EXCEPTION_EVENT, time);
         dataPoint.setAttribute(ATTR_UNRELIABLE_KEY, false);

         updatePointValue(dataPoint, pl, value, time);
         } catch (ErrorResponseException e) {
         raiseEvent(POINT_READ_EXCEPTION_EVENT, time, true, new LocalizableMessageImpl("event.exception2", dataPoint
         .getVo().getName(), e.getMessage()));
         dataPoint.setAttribute(ATTR_UNRELIABLE_KEY, true);
         } catch (ModbusTransportException e) {
         // Don't raise a data source exception. Polling should do that.
         LOG.warn("Error during forcePointRead", e);
         dataPoint.setAttribute(ATTR_UNRELIABLE_KEY, true);
         }
         }
         */
    }

    private void updatePointValue(DataPointRT dataPoint, ModbusPointLocatorRT pl, Object value, long time) {
        if (pl.getDataType() == DataType.BINARY) {
            dataPoint.updatePointValue(new PointValueTime((Boolean) value, time));
        } else if (pl.getDataType() == DataType.ALPHANUMERIC) {
            dataPoint.updatePointValue(new PointValueTime((String) value, time));
        } else {
            // Apply arithmetic conversions.
            double newValue = ((Number) value).doubleValue();
            newValue *= pl.getVo().getMultiplier();
            newValue += pl.getVo().getAdditive();
            dataPoint.updatePointValue(new PointValueTime(newValue, time));
        }
    }

    @Override
    public void terminate() {
        super.terminate();
        modbusMaster.destroy();
    }

    //
    //
    // Data source interface
    //
    @Override
    public void setPointValue(DataPointRT dataPoint, PointValueTime valueTime, SetPointSource source) {
        ModbusPointLocatorRT pl = dataPoint.getPointLocator();
        BaseLocator<?> ml = createModbusLocator(pl.getVo());

        try {
            // See if this is a numeric value that needs to be converted.
            if (dataPoint.getDataType() == DataType.NUMERIC) {
                double convertedValue = valueTime.getDoubleValue();
                convertedValue -= pl.getVo().getAdditive();
                convertedValue /= pl.getVo().getMultiplier();
                modbusMaster.setValue(ml, convertedValue);
            } else if (dataPoint.getDataType() == DataType.ALPHANUMERIC) {
                modbusMaster.setValue(ml, valueTime.getStringValue());
            } else {
                modbusMaster.setValue(ml, valueTime.getBooleanValue());
            }
            dataPoint.setPointValue(valueTime, source);

            // Deactivate any existing event.
            returnToNormal(POINT_WRITE_EXCEPTION_EVENT, valueTime.getTime());
        } catch (ModbusTransportException e) {
            // Raise an event.
            raiseEvent(POINT_WRITE_EXCEPTION_EVENT, valueTime.getTime(), true, new LocalizableMessageImpl(
                    "event.exception2", dataPoint.getVo().getName(), e.getMessage()));
            LOG.info("Error setting point value", e);
        } catch (ErrorResponseException e) {
            raiseEvent(POINT_WRITE_EXCEPTION_EVENT, valueTime.getTime(), true, new LocalizableMessageImpl(
                    "event.exception2", dataPoint.getVo().getName(), e.getErrorResponse().getExceptionMessage()));
            LOG.info("Error setting point value", e);
        }
    }

    public static BaseLocator<?> createModbusLocator(ModbusPointLocatorVO vo) {
        return BaseLocator.createLocator(vo.getSlaveId(), vo.getRange(), vo.getOffset(), vo.getModbusDataType(),
                vo.getBit(), vo.getRegisterCount(), Charset.forName(vo.getCharset()));
    }

    public static LocalizableException wrapException(Exception e) {
        if (e instanceof ModbusTransportException) {
            Throwable cause = e.getCause();
            if (cause instanceof TimeoutException) {
                return new LocalizableException("event.modbus.noResponse", ((ModbusTransportException) e).getSlaveId());
            }
            if (cause instanceof ConnectException) {
                return new LocalizableException("common.default", e.getMessage());
            }
        }

        return DataSourceRT.wrapException(e);
    }

    //
    //
    // MessagingConnectionListener interface
    //
    public void receivedException(Exception e) {
        LOG.warn("Modbus exception", e);
        raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                "event.modbus.master", e.getMessage()));
    }

}
