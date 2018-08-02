/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.dao;

import com.serotonin.mango.vo.WatchList;
import java.util.List;
import javax.inject.Named;

/**
 *
 * @author aploese
 */
@Named
public interface WatchListDao {

    public List<WatchList> getWatchLists(int id);

    public WatchList createNewWatchList(WatchList watchList, int id);

    public void saveWatchList(WatchList watchList);

    public void saveSelectedWatchList(int id, int selected);

    public WatchList getWatchList(int watchListId);

    public boolean isXidUnique(String xid, int id);
    
}
