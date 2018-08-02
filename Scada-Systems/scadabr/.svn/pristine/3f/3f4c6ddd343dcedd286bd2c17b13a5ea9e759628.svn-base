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
package com.serotonin.mango.rt;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.dao.DataSourceDao;
import br.org.scadabr.dao.NodeEdgeDao;
import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.rt.PointFolderRT;
import br.org.scadabr.rt.RT;
import br.org.scadabr.rt.UserRT;
import br.org.scadabr.rt.WrongEdgeTypeException;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.vo.Edge;
import br.org.scadabr.vo.EdgeType;
import br.org.scadabr.vo.NodeType;
import br.org.scadabr.vo.VO;
import br.org.scadabr.vo.datapoints.DataPointNodeVO;
import br.org.scadabr.vo.datapoints.PointFolderVO;
import br.org.scadabr.vo.datasource.PointLocatorVO;
import com.serotonin.mango.rt.dataImage.DataPointEventMulticaster;
import com.serotonin.mango.rt.dataImage.DataPointListener;
import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataSource.DataSourceRT;
import com.serotonin.mango.rt.dataSource.PointLocatorRT;
import com.serotonin.mango.rt.dataSource.meta.MetaDataSourceRT;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.rt.dataSource.PollingDataSource;
import com.serotonin.mango.web.UserSessionContextBean;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Optional;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.stream.Stream;
import javax.inject.Inject;
import javax.inject.Named;

@Named
public class RuntimeManager {

    private static final Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_CORE);

    private final Map<Integer, RT<?>> nodes = new HashMap<>();
    private final Map<Integer, DataSourceRT<?>> dataSources = new HashMap<>();

    private final List<DataSourceRT> runningDataSources = new CopyOnWriteArrayList<>();
    private final Set<UserSessionContextBean> userSessions = new HashSet<>();

    /**
     * Provides a quick lookup map of the running data points.
     */
    private final Map<Integer, DataPointRT> runningDataPoints = new HashMap<>();
    private final Map<Integer, PointLocatorRT> pointLocators = new HashMap<>();

    /**
     * The list of point listeners, kept here such that listeners can be
     * notified of point initializations (i.e. a listener can register itself
     * before the point is enabled).
     */
    private final Map<Integer, DataPointListener> dataPointListeners = new ConcurrentHashMap<>();

    private boolean started = false;

    @Inject
    private NodeEdgeDao<? extends VO<?>> nodeEdgeDao;
    @Inject
    private DataSourceDao dataSourceDao;
    @Inject
    private EventManager eventManager;

    public RuntimeManager() {

    }

    //
    // Lifecycle
    synchronized public void initialize() {
        if (started) {
            throw new ShouldNeverHappenException(
                    "RuntimeManager already started");
        }

        // Set the started indicator to true.
        started = true;

        nodeEdgeDao.forEachNode((node) -> {
            nodes.put(node.getId(), node.createRT());
        });

        nodeEdgeDao.forEachEdge((srcId, destId, edgeType) -> {
            try {
                nodes.get(srcId).wireEdgeAsSrc(nodes.get(destId), edgeType);
            } catch (WrongEdgeTypeException ex) {
                //TODO Event???
                LOG.log(Level.SEVERE, "Wire exception", ex);
            }
        });

        // Initialize data sources that are enabled.
        List<DataSourceVO<?>> pollingRound = new ArrayList<>();
        for (DataSourceVO<?> config : dataSourceDao.getDataSources()) {
            if (initializeDataSource(config)) {
                pollingRound.add(config);
            }
        }

        // Tell the data sources to start polling. Delaying the polling start
        // gives the data points a chance to
        // initialize such that point listeners in meta points and set point
        // handlers can run properly.
        for (DataSourceVO<?> config : pollingRound) {
            startDataSourcePolling(config);
        }
    }

    synchronized public void terminate() {
        if (!started) {
            throw new ShouldNeverHappenException(
                    "RuntimeManager not yet started");
        }

        started = false;
        // First stop meta data sources.
        for (DataSourceRT dataSource : runningDataSources) {
            if (dataSource instanceof MetaDataSourceRT) {
                stopDataSource(dataSource.getId());
            }
        }
        // Then stop everything else.
        for (DataSourceRT dataSource : runningDataSources) {
            stopDataSource(dataSource.getId());
        }

    }

    public void joinTermination() {
        for (DataSourceRT dataSource : runningDataSources) {
            try {
                dataSource.joinTermination();
            } catch (ShouldNeverHappenException e) {
                LOG.log(Level.SEVERE, "Error stopping data source " + dataSource.getId(), e);
            }
        }
    }

    //
    //
    // Data sources
    //
    public DataSourceRT getRunningDataSource(int dataSourceId) {
        for (DataSourceRT dataSource : runningDataSources) {
            if (dataSource.getId() == dataSourceId) {
                return dataSource;
            }
        }
        return null;
    }

    public boolean isDataSourceRunning(int dataSourceId) {
        return getRunningDataSource(dataSourceId) != null;
    }

    public Iterable<DataSourceVO<?>> getDataSources() {
        return dataSourceDao.getDataSources();
    }

    public DataSourceVO<?> getDataSourceVO(int dataSourceId) {
        return dataSources.get(dataSourceId).getVO();
    }

    public DataSourceRT<?> getDataSourceRT(int dataSourceId) {
        return dataSources.get(dataSourceId);
    }

    public void deleteDataSource(int dataSourceId) {
        stopDataSource(dataSourceId);
        eventManager.cancelEventsForDataSource(dataSourceId);
        dataSourceDao.deleteDataSource(dataSourceId);
    }

    public void saveDataSource(DataSourceVO<?> vo) {
        // If the data source is running, stop it.
        stopDataSource(vo.getId());

        // In case this is a new data source, we need to save to the database
        // first so that it has a proper id.
        dataSourceDao.saveDataSource(vo);

        // If the data source is enabled, start it.
        if (vo.isEnabled()) {
            if (initializeDataSource(vo)) {
                startDataSourcePolling(vo);
            }
        }
    }

    private boolean initializeDataSource(DataSourceVO<?> vo) {
        synchronized (runningDataSources) {
            // If the data source is already running, just quit.
            if (isDataSourceRunning(vo.getId())) {
                return false;
            }

            // Create the runtime version of the data source.
            final DataSourceRT dataSource = vo.createRT();
            dataSources.put(vo.getId(), dataSource);
            if (vo.isEnabled()) {
                // Add it to the list of running data sources.
                runningDataSources.add(dataSource);

                // Add the enabled points to the data source.
                // dataPointDao.loadPointFolders(vo);
                // startPointLocator(vo)
                /*    if (pointLocatorVO.isEnabled()) {
                    startPointLocator(pointLocatorVO);
                } else {
                    dataSource.pointLocatorDisabled(pointLocatorVO);
                }
                 */
                // Initialize and thus start the runtime version of the data source.
                dataSource.initialize();

                LOG.info("Data source '" + vo.getName() + "' initialized");

                return true;
            } else {
                return false;
            }
        }
    }

    private void startDataSourcePolling(DataSourceVO<?> vo) {
        DataSourceRT dataSource = getRunningDataSource(vo.getId());
        if (dataSource != null) {
            if (dataSource instanceof PollingDataSource) {
                ((PollingDataSource) dataSource).beginPolling();
            }
        }
    }

    public void stopDataSource(int id) {
        synchronized (runningDataSources) {
            DataSourceRT dataSource = getRunningDataSource(id);
            if (dataSource == null) {
                return;
            }
            dataSource.terminate();
            dataSource.joinTermination();
            LOG.info("Data source '" + dataSource.getName() + "' stopped");
        }
    }

    //
    //
    // Data points
    //
    public DataPointVO saveDataPoint(DataPointVO vo) {
        DataPointRT rt = getDataPoint(vo.getId());
        if (rt.getParent() != null) {
            rt.getParent().validateChildName(vo.getName());
        }
        vo = nodeEdgeDao.saveNode(vo);
        rt.patch(vo);
        return vo;
    }

    private void deleteDataPoint(DataPointRT point) {
        if (point.getParent() != null) {
            nodeEdgeDao.deleteEdge(new Edge(point.getParent().getId(), point.getId(), EdgeType.TREE_PARENT_TO_CHILD));
        }
        nodeEdgeDao.deleteNode(point.getId());
        if (point.getParent() != null) {
            if (!point.getParent().removeChild(point)) {
                throw new ShouldNeverHappenException("Can't remove child");
            }
        }
        if (nodes.remove(point.getId()) != point) {
            throw new ShouldNeverHappenException("Can't remove node");
        }
    }

    private <T extends PointValueTime, VO extends PointLocatorVO<T>> PointLocatorRT<T, VO> startPointLocator(VO vo) {
        synchronized (pointLocators) {
            //      Assert.isTrue(vo.isEnabled());

            // Only add the data point if its data source is enabled.
            DataSourceRT ds = getRunningDataSource(vo.getDataSourceId());
            if (ds != null) {
                // Change the VO into a data point implementation.
                PointLocatorRT rt = vo.createRuntime();

                // Add/update it in the data image.
                pointLocators.put(vo.getId(), rt);

                DataPointListener l = getDataPointListeners(vo.getId());
                if (l != null) {
                    l.pointInitialized();
                }

                // Add/update it in the data source.
                ds.dataPointLocatorEnabled(rt);
                return rt;
            }
        }
        return null;
    }

    /**
     * add a disabled datapoint to a running datasource
     *
     * @param vo
     * @return
     */
    private void addDisabledDataPointToRT(DataPointVO vo) {
        synchronized (runningDataPoints) {

            // Only add the data point if its data source is enabled.
            throw new ImplementMeException();
            /* TODO DataSourceRT ds = getRunningDataSource(vo.getDataSourceId());
            if (ds != null) {
                ds.dataPointDisabled(vo);
            }
             */
        }
    }

    private void stopDataPoint(DataPointVO dpVo) {
        synchronized (runningDataPoints) {
            // Remove this point from the data image if it is there. If not,
            // just quit.
            DataPointRT p = runningDataPoints.remove(dpVo.getId());

            // Remove it from the data source, and terminate it.
            if (p != null) {
                throw new ImplementMeException();
                /* TODO
                getRunningDataSource(p.getDataSourceId()).dataPointDisabled(p.getVo());
                DataPointListener l = getDataPointListeners(dpVo.getId());
                if (l != null) {
                    l.pointTerminated();
                }
                p.terminate();
                 */
            }
        }
    }

    public boolean isDataPointRunning(int dataPointId) {
        return runningDataPoints.get(dataPointId) != null;
    }

    public DataPointRT getDataPoint(int id) {
        return getNode(id, DataPointRT.class);
    }

    public void addDataPointListener(int dataPointId, DataPointListener l) {
        DataPointListener listeners = dataPointListeners.get(dataPointId);
        dataPointListeners.put(dataPointId,
                DataPointEventMulticaster.add(listeners, l));
    }

    public void removeDataPointListener(int dataPointId, DataPointListener l) {
        DataPointListener listeners = DataPointEventMulticaster.remove(
                dataPointListeners.get(dataPointId), l);
        if (listeners == null) {
            dataPointListeners.remove(dataPointId);
        } else {
            dataPointListeners.put(dataPointId, listeners);
        }
    }

    public DataPointListener getDataPointListeners(int dataPointId) {
        return dataPointListeners.get(dataPointId);
    }

    public void setDataPointValue(PointValueTime valueTime, VO<?> source) {
        DataPointRT dataPoint = runningDataPoints.get(valueTime.getDataPointId());
        if (dataPoint == null) {
            throw new RTException("Point is not enabled");
        }

        if (!dataPoint.getPointLocator().isSettable()) {
            throw new RTException("Point is not settable");
        }

        throw new ImplementMeException();
        /*
        // Tell the data source to set the value of the point.
        DataSourceRT ds = getRunningDataSource(dataPoint.getDataSourceId());
        // The data source may have been disabled. Just make sure.
        if (ds != null) {
            ds.setPointValue(dataPoint, valueTime, source);
        }
         */
    }

    public void relinquish(int dataPointId) {
        DataPointRT dataPoint = runningDataPoints.get(dataPointId);
        if (dataPoint == null) {
            throw new RTException("Point is not enabled");
        }

        if (!dataPoint.getPointLocator().isSettable()) {
            throw new RTException("Point is not settable");
        }
        if (!dataPoint.getPointLocator().isRelinquishable()) {
            throw new RTException("Point is not relinquishable");
        }

        throw new ImplementMeException();
        /*
        // Tell the data source to relinquish value of the point.
        DataSourceRT ds = getRunningDataSource(dataPoint.getDataSourceId());
        // The data source may have been disabled. Just make sure.
        if (ds != null) {
            ds.relinquish(dataPoint);
        }
         */
    }

    public void forcePointRead(int dataPointId) {
        DataPointRT dataPoint = runningDataPoints.get(dataPointId);
        if (dataPoint == null) {
            throw new RTException("Point is not enabled");
        }

        throw new ImplementMeException();
        /*
        // Tell the data source to read the point value;
        DataSourceRT ds = getRunningDataSource(dataPoint.getDataSourceId());
        if (ds != null) // The data source may have been disabled. Just make sure.
        {
            ds.forcePointRead(dataPoint);
        }
         */
    }

    public void UserSessionStarts(UserSessionContextBean us) {
        userSessions.add(us);
    }

    public void UserSessionEnds(UserSessionContextBean us) {
        userSessions.remove(us);
    }

    public Set<UserSessionContextBean> getUserSessionContextBeans() {
        return userSessions;
    }

    public RT<?> getNode(int id) {
        final RT<?> result = nodes.get(id);
        if (result == null) {
            throw new NodeNotFoundException(id);
        }
        return result;
    }

    public <T extends RT<?>> T getNode(int id, Class<T> clazz) {
        final T result = (T) nodes.get(id);
        if (result == null) {
            throw new NodeNotFoundException(id);
        }
        return result;
    }

    public <T extends VO<T>> T getVo(int id, Class<T> clazz) {
        final RT<T> result = (RT<T>) nodes.get(id);
        if (result == null) {
            throw new NodeNotFoundException(id);
        }
        return result.getVO();
    }

    public PointFolderRT getPointFolder(int id) {
        return getNode(id, PointFolderRT.class);
    }

    //TODO make this generic ??
    private void deletePointFolder(PointFolderRT pf) {
        if (pf.getChildFolderSize() > 0) {
            throw new UnsupportedOperationException("Deleting with Children Not supported yet.");
        }
        if (pf.getParent() != null) {
            nodeEdgeDao.deleteEdge(new Edge(pf.getParent().getId(), pf.getId(), EdgeType.TREE_PARENT_TO_CHILD));
        }
        nodeEdgeDao.deleteNode(pf.getId());
        if (pf.getParent() != null) {
            if (!pf.getParent().removeChild(pf)) {
                throw new ShouldNeverHappenException("Can't remove child");
            }
        }
        if (nodes.remove(pf.getId()) != pf) {
            throw new ShouldNeverHappenException("Can't remove node");
        }
    }

    public <U extends VO<U>> U addNode(U node) {
        node = nodeEdgeDao.saveNode(node);
        nodes.put(node.getId(), node.createRT());
        return node;
    }

    /**
     * Add a new dest node on the edge to an existing src node
     *
     * @param sourceNodeId the id of the source node
     * @param edgeType the edge type
     * @param newDestNode the new dest node to add
     */
    //TODO make generic and Rollback if errer... and other like given destId ..
    public void addNewNodeOnEdge(int sourceNodeId, EdgeType edgeType, VO newDestNode) {
        switch (edgeType) {

            case TREE_PARENT_TO_CHILD:
                PointFolderRT src = (PointFolderRT) nodes.get(sourceNodeId);
                if (src != null) {
                    src.validateChildName(newDestNode.getName());
                }
                newDestNode = nodeEdgeDao.saveNode(newDestNode);
                nodes.put(newDestNode.getId(), newDestNode.createRT());
                nodeEdgeDao.saveEdge(sourceNodeId, newDestNode.getId(), EdgeType.TREE_PARENT_TO_CHILD);
                try {
                    nodes.get(sourceNodeId).wireEdgeAsSrc(nodes.get(newDestNode.getId()), EdgeType.TREE_PARENT_TO_CHILD);
                } catch (WrongEdgeTypeException ex) {
                    LOG.log(Level.SEVERE, "Can't wire in RuntimeManager.addPointFolder", ex);
                    throw new RuntimeException(ex);
                }
                break;
            default:
                throw new ImplementMeException();
        }
    }

        //TODO use Java8 stream
    @Deprecated
    public List<DataPointNodeVO<?>> getRootPointFolders() {
        List<DataPointNodeVO<?>> result = new LinkedList<>();
        nodes.forEach((index, node) -> {
            switch (node.getNodeType()) {
                case DATA_POINT:
                    if (((DataPointRT<?, ?>) node).getParent() == null) {
                        result.add(((DataPointRT<?, ?>) node).getVO());
                    }
                    break;
                case POINT_FOLDER:
                    if (((PointFolderRT) node).getParent() == null) {
                        result.add(((PointFolderRT) node).getVO());
                    }
            }
        });
        return result;
    }

    public PointFolderVO savePointFolder(PointFolderVO pfvo) {
        PointFolderRT pfrt = getPointFolder(pfvo.getId());
        if (pfrt.getParent() != null) {
            pfrt.getParent().validateChildName(pfvo.getName());
        }
        pfvo = nodeEdgeDao.saveNode(pfvo);
        pfrt.patch(pfvo);
        return pfvo;
    }

    public void addDataPoint(DataPointVO dpvo, int parentId) {
        PointFolderRT src = (PointFolderRT) nodes.get(parentId);
        if (src != null) {
            src.validateChildName(dpvo.getName());
        }
        dpvo = nodeEdgeDao.saveNode(dpvo);
        nodes.put(dpvo.getId(), dpvo.createRT());
        nodeEdgeDao.saveEdge(parentId, dpvo.getId(), EdgeType.TREE_PARENT_TO_CHILD);
        try {
            nodes.get(parentId).wireEdgeAsSrc(nodes.get(dpvo.getId()), EdgeType.TREE_PARENT_TO_CHILD);
        } catch (WrongEdgeTypeException ex) {
            LOG.log(Level.SEVERE, "Can't wire in RuntimeManager.addPointFolder", ex);
            throw new RuntimeException(ex);
        }
    }

    public void deleteNode(int id) {
        RT node = getNode(id);
        switch (node.getNodeType()) {
            case POINT_FOLDER:
                deletePointFolder((PointFolderRT) node);
                break;
            case DATA_POINT:
                deleteDataPoint((DataPointRT) node);
                break;
            default:
                throw new ImplementMeException();
        }
    }

    public VO saveNode(VO vo) {
        switch (vo.getNodeType()) {
            case POINT_FOLDER:
                return savePointFolder((PointFolderVO) vo);
            case DATA_POINT:
                return saveDataPoint((DataPointVO) vo);
            default:
                throw new ImplementMeException();
        }
    }

    public <T extends RT<?>> Stream<T> getNodes(Class<T> clazz, NodeType nodeType) {
        return nodes.entrySet().stream()
                .map((e) -> (T) e.getValue())
                .filter((rt) -> {
                    return rt.getNodeType() == nodeType;
                });
    }

    public Stream<UserRT> getActiveUsers() {
        return getNodes(UserRT.class, NodeType.USER)
                .filter((user) -> {
                    return !user.isDisabled();
                });
    }

    public UserRT getUser(String username) {
        return getFirstNodeByName(username, UserRT.class, NodeType.USER);
    }

    public <T extends RT<?>> T getFirstNodeByName(String name, Class<T> clazz, NodeType nodeType) {
        Optional<T> result = getNodes(clazz, nodeType)
                .filter((value) -> {
                    return value.getName().equals(name);
                })
                .findAny();
        return result.get();
    }

    public void recordLogin(UserRT user) {
        //todo IMPLEMENT
    }

    public void recordLogOUT(UserRT user) {
        //todo IMPLEMENT
    }
}
