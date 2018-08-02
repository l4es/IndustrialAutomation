/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.rt;

import br.org.scadabr.vo.UserVO;
import br.org.scadabr.vo.VO;
import br.org.scadabr.vo.event.AlarmLevel;
import com.fasterxml.jackson.databind.ObjectMapper;
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
public class UserRTTest {
    
    public UserRTTest() {
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

    private UserVO createUserVO() {
        UserVO vo = new UserVO();
        vo.setId(0);
        vo.setName("admin");
        vo.setAdmin(true);
        vo.setDisabled(false);
        vo.setEmail("foo@foo.com");
        vo.setHomeUrl("http://scadabr.org.br");
        vo.setPasswordHash(UserRT.encryptPassword("password"));
        vo.setPhone("0");
        vo.setReceiveAlarmEmails(AlarmLevel.LIFE_SAFETY);
        vo.setReceiveOwnAuditEvents(true);
        return vo;
    }
    
    /**
     * Test of patch method, of class UserRT.
     */
    @Test
    public void testVO2RT2VO() {
        System.out.println("patch");
        UserVO vo = createUserVO();
        UserRT rt = vo.createRT();
        assertTrue(rt.comparePaswordWithHash("password"));
        assertEquals(vo, rt.getVO());
        vo.setDisabled(true);
        rt.patch(vo);
        assertEquals(vo, rt.getVO());
    }

    
    @Test
    public void testJson() throws Exception {
        ObjectMapper objectMapper = new ObjectMapper();
        objectMapper.registerSubtypes(UserVO.class);
        UserVO vo = createUserVO();
        String voJson = objectMapper.writeValueAsString(vo);
        assertEquals("{\"scadaBrType\":\"USER\",\"id\":0,\"name\":\"admin\",\"passwordHash\":\"XohImNooBHFR0OVvjcYpJ3NgPQ1qq73WKhHvch0VQtg=\",\"email\":\"foo@foo.com\",\"phone\":\"0\",\"admin\":true,\"disabled\":false,\"homeUrl\":\"http://scadabr.org.br\",\"lastLogin\":null,\"receiveAlarmEmails\":\"LIFE_SAFETY\",\"receiveOwnAuditEvents\":true,\"nodeType\":\"USER\"}", voJson);
        UserVO voFromJson = (UserVO)objectMapper.readValue(voJson, VO.class);
        assertEquals(vo, voFromJson);
    }
    
}
