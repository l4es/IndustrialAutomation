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
package com.serotonin.mango.rt.publish;

import br.org.scadabr.utils.ImplementMeException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.dao.PublisherDao;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.event.type.EventType;
import com.serotonin.mango.util.timeout.RunClient;
import com.serotonin.mango.vo.publish.PublishedPointVO;
import com.serotonin.mango.vo.publish.PublisherVO;
import br.org.scadabr.timer.TimerTask;
import com.serotonin.mango.rt.EventManager;
import java.io.Serializable;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

/**
 * @author Matthew Lohbihler
 */
@Configurable
abstract public class PublisherRT<T extends PublishedPointVO> implements RunClient {

    public static final int POINT_DISABLED_EVENT = 1;
    public static final int QUEUE_SIZE_WARNING_EVENT = 2;

    private final Object persistentDataLock = new Object();

    private final EventType pointDisabledEventType;
    private final EventType queueSizeWarningEventType;

    private final PublisherVO<T> vo;
    protected final List<PublishedPointRT<T>> pointRTs = new ArrayList<>();
    protected final PublishQueue<T> queue;
    @Autowired
    private RuntimeManager runtimeManager;
    @Autowired
    private EventManager eventManager;
    @Autowired
    private PublisherDao publisherDao;
    private boolean pointDisabledEventActive;
    private volatile Thread jobThread;
    private SendThread sendThread;
    private TimerTask snapshotTask;

    public PublisherRT(PublisherVO<T> vo) {
        this.vo = vo;
        queue = createPublishQueue(vo);

        throw new ImplementMeException();
//        pointDisabledEventType = new PublisherEventType(vo, POINT_DISABLED_EVENT);
//        queueSizeWarningEventType = new PublisherEventType(vo, QUEUE_SIZE_WARNING_EVENT);
    }

    public int getId() {
        return vo.getId();
    }

    protected PublishQueue<T> createPublishQueue(PublisherVO<T> vo) {
        return new PublishQueue<>(this, vo.getCacheWarningSize());
    }

    public PublisherVO<T> getVo() {
        return vo;
    }

    /**
     * This method is usable by subclasses to retrieve serializable data stored
     * using the setPersistentData method.
     */
    public Object getPersistentData(String key) {
        synchronized (persistentDataLock) {
            @SuppressWarnings("unchecked")
            Map<String, Serializable> map = publisherDao.getPersistentData(vo);
            if (map != null) {
                return map.get(key);
            }
            return null;
        }
    }

    /**
     * This method is usable by subclasses to store any type of serializable
     * data. This intention is to provide a mechanism for publisher RTs to be
     * able to persist data between runs. Normally this method would at least be
     * called in the terminate method, but may also be called regularly for
     * failover purposes.
     *
     * @param key
     */
    public void setPersistentData(String key, Serializable persistentData) {
        synchronized (persistentDataLock) {
            @SuppressWarnings("unchecked")
            Map<String, Serializable> map = publisherDao.getPersistentData(vo);
            if (map == null) {
                map = new HashMap<>();
            }

            map.put(key, persistentData);

            publisherDao.savePersistentData(vo, map);
        }
    }

    void publish(T vo, PointValueTime newValue) {
        queue.add(vo, newValue);

        synchronized (sendThread) {
            sendThread.notify();
        }
    }

    public void publish(T vo, List<PointValueTime> newValues) {
        queue.add(vo, newValues);

        synchronized (sendThread) {
            sendThread.notify();
        }
    }

    protected void pointInitialized(@SuppressWarnings("unused") PublishedPointRT<T> rt) {
        checkForDisabledPoints();
    }

    protected void pointTerminated(@SuppressWarnings("unused") PublishedPointRT<T> rt) {
        checkForDisabledPoints();
    }

    synchronized private void checkForDisabledPoints() {
        boolean foundDisabledPoint = false;
        for (PublishedPointRT<T> rt : pointRTs) {
            if (!rt.isPointEnabled()) {
                foundDisabledPoint = true;
                break;
            }
        }

        if (pointDisabledEventActive != foundDisabledPoint) {
            pointDisabledEventActive = foundDisabledPoint;
            if (pointDisabledEventActive) // A published point has been terminated, was never enabled, or no longer exists.
            {
                pointDisabledEventType.fire(createEventContext(), "event.publish.pointMissing");
            } else // Everything is good
            {
                pointDisabledEventType.clearAlarm();
            }
        }
    }

    void fireQueueSizeWarningEvent() {
        queueSizeWarningEventType.fire(createEventContext(), "event.publish.queueSize", vo.getCacheWarningSize());
    }

    void deactivateQueueSizeWarningEvent() {
        queueSizeWarningEventType.clearAlarm();
    }

    protected Map<String, Object> createEventContext() {
        Map<String, Object> context = new HashMap<>();
        context.put("publisher", vo);
        return context;
    }

    //
    //
    // Lifecycle
    //
    abstract public void initialize();

    protected void initialize(SendThread sendThread) {
        this.sendThread = sendThread;
        sendThread.initialize();

        for (T p : vo.getPoints()) {
            pointRTs.add(new PublishedPointRT<>(p, this));
        }

        if (vo.isSendSnapshot()) {
            // Add a schedule to send the snapshot
            long snapshotPeriodMillis = vo.getSnapshotSendPeriodType().getMillis(vo.getSnapshotSendPeriods());
            throw new ImplementMeException(); //WAS: snapshotTask = new TimeoutTask(new FixedRateTrigger(0, snapshotPeriodMillis), this);
        }

        checkForDisabledPoints();
    }

    public void terminate() {
        sendThread.terminate();
        sendThread.joinTermination();

        // Unschedule any job that is running.
        if (snapshotTask != null) {
            snapshotTask.cancel();
        }

        // Terminate the point listeners
        for (PublishedPointRT<T> rt : pointRTs) {
            rt.terminate();
        }

        //TODO notify runtime???
        // Remove any outstanding events.
        eventManager.cancelEventsForPublisher(getId());
    }

    public void joinTermination() {
        Thread localThread = jobThread;
        if (localThread != null) {
            try {
                localThread.join(30000); // 30 seconds
            } catch (InterruptedException e) { /* no op */

            }
            if (jobThread != null) {
                throw new ShouldNeverHappenException("Timeout waiting for publisher to stop: id=" + getId());
            }
        }
    }

    //
    //
    // Scheduled snapshot send stuff
    //
    @Override
    public void run(long fireTime) {
        if (jobThread != null) {
            return;
        }

        jobThread = Thread.currentThread();

        try {
            RuntimeManager rm = runtimeManager;
            synchronized (this) {
                for (PublishedPointRT<T> rt : pointRTs) {
                    if (rt.isPointEnabled()) {
                        DataPointRT dp = rm.getDataPoint(rt.getVo().getDataPointId());
                        if (dp != null) {
                            PointValueTime pvt = dp.getPointValue();
                            if (pvt != null) {
                                publish(rt.getVo(), pvt);
                            }
                        }
                    }
                }
            }
        } finally {
            jobThread = null;
        }
    }
}
