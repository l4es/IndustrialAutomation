/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.dao;

import br.org.scadabr.rt.UserRT;
import com.serotonin.mango.vo.UserComment;
import java.util.Collection;
import javax.inject.Named;

/**
 *
 * @author aploese
 */
@Named
public interface UserDao {

    void insertUserComment(int TYPE_EVENT, int eventId, UserComment comment);

    UserRT getUser(int userId);

    /**
     * Returns all users
     * @return 
     */
    Collection<UserRT> getUsers();

    Collection<UserRT> getActiveUsers();

    void saveHomeUrl(int id, String url);

    UserRT getUser(String username);

    void recordLogin(int id);
    
}
