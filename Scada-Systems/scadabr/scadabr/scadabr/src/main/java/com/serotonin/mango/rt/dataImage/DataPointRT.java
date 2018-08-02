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
package com.serotonin.mango.rt.dataImage;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import br.org.scadabr.dao.PointValueDao;
import br.org.scadabr.dao.SystemSettingsDao;
import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.rt.DataPointNodeRT;
import br.org.scadabr.rt.PointFolderRT;
import br.org.scadabr.rt.RT;
import br.org.scadabr.rt.SchedulerPool;
import br.org.scadabr.rt.event.schedule.ScheduledEventManager;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.rt.dataSource.PointLocatorRT;
import com.serotonin.mango.rt.event.detectors.PointEventDetectorRT;
import com.serotonin.mango.util.timeout.RunClient;
import com.serotonin.mango.vo.DataPointVO;
import br.org.scadabr.timer.cron.EventRunnable;
import br.org.scadabr.util.ILifecycle;
import br.org.scadabr.vo.EdgeType;
import br.org.scadabr.vo.LoggingTypes;
import br.org.scadabr.vo.NodeType;
import br.org.scadabr.rt.AbstractRT;
import br.org.scadabr.vo.VO;
import com.serotonin.mango.rt.EventManager;
import java.util.logging.Logger;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;
import br.org.scadabr.vo.EdgeConsumer;

//TODO split tist to datatypes Double ....
/**
 *
 * @param <T>
 * @param <P>
 */
@Configurable
public abstract class DataPointRT<T extends DataPointVO<T, P>, P extends PointValueTime> 
        extends AbstractRT<T> 
        implements DataPointNodeRT<T>, IDataPoint<P>, ILifecycle, RunClient {

    protected static final Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_CORE);
    private static final PvtTimeComparator pvtTimeComparator = new PvtTimeComparator();

    protected final PointLocatorRT pointLocator;

    // Runtime data.
    protected volatile P pointValue;
    @Autowired
    protected RuntimeManager runtimeManager;
    @Autowired
    protected ScheduledEventManager scheduledEventManager;
    @Autowired
    protected EventManager eventManager;
    @Autowired
    protected PointValueDao pointValueDao;
    @Autowired
    protected SystemSettingsDao systemSettingsDao;
    @Autowired
    private SchedulerPool schedulerPool;
    private List<PointEventDetectorRT> detectors;
    private final Map<String, Object> attributes = new HashMap<>();
    private PointFolderRT parentFolder;
    protected LoggingTypes loggingType;
    
   
    
    
    public DataPointRT(T vo, PointLocatorRT pointLocator) {
        super(vo);
        loggingType = vo.getLoggingType();
        this.pointLocator = pointLocator;
    }

    /**
     * This method should only be called by the data source. Other types of
     * point setting should include a set point source object so that the
     * annotation can be logged.
     *
     * @param newValue
     */
    @Override
    public void updatePointValueAsync(P newValue) {
        savePointValueAsync(newValue, null);
    }

    @Override
    public void updatePointValueSync(P newValue) {
        savePointValueSync(newValue, null);
    }

    /**
     * Use this method to update a data point for reasons other than just data
     * source update.
     *
     * @param newValue the value to set
     * @param source the source of the set. This can be a user object if the
     * point was set from the UI, or could be a program run by schedule or on
     * event.
     */
    @Override
    public void setPointValueSync(P newValue, VO<?> source) {
        savePointValueSync(newValue, source);
    }

    @Override
    public void setPointValueAsync(P newValue) {
        savePointValueAsync(newValue, null);
    }

    protected abstract void savePointValueAsync(P newValue, VO<?> source);

    protected abstract void savePointValueSync(P newValue, VO<?> source);

    //
    // /
    // / Properties
    // /
    //
    @Override
    public P getPointValue() {
        return pointValue;
    }

    @SuppressWarnings("unchecked")
    public <T extends PointLocatorRT> T getPointLocator() {
        return (T) pointLocator;
    }

    public Map<String, Object> getAttributes() {
        return attributes;
    }

    public void setAttribute(String key, Object value) {
        attributes.put(key, value);
    }

    public Object getAttribute(String key) {
        return attributes.get(key);
    }

    @Override
    public int hashCode() {
        final int PRIME = 31;
        int result = 1;
        result = PRIME * result + getId();
        return result;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final DataPointRT other = (DataPointRT) obj;
        return getId() == other.getId();
    }

    @Override
    public String toString() {
        return "DataPointRT(id=" + getId() + ", name=" + name + ")";
    }

    //
    // /
    // / Listeners
    // /
    //
    protected void fireEvents(PointValueTime oldValue, PointValueTime newValue, boolean set, boolean backdate) {
        DataPointListener l = runtimeManager.getDataPointListeners(id);
        if (l != null) {
            schedulerPool.execute(new EventNotifyWorkItem(l, oldValue, newValue, set, backdate));
        }
    }

    class EventNotifyWorkItem implements EventRunnable {

        private final DataPointListener listener;
        private final PointValueTime oldValue;
        private final PointValueTime newValue;
        private final boolean set;
        private final boolean backdate;

        EventNotifyWorkItem(DataPointListener listener, PointValueTime oldValue, PointValueTime newValue, boolean set,
                boolean backdate) {
            this.listener = listener;
            this.oldValue = oldValue;
            this.newValue = newValue;
            this.set = set;
            this.backdate = backdate;
        }

        @Override
        public void run() {
            if (backdate) {
                listener.pointBackdated(newValue);
            } else {
                // Always fire this.
                listener.pointUpdated(newValue);

                // Fire if the point has changed.
                if (!PointValueTime.equalValues(oldValue, newValue)) {
                    listener.pointChanged(oldValue, newValue);
                }

                // Fire if the point was set.
                if (set) {
                    listener.pointSet(oldValue, newValue);
                }
            }
        }
        /*
         @Override
         public int getPriority() {
         return WorkItem.PRIORITY_MEDIUM;
         }
         */
    }

    //
    //
    // Lifecycle
    //
    @Override
    public void initialize() {
/*TODO
        // Get the latest value for the point from the database.
        pointValue = pointValueDao.getLatestPointValue(getVO());

        // Add point event listeners
        for (DoublePointEventDetectorVO ped : getVO().getEventDetectors()) {
            if (detectors == null) {
                detectors = new ArrayList<>();
            }

            PointEventDetectorRT pedRT = ped.createRuntime();
            detectors.add(pedRT);
            scheduledEventManager.addPointEventDetector(pedRT);
            runtimeManager.addDataPointListener(id, pedRT);
        }
*/
    }

    @Override
    public void terminate() {

        //TODO notify runtimeManger and lat them handle this???
        if (detectors != null) {
            for (PointEventDetectorRT pedRT : detectors) {
                runtimeManager.removeDataPointListener(id, pedRT);
                scheduledEventManager.removePointEventDetector(pedRT.getEventDetectorKey());
            }
        }
        //TODO notify runtimeManger and lat them handle this???
        eventManager.cancelEventsForDataPoint(id);
    }

    @Override
    public void joinTermination() {
        // no op
    }

    @Override
    public void updatePointValue(P newValue) {
        savePointValueAsync(pointValue, null);
    }

    @Override
    public void setPointValue(P newValue) {
        savePointValueAsync(pointValue, null);
    }

    @Override
    public PointFolderRT getParent() {
        return parentFolder;
    }

    @Override
    public void setParent(PointFolderRT parent) {
        this.parentFolder = parent;
    }

    @Override
    public void wireEdgeAsSrc(RT<?> dest, EdgeType edgeType) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void wireEdgeAsDest(RT<?> src, EdgeType edgeType) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void iterateEdgesAsSrc(EdgeConsumer edgeIterator, EdgeType... edgeTypes) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void iterateEdgesAsDest(EdgeConsumer edgeIterator, EdgeType... edgeTypes) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public NodeType getNodeType() {
        return NodeType.DATA_POINT;
    }

}
