/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.dao.jdbc;

import br.org.scadabr.DataType;
import br.org.scadabr.ScadaBrVersionBean;
import br.org.scadabr.dao.DataPointDao;
import br.org.scadabr.dao.DataSourceDao;
import br.org.scadabr.dao.MaintenanceEventDao;
import br.org.scadabr.dao.PointLinkDao;
import br.org.scadabr.jdbc.DatabaseAccessFactory;
import br.org.scadabr.rt.link.PointLinkManager;
import br.org.scadabr.util.ScadaBrObjectMapper;
import br.org.scadabr.vo.datasource.PointLocatorFolderVO;
import com.serotonin.mango.vo.dataSource.meta.MetaDataSourceVO;
import com.serotonin.mango.vo.dataSource.meta.MetaPointLocatorVO;
import javax.inject.Inject;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Ignore;
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
@ContextConfiguration(classes = {DataSourceDaoTest.Config.class})
public class DataSourceDaoTest {

    @Configuration
    public static class Config {
        
        private final MaintenanceEventDao maintenanceEventDao = new MaintenanceEventDaoImpl();
        
        private final DataPointDao dataPointDao = new DataPointDaoImpl();
        
        private final DataSourceDao dataSourceDao = new DataSourceDaoImpl();

        private final DatabaseAccessFactory databaseAccessFactory = new DatabaseAccessFactory();
                
        private final ScadaBrVersionBean ScadaBrVersionBean = new ScadaBrVersionBean();
        
        private final ScadaBrObjectMapper scadaBrObjectMapper = new ScadaBrObjectMapper();
        

        /**
         * No tests with PointLinkManager so return null.
         * @return 
         */
        @Bean
        public PointLinkManager getPointLinkManager() {
            return null;
        }
        
        /**
         * No tests with PointLinkDao so return null.
         * @return 
         */
        @Bean
        public PointLinkDao getPointLinkDao() {
            return null;
        }
        
        @Bean
        public DataSourceDao getDataSourceDao() {
            return dataSourceDao;
        }
        
        @Bean
        public DataPointDao getDataPointDao() {
            return dataPointDao;
        }
        
        @Bean
        public MaintenanceEventDao getMaintenanceEventDao() {
            return maintenanceEventDao;
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
        

    }

    public DataSourceDaoTest() {
    }

    @Inject
    private DataSourceDao dataSourceDao;
    
    @Before
    public void setUp() {
    }

    @Ignore
    @Test
    public void testCRUD() {
        MetaDataSourceVO dataSource = new MetaDataSourceVO();
        dataSourceDao.saveDataSource(dataSource);
        MetaDataSourceVO dataSource1 =  (MetaDataSourceVO)dataSourceDao.getDataSource(dataSource.getId());
        dataSource1.setName("Test1");
        dataSource1.setEnabled(true);
        dataSourceDao.saveDataSource(dataSource1);
        
        PointLocatorFolderVO plfVo = new PointLocatorFolderVO(dataSource, "TestA");
        
        dataSourceDao.savePointLocatorFolder(plfVo);
        Assert.assertNotNull(plfVo.getId());
        Assert.assertEquals(dataSource.getId(), plfVo.getDataSourceId());
        
        MetaPointLocatorVO pointLocator = new MetaPointLocatorVO(DataType.DOUBLE);
        pointLocator.setDataSourceId(dataSource.getId());
        pointLocator.setPointLocatorFolderId(plfVo.getId());
        dataSourceDao.savePointLocator(pointLocator);
        Assert.assertFalse(pointLocator.isNew());
        Assert.assertNotNull(pointLocator.getDataSourceId());
        Assert.assertNotNull(pointLocator.getPointLocatorFolderId());
        
        //TODO datapoint
        
        dataSourceDao.deleteDataSource(dataSource.getId());
        
        Assert.assertNull(dataSourceDao.getPointLocator(pointLocator.getId()));
        
    }

}
