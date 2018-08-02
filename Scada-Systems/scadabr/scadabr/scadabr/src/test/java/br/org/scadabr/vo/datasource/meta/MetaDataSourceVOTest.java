/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo.datasource.meta;

import br.org.scadabr.dao.DataSourceDao;
import br.org.scadabr.util.ScadaBrObjectMapper;
import br.org.scadabr.vo.VO;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.serotonin.mango.vo.dataSource.meta.MetaDataSourceVO;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import javax.inject.Inject;
import javax.validation.ConstraintViolation;
import javax.validation.Validator;
import org.easymock.EasyMock;
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
@ContextConfiguration(classes = {MetaDataSourceVOTest.Config.class})
public class MetaDataSourceVOTest {

    @Configuration
    public static class Config {

        private final Validator validator = new org.springframework.validation.beanvalidation.LocalValidatorFactoryBean();
        private final DataSourceDao dataSourceDao = EasyMock.createMock(DataSourceDao.class);
        private final ScadaBrObjectMapper scadaBrObjectMapper = new ScadaBrObjectMapper();
        
        @Bean
        public Validator getValidator() {
            return validator;
        }

        @Bean
        public DataSourceDao getDsDao() {
            return dataSourceDao;
        }
        
        @Bean
        public ScadaBrObjectMapper getScadaBrObjectMapper() {
            return scadaBrObjectMapper;
        }

    }

    public MetaDataSourceVOTest() {
    }

    @Inject
    private Validator validator;
    @Inject
    private DataSourceDao dataSourceDao;
    @Inject
    private ScadaBrObjectMapper scadaBrObjectMapper;
            

    @Before
    public void setUp() {
        EasyMock.reset(dataSourceDao);
    }

    
    @Test
    public void testJsonPersistence() throws Exception {
        MetaDataSourceVO sourceVO = new MetaDataSourceVO();
        sourceVO.setName("Test-" + sourceVO.getName());
        String s = scadaBrObjectMapper.writeValueAsString(sourceVO);
        assertEquals("{\"scadaBrType\":\"DATA_SOURCE.MetaDataSource\",\"id\":null,\"xid\":null,\"name\":\"Test-MetaDataSourceVO\",\"enabled\":false,\"dataSourceTypeKey\":\"META\",\"nodeType\":\"DATA_SOURCE\"}", s);
        scadaBrObjectMapper.registerSubtypes(MetaDataSourceVO.class);
        MetaDataSourceVO sourceVO1 = (MetaDataSourceVO)scadaBrObjectMapper.readValue(s, VO.class);
    }
    
    
    @Test
    public void testValidateAllErrors() {
        MetaDataSourceVO instance = new MetaDataSourceVO();
                
        EasyMock.expect(dataSourceDao.isXidUnique(instance)).andReturn(false);
        EasyMock.replay(dataSourceDao);
        
        Set<ConstraintViolation<MetaDataSourceVO>> constraintViolations = validator.validate(instance);

        EasyMock.verify(dataSourceDao);

        assertEquals(2, constraintViolations.size());
        Map<String, Set<String>> errors = new HashMap<>();

        for (ConstraintViolation<MetaDataSourceVO> cv : constraintViolations) {
            Set set = errors.get(cv.getPropertyPath().toString());
            if (set == null) {
                set = new HashSet<>();
                errors.put(cv.getPropertyPath().toString(), set);
            }
            set.add(cv.getMessageTemplate());
        }
        
        Set result = errors.remove("xid");
        assertNotNull(result);
        assertTrue(result.remove("{javax.validation.constraints.NotNull.message}"));
        assertTrue(result.remove("{validate.xidUsed}"));
        assertTrue(result.isEmpty());

        assertTrue(errors.isEmpty());


    }

    @Test
    public void testValidateNoErrors() {
        MetaDataSourceVO instance = new MetaDataSourceVO();
        
        instance.setXid("Test_XID");
        
        EasyMock.expect(dataSourceDao.isXidUnique(instance)).andReturn(true);
        EasyMock.replay(dataSourceDao);

        Set<ConstraintViolation<MetaDataSourceVO>> constraintViolations = validator.validate(instance);

        EasyMock.verify(dataSourceDao);

        assertEquals(0, constraintViolations.size());

    }
}
