/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.dao;

import com.serotonin.mango.vo.publish.PublishedPointVO;
import com.serotonin.mango.vo.publish.PublisherVO;
import java.io.Serializable;
import java.util.Map;

/**
 *
 * @author aploese
 */
public interface PublisherDao {
    
    Map<String, Serializable> getPersistentData(PublisherVO vo);

    void savePersistentData(PublisherVO vo, Map<String, Serializable> data);

    Iterable<PublisherVO<? extends PublishedPointVO>> getPublishers();

    void savePublisher(PublisherVO<? extends PublishedPointVO> vo);

    PublisherVO<? extends PublishedPointVO> getPublisher(int publisherId);

    void deletePublisher(int publisherId);

    boolean isXidUnique(String xid, int id);
    
}
