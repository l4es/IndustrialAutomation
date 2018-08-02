/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.serotonin.mango.rt.dataImage;

import br.org.scadabr.DataType;
import br.org.scadabr.timer.cron.DataSourceCronTask;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.vo.VO;
import com.serotonin.mango.rt.dataSource.PointLocatorRT;
import com.serotonin.mango.vo.DoubleDataPointVO;
import java.util.List;

/**
 *
 * @author aploese
 */
public class DoubleDataPointRT 
        extends DataPointRT<DoubleDataPointVO, DoubleValueTime> {

    
    public DoubleDataPointRT(DoubleDataPointVO vo, PointLocatorRT<DoubleValueTime, ?> pointLocator) {
        super(vo, pointLocator);
    }

    @Override
    public void initialize() {
        super.initialize();
 //       initializeIntervalLogging();
    }

    @Override
    public void terminate() {
   //     terminateIntervalLogging();
        super.terminate();
    }
    // Interval logging data.
    private DoubleValueTime intervalValue;
    private long intervalStartTime = -1;
    private List<DoubleValueTime> averagingValues;
    private final Object intervalLoggingLock = new Object();
    private DataSourceCronTask intervalLoggingTask;

    //
    // / Interval logging
    //
    /*
    private void initializeIntervalLogging() {
        synchronized (intervalLoggingLock) {
            if (loggingType != LoggingTypes.INTERVAL) {
                return;
            }

            if (true) {
                throw new ImplementMeException(); //WAS: intervalLoggingTask = new TimeoutTask(this, vo.getIntervalLoggingPeriodType(), vo.getIntervalLoggingPeriod());
            }
            intervalValue = pointValue;
            if (vo.getIntervalLoggingType() == IntervalLoggingTypes.AVERAGE) {
                intervalStartTime = System.currentTimeMillis();
                averagingValues = new ArrayList<>();
            }
        }
    }

    private void terminateIntervalLogging() {
        synchronized (intervalLoggingLock) {
            if (vo.getLoggingType() != LoggingTypes.INTERVAL) {
                return;
            }

            intervalLoggingTask.cancel();
        }
    }
*/
    /**
     * This is the value around which tolerance decisions will be made when
     * determining whether to log numeric values.
     */
    private double toleranceOrigin;

    /*
    @Override
    protected void savePointValue(DoubleValueTime newValue, SetPointSource source, boolean async) {
        // Null values are not very nice, and since they don't have a specific meaning they are hereby ignored.
        if (newValue == null) {
            return;
        }

        // Check the data type of the value against that of the locator, just for fun.
        DataType valueDataType = newValue.getDataType();
        if (valueDataType != DataType.UNKNOWN && valueDataType != vo.getDataType()) // This should never happen, but if it does it can have serious downstream consequences. Also, we need
        // to know how it happened, and the stack trace here provides the best information.
        {
            throw new ShouldNeverHappenException("Data type mismatch between new value and point locator: newValue="
                    + newValue.getDataType() + ", locator=" + vo.getDataType());
        }

        if (newValue.getTimestamp() > System.currentTimeMillis() + systemSettingsDao.getFutureDateLimit()) {
            // Too far future dated. Toss it. But log a message first.
            LOG.log(Level.WARNING, "Future dated value detected: pointId={0}, value={1}, type={2}, ts={3}", new Object[]{vo.getId(), newValue.getValue(), vo.getDataType(), newValue.getTimestamp()});
            return;
        }

        boolean backdated = pointValue != null && newValue.getTimestamp() < pointValue.getTimestamp();

        // Determine whether the new value qualifies for logging.
        boolean logValue;
        // ... or even saving in the cache.
        boolean saveValue = true;
        switch (vo.getLoggingType()) {
            case ON_CHANGE:
                if (pointValue == null) {
                    logValue = true;
                } else if (backdated) // Backdated. Ignore it
                {
                    logValue = false;
                } else {
                    // Get the new double
                    double newd = newValue.getDoubleValue();

                    // See if the new value is outside of the tolerance.
                    double diff = toleranceOrigin - newd;
                    if (diff < 0) {
                        diff = -diff;
                    }

                    if (diff > vo.getTolerance()) {
                        toleranceOrigin = newd;
                        logValue = true;
                    } else {
                        logValue = false;
                    }
                }

                saveValue = logValue;
                break;
            case ALL:
                logValue = true;
                break;
            case ON_TS_CHANGE:
                if (pointValue == null) {
                    logValue = true;
                } else if (backdated) // Backdated. Ignore it
                {
                    logValue = false;
                } else {
                    logValue = newValue.getTimestamp() != pointValue.getTimestamp();
                }

                saveValue = logValue;
                break;
            case INTERVAL:
                if (!backdated) {
                    intervalSave(newValue);
                }
            default:
                logValue = false;
        }

        if (saveValue) {
            if (logValue) {
                if (async) {
                    pointValueDao.savePointValueAsync(newValue, source);
                } else {
                    pointValueDao.savePointValueSync(newValue, source);
                }
            }
        }

        // Ignore historical values.
        if (pointValue == null || newValue.getTimestamp() >= pointValue.getTimestamp()) {
            PointValueTime oldValue = pointValue;
            pointValue = newValue;
            fireEvents(oldValue, newValue, source != null, false);
        } else {
            fireEvents(null, newValue, false, true);
        }
    }
*/
    /*
    private void intervalSave(DoubleValueTime pvt) {
        synchronized (intervalLoggingLock) {
            switch (vo.getIntervalLoggingType()) {
                case MAXIMUM:
                    if (intervalValue == null) {
                        intervalValue = pvt;
                    } else if (pvt != null) {
                        if (intervalValue.getDoubleValue() < pvt.getDoubleValue()) {
                            intervalValue = pvt;
                        }
                    }
                    break;
                case MINIMUM:
                    if (intervalValue == null) {
                        intervalValue = pvt;
                    } else if (pvt != null) {
                        if (intervalValue.getDoubleValue() > pvt.getDoubleValue()) {
                            intervalValue = pvt;
                        }
                    }
                    break;
                case AVERAGE:
                    averagingValues.add(pvt);
            }
        }
    }
*/
    /**
     * Collect the data and store them
     *
     * @param fireTime
     */
    @Override
    public void run(long fireTime) {
        throw new ImplementMeException();
        /*
         synchronized (intervalLoggingLock) {
         DoubleValue value;
         switch (vo.getIntervalLoggingType()) {
         case INSTANT:
         value = PointValueTime.getValue(pointValue);
         break;
         case MAXIMUM:
         case MINIMUM:
         value = PointValueTime.getValue(intervalValue);
         intervalValue = pointValue;
         break;
         case AVERAGE:
         AnalogStatistics stats = new AnalogStatistics(intervalValue, averagingValues, intervalStartTime,
         fireTime);
         value = new DoubleValue(stats.getAverage());

         intervalValue = pointValue;
         averagingValues.clear();
         intervalStartTime = fireTime;
         break;
         default:
         throw new ShouldNeverHappenException("Unknown interval logging type: " + vo.getIntervalLoggingType());
         }

         if (value != null) {
         pointValueDao.savePointValueAsync(new PointValueTime(value, vo.getId(), fireTime), null);
         }
         }
         */
    }

    protected void intervallSave(DoubleValueTime pvt) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }
    
        public void initializeHistorical() {
    //    initializeIntervalLogging();
    }

    public void terminateHistorical() {
    //    terminateIntervalLogging();
    }

    @Override
    protected void savePointValueAsync(DoubleValueTime newValue, VO<?> source) {
                    pointValueDao.savePointValueAsync(newValue, source);
    }

    @Override
    protected void savePointValueSync(DoubleValueTime newValue, VO<?> source) {
                    pointValueDao.savePointValueSync(newValue, source);
    }

    @Override
    public DoubleDataPointVO getVO() {
        final DoubleDataPointVO result = new DoubleDataPointVO();
        fillVO(result);
        return result;
    }

    @Override
    public DataType getDataType() {
        return DataType.DOUBLE;
    }

}
