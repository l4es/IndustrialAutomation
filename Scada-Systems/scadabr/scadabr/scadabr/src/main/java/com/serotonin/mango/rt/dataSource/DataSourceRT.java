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
package com.serotonin.mango.rt.dataSource;

import gnu.io.NoSuchPortException;
import gnu.io.PortInUseException;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.dao.DataSourceDao;
import br.org.scadabr.rt.IDataPointLiveCycleListener;
import br.org.scadabr.rt.RT;
import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import br.org.scadabr.util.ILifecycle;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.utils.i18n.LocalizableException;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.NodeType;
import br.org.scadabr.vo.datasource.PointLocatorVO;
import br.org.scadabr.vo.event.type.DataSourceEventKey;
import br.org.scadabr.rt.AbstractRT;
import br.org.scadabr.vo.VO;
import com.serotonin.mango.rt.EventManager;
import com.serotonin.mango.vo.DataPointVO;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

/**
 * Data sources are things that produce data for consumption of this system.
 * Anything that houses, creates, manages, or otherwise can get data to Mango
 * can be considered a data source. As such, this interface can more precisely
 * be considered a proxy of the real thing.
 *
 * Mango contains multiple objects that carry the name data source. This
 * interface represents those types of objects that execute and perform the
 * actual task of getting information one way or another from the external data
 * source and into the system, and is known as the "run-time" (RT) data source.
 * (Another type is the data source VO, which represents the configuration of a
 * data source RT, a subtle but useful distinction. In particular, a VO is
 * serializable, while an RT is not.)
 *
 * @author Matthew Lohbihler
 * @param <T>
 */
@Configurable
abstract public class DataSourceRT<T extends DataSourceVO<T>>
        extends AbstractRT<T>
        implements ILifecycle, IDataPointLiveCycleListener, RT<T> {

    public static final String ATTR_UNRELIABLE_KEY = "UNRELIABLE";
    /**
     * If a entry is there for a key this key has an statefull event.
     * The type of the enty is dependend on the suclass and key
     */
    protected Map<DataSourceEventKey, Object> activeEvents;
    
    /**
     * Under the expectation that most data sources will run in their own
     * threads, the addedPoints field is used as a cache for points that have
     * been added to the data source, so that at a convenient time for the data
     * source they can be included in the polling.
     *
     * Access should be synchronized with the monitor of addedChangedPoints
     *
     * Note that updated versions of data points that could already be running
     * may be added here, so implementations should always check for existing
     * instances.
     */
    protected final Map<Integer, DataPointRT> enabledDataPointsCache = new HashMap<>();
    protected final Map<Integer, DataPointRT> enabledDataPoints = new HashMap<>();

    /**
     * Under the expectation that most data sources will run in their own
     * threads, the removedPoints field is used as a cache for points that have
     * been removed from the data source, so that at a convenient time for the
     * data source they can be removed from the polling.
     *
     * Access should be synchronized with the monitor of removedPoints
     *
     */
    protected final List<DataPointVO> disabledDataPointsCache = new ArrayList<>();
    protected final Map<Integer, DataPointVO> disabledDataPoints = new HashMap<>();

    protected final List<DataPointVO> deletedDataPointsCache = new ArrayList<>();

    protected final Object dataPointsCacheLock = new Object();
    private final boolean caching;
    private boolean cacheChanged;
    protected boolean enabledDataPointsChanged;
    @Autowired
    private DataSourceDao dataSourceDao;
    @Autowired
    private EventManager eventManager;

    /**
     *
     * @param vo
     * @param doCache whether or not enabling/disabling of datapoints will be
     * cached
     */
    public DataSourceRT(T vo, boolean doCache) {
        super(vo);
        activeEvents = (Map<DataSourceEventKey, Object>)vo.createEventKeyMap();
        caching = doCache;
    }

    /**
     * This method is usable by subclasses to retrieve serializable data stored
     * using the setPersistentData method.
     */
    public Object getPersistentData() {
throw new ImplementMeException();
//return dataSourceDao.getPersistentData(vo);
    }

    /**
     * This method is usable by subclasses to store any type of serializable
     * data. This intention is to provide a mechanism for data source RTs to be
     * able to persist data between runs. Normally this method would at least be
     * called in the terminate method, but may also be called regularly for
     * failover purposes.
     */
    protected void setPersistentData(Object persistentData) {
throw new ImplementMeException();
//        dataSourceDao.savePersistentData(vo, persistentData);
    }

    /*
     * add activated DataPoints to this datasource
     */
    @Override
    public void dataPointEnabled(DataPointRT dataPoint) {
        synchronized (dataPointsCacheLock) {
            if (caching) {
                cacheChanged |= enabledDataPointsCache.put(dataPoint.getId(), dataPoint) == null;
                cacheChanged |= disabledDataPointsCache.remove(dataPoint.getVO());
                cacheChanged |= deletedDataPointsCache.remove(dataPoint.getVO());
            } else {
                enabledDataPoints.put(dataPoint.getId(), dataPoint);
                disabledDataPoints.remove(dataPoint.getId());
            }
        }
    }

    /*
     * remove disabled DataPoints from this datasource
     */
    @Override
    public void dataPointDisabled(DataPointVO dataPoint) {
        synchronized (dataPointsCacheLock) {
            if (caching) {
                cacheChanged |= enabledDataPointsCache.remove(dataPoint.getId()) != null;
                cacheChanged |= disabledDataPointsCache.add(dataPoint);
                cacheChanged |= deletedDataPointsCache.remove(dataPoint);
            } else {
                enabledDataPoints.remove(dataPoint.getId());
                disabledDataPoints.put(dataPoint.getId(), dataPoint);
            }
        }
    }

    /*
     * remove disabled DataPoints from this datasource
     */
    @Override
    public void dataPointDeleted(DataPointVO dataPoint) {
        synchronized (dataPointsCacheLock) {
            if (caching) {
                cacheChanged |= enabledDataPointsCache.remove(dataPoint.getId()) != null;
                cacheChanged |= disabledDataPointsCache.remove(dataPoint);
                cacheChanged |= deletedDataPointsCache.add(dataPoint);
            } else {
                enabledDataPoints.remove(dataPoint.getId());
                disabledDataPoints.remove(dataPoint.getId());
            }
        }
    }

    /**
     * No really need to synchronize with #cacheChanged
     */
    protected void updateChangedPoints() {
        if (!cacheChanged) {
            return;
        }
        cacheChanged = false;
        synchronized (dataPointsCacheLock) {
            enabledDataPoints.putAll(enabledDataPointsCache);
            enabledDataPointsCache.clear();
            enabledDataPointsChanged = true;
            for (DataPointVO dpVo : disabledDataPointsCache) {
                disabledDataPoints.remove(dpVo.getId());
            }
            disabledDataPointsCache.clear();
        }
    }

    public void setPointValue(DataPointRT dataPoint, PointValueTime valueTime, VO<?> source) {
        // no Op
    }

    public void relinquish(DataPointRT dataPoint) {
        throw new ShouldNeverHappenException("not implemented in " + getClass());
    }

    public void forcePointRead(@SuppressWarnings("unused") DataPointRT dataPoint) {
        // No op by default. Override as required.
    }

    protected void raiseAlarm(DataSourceEventKey eventKey, LocalizableMessage message) {
        throw new ImplementMeException();
        /*
        final DataSourceEventType type = vo.getEventType(eventKey);
        final Map<String, Object> context = new HashMap<>();
        context.put("dataSource", vo);

        type.fire(context, "event.ds", vo.getName(), message);
*/
    }
    
    protected void raiseAlarm(DataSourceEventKey eventKey, String i18nKey, Object ... i18nArgs) {
        raiseAlarm(eventKey, new LocalizableMessageImpl(i18nKey, i18nArgs));
    }
    
    protected void raiseAlarm(DataSourceEventKey eventKey, long timestamp, LocalizableMessage message) {
        throw new ImplementMeException();
        /*
        final DataSourceEventType type = vo.getEventType(eventKey);
        final Map<String, Object> context = new HashMap<>();
        context.put("dataSource", vo);

        type.fire(context, timestamp, "event.ds", vo.getName(), message);
*/    }
    
    protected void fireEvent(DataSourceEventKey eventKey, LocalizableMessage message) {
        fireEvent(eventKey, System.currentTimeMillis(), message);
    }

    protected void fireEvent(DataSourceEventKey eventKey, long timestamp, LocalizableMessage message) {
        throw new ImplementMeException();
        /*
        final DataSourceEventType type = vo.getEventType(eventKey);
        final Map<String, Object> context = new HashMap<>();
        context.put("dataSource", vo);
        type.fire(context, timestamp, new LocalizableMessageImpl("event.ds", vo.getName(), message));
*/    }

    protected void fireEvent(DataSourceEventKey eventKey, long timestamp, String i18nKey, Object ... i18nArgs) {
        fireEvent(eventKey, timestamp, new LocalizableMessageImpl(i18nKey, i18nArgs));
    }

    protected void clearAlarm(DataSourceEventKey eventKey) {
        throw new ImplementMeException();
        /*
        final DataSourceEventType type = vo.getEventType(eventKey);
        type.clearAlarm();
*/    }

    protected void clearAlarm(DataSourceEventKey eventKey, long timestamp) {
        throw new ImplementMeException();
        /*
        final DataSourceEventType type = vo.getEventType(eventKey);
        type.clearAlarm(timestamp);
*/    }

    public static LocalizableException wrapSerialException(Exception e, String portId) {
        if (e instanceof NoSuchPortException) {
            return new LocalizableException("event.serial.portOpenError", portId);
        }
        if (e instanceof PortInUseException) {
            return new LocalizableException("event.serial.portInUse", portId);
        }
        return wrapException(e);
    }

    public static LocalizableException wrapException(Exception e) {
        return new LocalizableException("event.exception2", e.getClass().getName(), e.getMessage());
    }

    //
    // /
    // / Lifecycle
    // /
    //
    @Override
    public void initialize() {
        // no op
    }

    @Override
    public void terminate() {
        // Remove any outstanding events.
        //TODO move this to runtimeManger ??? 
        eventManager.cancelEventsForDataSource(id);
    }

    @Override
    public void joinTermination() {
        // no op
    }

    public void pointLocatorDisabled(PointLocatorVO pointLocatorVO) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    public void dataPointLocatorEnabled(PointLocatorRT rt) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }
    
    @Override
    public NodeType getNodeType() {
        return NodeType.DATA_SOURCE;
    }
}
