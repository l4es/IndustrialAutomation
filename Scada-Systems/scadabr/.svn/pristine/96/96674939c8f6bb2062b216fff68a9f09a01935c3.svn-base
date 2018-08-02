/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo.datasource.meta;

import br.org.scadabr.DataType;
import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.DoubleDataPointRT;
import com.serotonin.mango.rt.dataSource.meta.MetaDataSourceRT;
import com.serotonin.mango.rt.dataSource.meta.MetaPointLocatorRT;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.DoubleDataPointVO;
import com.serotonin.mango.vo.dataSource.meta.MetaDataSourceVO;
import com.serotonin.mango.vo.dataSource.meta.MetaPointLocatorVO;
import org.junit.Test;
import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Ignore;

/**
 *
 * @author aploese
 */
public class MetaPointLocatorVOTest {

    public MetaPointLocatorVOTest() {
    }

    @Before
    public void setUp() {
    }

    
    @Ignore
    @Test
    public void testRunScript() {
        MetaDataSourceVO dsVO = new MetaDataSourceVO();
        MetaPointLocatorVO plVO = new MetaPointLocatorVO(DataType.DOUBLE);
        
        DoubleDataPointVO dpVO = new DoubleDataPointVO();
        
        plVO.setScript("return -1;");
        plVO.setUpdateEvent(UpdateEvent.SECONDS);
                
        MetaDataSourceRT dsRT = dsVO.createRT();
        MetaPointLocatorRT plRT = plVO.createRuntime();
        DoubleDataPointRT dpRT = new DoubleDataPointRT(dpVO, plRT);
//        plRT.start(dsRT, dpRT);
//        plRT.doPoll(0);
        
        
//        fail("Proto");
    }

}
