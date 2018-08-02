/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo;

import br.org.scadabr.ShouldNeverHappenException;

/**
 *
 * @author aploese
 */
public enum EdgeType {
    /**
     * 
     * src is the parent 
     * dest is the child
     */
    TREE_PARENT_TO_CHILD;

    public static EdgeType fromId(int id) {
        for (EdgeType edgeType : values()) {
            if (edgeType.getId() == id) {
                return edgeType;
            }
        }
        throw new ShouldNeverHappenException("Unknown EdgeTypeId: " + id);
    }
    
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
