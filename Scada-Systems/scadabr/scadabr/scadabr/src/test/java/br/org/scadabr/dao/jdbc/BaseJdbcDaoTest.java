/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.dao.jdbc;

import br.org.scadabr.ScadaBrVersionBean;
import br.org.scadabr.jdbc.DatabaseAccessFactory;
import br.org.scadabr.rt.UserRT;
import com.serotonin.mango.rt.RuntimeManager;
import java.util.Collection;
import javax.inject.Inject;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.test.context.ContextConfiguration;
import org.springframework.test.context.junit4.SpringJUnit4ClassRunner;

import static org.junit.Assert.*;
import org.junit.Ignore;

/**
 *
 * @author aploese
 */
@RunWith(SpringJUnit4ClassRunner.class)
@ContextConfiguration(classes = {BaseJdbcDaoTest.Config.class})
public class BaseJdbcDaoTest {

    @Configuration
    public static class Config {

        private final RuntimeManager runtimeManager = new RuntimeManager();
        
        private final DatabaseAccessFactory databaseAccessFactory = new DatabaseAccessFactory();
                
        private final ScadaBrVersionBean ScadaBrVersionBean = new ScadaBrVersionBean();
        

        @Bean
        public RuntimeManager getRuntimeManager() {
            return runtimeManager;
        }
        
        @Bean
        public DatabaseAccessFactory getDatabaseAccessFactory() {
            return databaseAccessFactory;
        }

        @Bean
        public ScadaBrVersionBean getScadaBrVersionBean() {
            return ScadaBrVersionBean;
        }
        

    }

    public BaseJdbcDaoTest() {
    }

    @Inject
    private RuntimeManager runtimeManager;

    @Before
    public void setUp() {
    }

    @Test
    @Ignore
    public void testSetup() {
//TODO        Collection<UserRT> users = runtimeManager.getUsers();
//TODO        assertEquals(1, users.size());
    }

}
