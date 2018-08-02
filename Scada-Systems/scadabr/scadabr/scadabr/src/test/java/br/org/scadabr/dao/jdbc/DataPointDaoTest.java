/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.dao.jdbc;

import br.org.scadabr.ModuleRegistry;
import br.org.scadabr.ScadaBrVersionBean;
import br.org.scadabr.dao.NodeEdgeDao;
import br.org.scadabr.jdbc.DatabaseAccessFactory;
import br.org.scadabr.rt.link.PointLinkManager;
import br.org.scadabr.util.ScadaBrObjectMapper;
import br.org.scadabr.vo.Edge;
import br.org.scadabr.vo.EdgeType;
import br.org.scadabr.vo.NodeType;
import br.org.scadabr.vo.VO;
import br.org.scadabr.vo.datapoints.PointFolderVO;
import com.serotonin.mango.vo.DataPointVO;
import java.util.HashSet;
import java.util.Set;
import javax.inject.Inject;
import org.junit.After;
import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.test.context.ContextConfiguration;
import org.springframework.test.context.junit4.SpringJUnit4ClassRunner;

/**
 *
 * @author aploese
 */
@RunWith(SpringJUnit4ClassRunner.class)
@ContextConfiguration(classes = {DataPointDaoTest.Config.class})
public class DataPointDaoTest {

    @Configuration
    public static class Config {
        
        private final NodeEdgeDao nodeEdgeDao = new NodeEdgeDaoImpl();
        
        private final DatabaseAccessFactory databaseAccessFactory = new DatabaseAccessFactory();
                
        private final ScadaBrVersionBean ScadaBrVersionBean = new ScadaBrVersionBean();
        
        private final ScadaBrObjectMapper scadaBrObjectMapper = new ScadaBrObjectMapper();

        private final ModuleRegistry moduleRegistry = new ModuleRegistry();
        /**
         * No tests with PointLinkManager so return null.
         * @return 
         */
        @Bean
        public PointLinkManager getPointLinkManager() {
            return null;
        }
        
        @Bean
        public NodeEdgeDao getNodeEdgeDao() {
            return nodeEdgeDao;
        }
        
        @Bean
        public DatabaseAccessFactory getDatabaseAccessFactory() {
            return databaseAccessFactory;
        }

        @Bean
        public ScadaBrVersionBean getScadaBrVersionBean() {
            return ScadaBrVersionBean;
        }
        
        @Bean 
        public ScadaBrObjectMapper getScadaBrObjectMapper() {
            return scadaBrObjectMapper;
        }

        @Bean 
        public ModuleRegistry getModuleRegistry() {
            return moduleRegistry;
        }
    }

    public DataPointDaoTest() {
    }

    @Inject
    private NodeEdgeDao<? extends VO<?>> nodeEdgeDao;
    @Inject
    ScadaBrObjectMapper scadaBrObjectMapper;
    

    @After
    public void tearDown() {
    }
    
     /**
     * Test the whole lifecycle of a statefull event
     */
    @Test
    public void testPointFolderCRUD() {
        
        final PointFolderVO pfvo = new PointFolderVO();
        pfvo.setName("0 0");
        PointFolderVO p = nodeEdgeDao.saveNode(pfvo);
        PointFolderVO pfvo1 = (PointFolderVO)nodeEdgeDao.getNodeById(pfvo.getId());
        pfvo1.setName("TEST1");
        nodeEdgeDao.saveNode(pfvo1);
        PointFolderVO pfvo2 = (PointFolderVO)nodeEdgeDao.getNodeById(pfvo.getId());
        assertEquals(pfvo1.getName(), pfvo2.getName());
        assertEquals(pfvo1.getId(), pfvo2.getId());
        assertTrue(nodeEdgeDao.isNodeExisting(pfvo.getId()));
        nodeEdgeDao.deleteNode(pfvo.getId());
        assertFalse(nodeEdgeDao.isNodeExisting(pfvo.getId()));
        
    }

     /**
     * Test the whole lifecycle of a statefull event
     */
    @Test
    public void testParentChildOfPointFolderCRUD() {
        
        final Set<Edge> edges = new HashSet<>();
        final PointFolderVO pf_0 = new PointFolderVO();
        pf_0.setName("PF 0");
        nodeEdgeDao.saveNode(pf_0);

        final PointFolderVO pf_0_0 = new PointFolderVO();
        pf_0_0.setName("PF 0 0");
        nodeEdgeDao.saveNode(pf_0_0);
        Edge edge_0_To_0_0 = nodeEdgeDao.saveEdge(new Edge(pf_0, pf_0_0, EdgeType.TREE_PARENT_TO_CHILD));
        edges.add(edge_0_To_0_0);

        final PointFolderVO pf_0_1 = new PointFolderVO();
        pf_0_1.setName("PF 0 1");
        nodeEdgeDao.saveNode(pf_0_1);
        Edge edge_0_To_0_1 = nodeEdgeDao.saveEdge(new Edge(pf_0, pf_0_1, EdgeType.TREE_PARENT_TO_CHILD));
        edges.add(edge_0_To_0_1);

        
        final PointFolderVO pf_0_1_0 = new PointFolderVO();
        pf_0_1_0.setName("PF 0 1 0");
        nodeEdgeDao.saveNode(pf_0_1_0);
        Edge edge_0_1_To_0_1_0 = nodeEdgeDao.saveEdge(new Edge(pf_0_1, pf_0_1_0, EdgeType.TREE_PARENT_TO_CHILD));
        edges.add(edge_0_1_To_0_1_0);

        final PointFolderVO pf_0_2 = new PointFolderVO();
        pf_0_2.setName("PF 0 2");
        nodeEdgeDao.saveNode(pf_0_2);
        Edge edge_0_To_0_2 = nodeEdgeDao.saveEdge(new Edge(pf_0, pf_0_2, EdgeType.TREE_PARENT_TO_CHILD));
        edges.add(edge_0_To_0_2);

        assertTrue(nodeEdgeDao.isEdgeExisting(edge_0_To_0_2));
        
        assertEquals(5, nodeEdgeDao.countNodes(NodeType.POINT_FOLDER));
        assertEquals(4, nodeEdgeDao.countEdges(EdgeType.TREE_PARENT_TO_CHILD));

        nodeEdgeDao.deleteEdge(edge_0_1_To_0_1_0);
        assertFalse(nodeEdgeDao.isEdgeExisting(edge_0_1_To_0_1_0));
        assertEquals(3, nodeEdgeDao.countEdges(EdgeType.TREE_PARENT_TO_CHILD));
        nodeEdgeDao.deleteNode(pf_0_1_0);
        
        
        
    }

    /**
     * Test the whole lifecycle of a statefull event
     */
    @Test
    public void testDoubleDataPointCRUD() {
     /*   
        final DataPointVO dpvo = new DoubleDataPointVO();
        dataPointDao.saveDataPoint(dpvo);
        DataPointVO dpvo1 = dataPointDao.getDataPoint(dpvo.getId());
        dpvo1.setName("TEST1");
        dataPointDao.saveDataPoint(dpvo1);
        DataPointVO dpvo2 = dataPointDao.getDataPoint(dpvo.getId());
        assertEquals(dpvo1, dpvo2);
        assertEquals(dpvo1.getLoggingType(), dpvo2.getLoggingType());
        dataPointDao.deleteDataPoint(dpvo.getId());
        DataPointVO dpvo3 = dataPointDao.getDataPoint(dpvo.getId());
        
        assertNull(dpvo3);
        */
    }

}
