/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo;

/**
 *
 * @author aploese
 */
public enum NodeType {
    DATA_SOURCE,
    POINT_LOCATOR_FOLDER,
    POINT_LOCATOR,
    POINT_FOLDER,
    DATA_POINT,
    USER;
    
    private Integer id;
    
    public void setId(Integer id) {
        if (id == null) {
            //TODO Error???
        }
        if (this.id == null) {
//            LOG.debug
        } else {
//            LOG.warn
        }
        this.id = id;
    }
    
    public Integer getId() {
        return id;
    }
}
