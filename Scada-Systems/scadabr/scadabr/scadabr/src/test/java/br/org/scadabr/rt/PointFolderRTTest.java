/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.rt;

import br.org.scadabr.vo.EdgeType;
import br.org.scadabr.vo.datapoints.PointFolderVO;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author aploese
 */
public class PointFolderRTTest {
    
    public PointFolderRTTest() {
    }
    
    @BeforeClass
    public static void setUpClass() {
    }
    
    @AfterClass
    public static void tearDownClass() {
    }
    
    @Before
    public void setUp() {
    }
    
    @After
    public void tearDown() {
    }

    /**
     * Test of getId method, of class PointFolderRT.
     */
    @Test
    public void testCreateRT() throws Exception {
        System.out.println("getId");
        PointFolderVO pfvo_0 = new PointFolderVO();
        pfvo_0.setId(0);
        pfvo_0.setName("0");
        PointFolderRT pfrt_0 = pfvo_0.createRT();
        PointFolderVO pfvo_0_0 = new PointFolderVO();
        pfvo_0_0.setId(1);
        pfvo_0_0.setName("0 0");
        PointFolderRT pfrt_0_0 = pfvo_0_0.createRT();
        pfrt_0.wireEdgeAsSrc(pfrt_0_0, EdgeType.TREE_PARENT_TO_CHILD);
        
        assertEquals(pfrt_0.getId(), pfrt_0_0.getParent().getId());
        assertEquals(pfrt_0_0.getId(), pfrt_0.getChildFolder(0).getId());
        
    }
    
}
