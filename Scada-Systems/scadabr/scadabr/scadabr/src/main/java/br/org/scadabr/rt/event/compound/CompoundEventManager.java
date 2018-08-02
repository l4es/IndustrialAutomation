/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.rt.event.compound;

import br.org.scadabr.dao.CompoundEventDetectorDao;
import br.org.scadabr.util.LifecycleException;
import br.org.scadabr.utils.i18n.LocalizableException;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import com.serotonin.mango.rt.event.compound.CompoundEventDetectorRT;
import com.serotonin.mango.vo.event.CompoundEventDetectorVO;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import javax.inject.Inject;
import javax.inject.Named;

/**
 *
 * @author aploese
 */
@Named
public class CompoundEventManager {

    @Inject
    private CompoundEventDetectorDao compoundEventDetectorDao;
    /**
     * Store of enabled compound event detectors.
     */
    private final Map<Integer, CompoundEventDetectorRT> compoundEventDetectors = new ConcurrentHashMap<>();

    synchronized public void terminate() {

        for (Integer id : compoundEventDetectors.keySet()) {
            stopCompoundEventDetector(id);
        }
    }

    //
    // Lifecycle

    synchronized public void initialize(boolean safe) {

        // Initialize the compound events.
        for (CompoundEventDetectorVO ced : compoundEventDetectorDao.getCompoundEventDetectors()) {
            if (!ced.isDisabled()) {
                if (safe) {
                    ced.setDisabled(true);
                    compoundEventDetectorDao.saveCompoundEventDetector(ced);
                } else {
                    startCompoundEventDetector(ced);
                }
            }
        }

    }

    //
    //
    // Compound event detectors
    //

    public boolean saveCompoundEventDetector(CompoundEventDetectorVO vo) {
        // If the CED is running, stop it.
        stopCompoundEventDetector(vo.getId());

        compoundEventDetectorDao.saveCompoundEventDetector(vo);

        // If the scheduled event is enabled, start it.
        if (!vo.isDisabled()) {
            return startCompoundEventDetector(vo);
        }

        return true;
    }

    public boolean startCompoundEventDetector(CompoundEventDetectorVO ced) {
        stopCompoundEventDetector(ced.getId());
        CompoundEventDetectorRT rt = ced.createRuntime();
        try {
            rt.initialize();
            compoundEventDetectors.put(ced.getId(), rt);
            return true;
        } catch (LifecycleException e) {
            rt.raiseFailureEvent(new LocalizableMessageImpl(
                    "event.compound.exceptionFailure", ced.getName(),
                    ((LocalizableException) e.getCause())));
        } catch (Exception e) {
            rt.raiseFailureEvent(new LocalizableMessageImpl(
                    "event.compound.exceptionFailure", ced.getName(), e
                    .getMessage()));
        }
        return false;
    }

    public void stopCompoundEventDetector(int compoundEventDetectorId) {
        CompoundEventDetectorRT rt = compoundEventDetectors
                .remove(compoundEventDetectorId);
        if (rt != null) {
            rt.terminate();
        }
    }

}
