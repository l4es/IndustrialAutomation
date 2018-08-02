/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.dao;

import com.serotonin.mango.vo.event.MaintenanceEventVO;

/**
 *
 * @author aploese
 */
public interface MaintenanceEventDao {

    public Iterable<MaintenanceEventVO> getMaintenanceEvents();

    public void saveMaintenanceEvent(MaintenanceEventVO vo);

    public void deleteMaintenanceEvent(int id);

    public void deleteMaintenanceEventsForDataSource(int dataSourceId);
    
}
