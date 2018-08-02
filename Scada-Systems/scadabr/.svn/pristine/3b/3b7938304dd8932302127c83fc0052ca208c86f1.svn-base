/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.rt.link;

import br.org.scadabr.dao.PointLinkDao;
import com.serotonin.mango.rt.link.PointLinkRT;
import com.serotonin.mango.vo.link.PointLinkVO;
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
public class PointLinkManager {

    @Inject
    private PointLinkDao pointLinkDao;

    /**
     * Store of enabled point links
     */
    private final List<PointLinkRT> pointLinks = new CopyOnWriteArrayList<>();

    synchronized public void terminate() {
        for (PointLinkRT pointLink : pointLinks) {
            stopPointLink(pointLink.getId());
        }

    }

    //
    //
    // Point links
    //

    private PointLinkRT getRunningPointLink(int pointLinkId) {
        for (PointLinkRT pointLink : pointLinks) {
            if (pointLink.getId() == pointLinkId) {
                return pointLink;
            }
        }
        return null;
    }

    public boolean isPointLinkRunning(int pointLinkId) {
        return getRunningPointLink(pointLinkId) != null;
    }

    public void deletePointLink(int pointLinkId) {
        stopPointLink(pointLinkId);
        pointLinkDao.deletePointLink(pointLinkId);
    }

    public void savePointLink(PointLinkVO vo) {
        // If the point link is running, stop it.
        stopPointLink(vo.getId());

        pointLinkDao.savePointLink(vo);

        // If the point link is enabled, start it.
        if (!vo.isDisabled()) {
            startPointLink(vo);
        }
    }

    private void startPointLink(PointLinkVO vo) {
        synchronized (pointLinks) {
            // If the point link is already running, just quit.
            if (isPointLinkRunning(vo.getId())) {
                return;
            }

            // Ensure that the point link is enabled.
            Assert.isTrue(!vo.isDisabled());

            // Create and start the runtime version of the point link.
            PointLinkRT pointLink = new PointLinkRT(vo);
            pointLink.initialize();

            // Add it to the list of running point links.
            pointLinks.add(pointLink);
        }
    }

    private void stopPointLink(int id) {
        synchronized (pointLinks) {
            PointLinkRT pointLink = getRunningPointLink(id);
            if (pointLink == null) {
                return;
            }

            pointLinks.remove(pointLink);
            pointLink.terminate();
        }
    }

    //
    // Lifecycle
    synchronized public void initialize(boolean safe) {
        // Set up point links.
        for (PointLinkVO vo : pointLinkDao.getPointLinks()) {
            if (!vo.isDisabled()) {
                if (safe) {
                    vo.setDisabled(true);
                    pointLinkDao.savePointLink(vo);
                } else {
                    startPointLink(vo);
                }
            }
        }

    }

}
