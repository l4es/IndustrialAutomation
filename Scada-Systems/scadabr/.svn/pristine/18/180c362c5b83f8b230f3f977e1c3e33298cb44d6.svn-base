/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.rt.publish;

import br.org.scadabr.dao.PublisherDao;
import com.serotonin.mango.rt.EventManager;
import com.serotonin.mango.rt.publish.PublisherRT;
import com.serotonin.mango.vo.publish.PublishedPointVO;
import com.serotonin.mango.vo.publish.PublisherVO;
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
public class PublisherManager {
    @Inject
    private PublisherDao publisherDao;
    @Inject
    private EventManager eventManager;

    
    /**
     * Store of enabled publishers
     */
    private final List<PublisherRT<?>> runningPublishers = new CopyOnWriteArrayList<>();
    
        //
    // Lifecycle
    synchronized public void initialize(boolean safe) {
        // Start the publishers that are enabled
        for (PublisherVO<? extends PublishedPointVO> vo : publisherDao.getPublishers()) {
            if (vo.isEnabled()) {
                if (safe) {
                    vo.setEnabled(false);
                    publisherDao.savePublisher(vo);
                } else {
                    startPublisher(vo);
                }
            }
        }


    }
    
    synchronized public void terminate() {

        for (PublisherRT<? extends PublishedPointVO> publisher : runningPublishers) {
            stopPublisher(publisher.getId());
        }
        }
            //
    //
    // Publishers
    //
    public PublisherRT<?> getRunningPublisher(int publisherId) {
        for (PublisherRT<?> publisher : runningPublishers) {
            if (publisher.getId() == publisherId) {
                return publisher;
            }
        }
        return null;
    }

    public boolean isPublisherRunning(int publisherId) {
        return getRunningPublisher(publisherId) != null;
    }

    public PublisherVO<? extends PublishedPointVO> getPublisher(int publisherId) {
        return publisherDao.getPublisher(publisherId);
    }

    public void deletePublisher(int publisherId) {
        stopPublisher(publisherId);
        publisherDao.deletePublisher(publisherId);
        eventManager.cancelEventsForPublisher(publisherId);
    }

    public void savePublisher(PublisherVO<? extends PublishedPointVO> vo) {
        // If the data source is running, stop it.
        stopPublisher(vo.getId());

        // In case this is a new publisher, we need to save to the database
        // first so that it has a proper id.
        publisherDao.savePublisher(vo);

        // If the publisher is enabled, start it.
        if (vo.isEnabled()) {
            startPublisher(vo);
        }
    }

    private void startPublisher(PublisherVO<? extends PublishedPointVO> vo) {
        synchronized (runningPublishers) {
            // If the publisher is already running, just quit.
            if (isPublisherRunning(vo.getId())) {
                return;
            }

            // Ensure that the data source is enabled.
            Assert.isTrue(vo.isEnabled());

            // Create and start the runtime version of the publisher.
            PublisherRT<?> publisher = vo.createPublisherRT();
            publisher.initialize();

            // Add it to the list of running publishers.
            runningPublishers.add(publisher);
        }
    }

    private void stopPublisher(int id) {
        synchronized (runningPublishers) {
            PublisherRT<?> publisher = getRunningPublisher(id);
            if (publisher == null) {
                return;
            }

            runningPublishers.remove(publisher);
            publisher.terminate();
            publisher.joinTermination();
        }
    }


}
