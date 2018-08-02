/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.dao;

import br.org.scadabr.vo.Edge;
import br.org.scadabr.vo.datapoints.DataPointNodeVO;
import br.org.scadabr.vo.datapoints.PointFolderVO;
import br.org.scadabr.web.LazyTreeNode;
import com.serotonin.mango.vo.DataPointVO;
import java.util.Collection;
import javax.inject.Named;
import br.org.scadabr.vo.EdgeConsumer;

/**
 *
 * @author aploese
 */
@Named
public interface DataPointDao {

    @Deprecated // DP no longer belang to DS
    Iterable<DataPointVO> getDataPoints(int dsId);

    void updateDataPoint(DataPointVO dp);

    @Deprecated // DP no longer belang to DS
    void deleteDataPoints(int dataSourceId);

    String generateUniqueXid();

    DataPointVO saveDataPoint(DataPointVO vo);

    void copyPermissions(int id, int id0);

    DataPointVO getDataPoint(int pointId);

    DataPointVO getDataPoint(String xid);

    DataPointVO deleteDataPoint(int id);

    @Deprecated // DP has no extended name anymore
    String getExtendedPointName(int targetPointId);

    Iterable<DataPointVO> getDataPoints(boolean includeRelationalData);

    String getCanonicalPointName(DataPointVO dp);

    Collection<LazyTreeNode> getFoldersAndDpByParentId(int parentId);

    boolean isXidUnique(String xid, int id);

    void addPointToHierarchy(DataPointVO dp, String[] pathToPoint);

    public Iterable<PointFolderVO> getPointFolders();

    public Iterable<DataPointVO> getDataPoints();

    public void iteratePointFolderEdges(EdgeConsumer edgeIterator);

    public void iterateDataPointEdges(EdgeConsumer edgeIterator);

}
