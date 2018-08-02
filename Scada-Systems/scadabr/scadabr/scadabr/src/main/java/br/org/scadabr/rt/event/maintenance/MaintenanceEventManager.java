/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.rt.event.maintenance;

import br.org.scadabr.dao.MaintenanceEventDao;
import com.serotonin.mango.rt.event.maintenance.MaintenanceEventRT;
import com.serotonin.mango.vo.event.MaintenanceEventVO;
import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;
import javax.inject.Inject;
import javax.inject.Named;
import org.springframework.util.Assert;

/**
 *
 * @author aploese
 */
@Named
public class MaintenanceEventManager {
    @Inject
    private MaintenanceEventDao maintenanceEventDao;
    /**
     * Store of maintenance events
     */
    private final List<MaintenanceEventRT> maintenanceEvents = new CopyOnWriteArrayList<>();


    synchronized public void terminate() {
        for (MaintenanceEventRT me : maintenanceEvents) {
            stopMaintenanceEvent(me.getVo().getId());
        }

        }
        
    //
    // Lifecycle

    synchronized public void initialize(boolean safe) {
        // Start the maintenance events that are enabled
        for (MaintenanceEventVO vo : maintenanceEventDao.getMaintenanceEvents()) {
            if (!vo.isDisabled()) {
                if (safe) {
                    vo.setDisabled(true);
                    maintenanceEventDao.saveMaintenanceEvent(vo);
                } else {
                    startMaintenanceEvent(vo);
                }
            }
        }
    }
    
    //
    //
    // Maintenance events
    //
    public MaintenanceEventRT getRunningMaintenanceEvent(int id) {
        for (MaintenanceEventRT rt : maintenanceEvents) {
            if (rt.getVo().getId() == id) {
                return rt;
            }
        }
        return null;
    }

    public boolean isActiveMaintenanceEvent(int dataSourceId) {
        for (MaintenanceEventRT rt : maintenanceEvents) {
            if (rt.getVo().getDataSourceId() == dataSourceId
                    && rt.isEventActive()) {
                return true;
            }
        }
        return false;
    }

    public boolean isMaintenanceEventRunning(int id) {
        return getRunningMaintenanceEvent(id) != null;
    }

    public void deleteMaintenanceEvent(int id) {
        stopMaintenanceEvent(id);
        maintenanceEventDao.deleteMaintenanceEvent(id);
    }

    public void saveMaintenanceEvent(MaintenanceEventVO vo) {
        // If the maintenance event is running, stop it.
        stopMaintenanceEvent(vo.getId());

        maintenanceEventDao.saveMaintenanceEvent(vo);

        // If the maintenance event is enabled, start it.
        if (!vo.isDisabled()) {
            startMaintenanceEvent(vo);
        }
    }

    private void startMaintenanceEvent(MaintenanceEventVO vo) {
        synchronized (maintenanceEvents) {
            // If the maintenance event is already running, just quit.
            if (isMaintenanceEventRunning(vo.getId())) {
                return;
            }

            // Ensure that the maintenance event is enabled.
            Assert.isTrue(!vo.isDisabled());

            // Create and start the runtime version of the maintenance event.
            MaintenanceEventRT rt = new MaintenanceEventRT(vo);
            rt.initialize();

            // Add it to the list of running maintenance events.
            maintenanceEvents.add(rt);
        }
    }

    private void stopMaintenanceEvent(int id) {
        synchronized (maintenanceEvents) {
            MaintenanceEventRT rt = getRunningMaintenanceEvent(id);
            if (rt == null) {
                return;
            }

            maintenanceEvents.remove(rt);
            rt.terminate();
        }
    }

}
