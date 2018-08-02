/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.rt.event.schedule;

import br.org.scadabr.dao.ScheduledEventDao;
import com.serotonin.mango.rt.event.SimpleEventDetector;
import com.serotonin.mango.rt.event.detectors.PointEventDetectorRT;
import com.serotonin.mango.rt.event.schedule.ScheduledEventRT;
import com.serotonin.mango.vo.event.ScheduledEventVO;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import javax.inject.Inject;
import javax.inject.Named;

/**
 *
 * @author aploese
 */
@Named
public class ScheduledEventManager {
    @Inject
    private ScheduledEventDao scheduledEventDao;

    /**
     * Store of enabled event detectors.
     */
    private final Map<String, SimpleEventDetector> simpleEventDetectors = new ConcurrentHashMap<>();

    //
    // Lifecycle
    synchronized public void initialize(boolean safe) {
        // Initialize the scheduled events.
        for (ScheduledEventVO se : scheduledEventDao.getScheduledEvents()) {
            if (!se.isDisabled()) {
                if (safe) {
                    se.setDisabled(true);
                    scheduledEventDao.saveScheduledEvent(se);
                } else {
                    startScheduledEvent(se);
                }
            }
        }

    }

    synchronized public void terminate() {
        for (String key : simpleEventDetectors.keySet()) {
            stopSimpleEventDetector(key);
        }
    }

    private void startScheduledEvent(ScheduledEventVO vo) {
        synchronized (simpleEventDetectors) {
            stopSimpleEventDetector(vo.getEventDetectorKey());
            ScheduledEventRT rt = vo.createRuntime();
            simpleEventDetectors.put(vo.getEventDetectorKey(), rt);
            rt.initialize();
        }
    }

    public void stopSimpleEventDetector(String key) {
        synchronized (simpleEventDetectors) {
            SimpleEventDetector rt = simpleEventDetectors.remove(key);
            if (rt != null) {
                rt.terminate();
            }
        }
    }

    //
    //
    // Point event detectors
    //
    public void addPointEventDetector(PointEventDetectorRT ped) {
        synchronized (simpleEventDetectors) {
            ped.initialize();
            simpleEventDetectors.put(ped.getEventDetectorKey(), ped);
        }
    }

    public void removePointEventDetector(String pointEventDetectorKey) {
        synchronized (simpleEventDetectors) {
            SimpleEventDetector sed = simpleEventDetectors
                    .remove(pointEventDetectorKey);
            if (sed != null) {
                sed.terminate();
            }
        }
    }

    public SimpleEventDetector getSimpleEventDetector(String key) {
        return simpleEventDetectors.get(key);
    }

    //
    //
    // Scheduled events
    //
    public void saveScheduledEvent(ScheduledEventVO vo) {
        // If the scheduled event is running, stop it.
        stopSimpleEventDetector(vo.getEventDetectorKey());

        scheduledEventDao.saveScheduledEvent(vo);

        // If the scheduled event is enabled, start it.
        if (!vo.isDisabled()) {
            startScheduledEvent(vo);
        }
    }

}
