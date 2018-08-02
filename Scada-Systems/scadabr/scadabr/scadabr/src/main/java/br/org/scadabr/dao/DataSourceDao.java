/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.dao;

import br.org.scadabr.vo.datasource.PointLocatorFolderVO;
import br.org.scadabr.vo.datasource.PointLocatorVO;
import com.serotonin.mango.rt.event.type.EventType;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import javax.inject.Named;

/**
 *
 * @author aploese
 */
@Named
public interface DataSourceDao {

    Object getPersistentData(DataSourceVO dsVO);

    void savePersistentData(DataSourceVO dsVO, Object persistentData);

    DataSourceVO getDataSource(int id);

    Iterable<DataSourceVO<?>> getDataSources();

    void saveDataSource(DataSourceVO<?> config);

    void deleteDataSource(int dsId);

    EventType getEventType(int refId1, int refId2);

    PointLocatorVO getPointLocator(int id);

    @Deprecated
    Iterable<PointLocatorVO> getPointLocatorsByParent(int dataSourceId, int parentFolderId);

    Iterable<PointLocatorVO> getPointLocatorsByParent(int parentFolderId);

    void savePointLocator(PointLocatorVO<?> plVo);

    void deletePointLocator(int plId);

    PointLocatorFolderVO getPointLocatorFolder(int id);

    PointLocatorFolderVO getRootPointLocatorFolder(int dataSourceId);

    @Deprecated
    Iterable<PointLocatorFolderVO> getPointLocatorsFolderByParent(int dataSourceId, int parentFolderId);

    Iterable<PointLocatorFolderVO> getPointLocatorsFolderByParent(int parentFolderId);

    void savePointLocatorFolder(PointLocatorFolderVO plfVo);

    void deletePointLocatorFolder(int plfId);
   
    boolean isXidUnique(DataSourceVO<?> dsVo);
    
    boolean isXidUnique(String xid);

}
