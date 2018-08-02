/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.mvc.controller.datasources;

import br.org.scadabr.l10n.Localizer;
import br.org.scadabr.vo.datasource.PointLocatorVO;
import java.io.Serializable;

/**
 * Tree node 
 * @author aploese
 */
public class JsonPointLocator implements Serializable {

    private String name;
    private String typeKey;
    private int plId;
    private boolean enabled;

    public JsonPointLocator(PointLocatorVO dsVo, Localizer localizer) {
        this.name = dsVo.getName();
        this.plId = dsVo.getId();
        this.typeKey = localizer.getMessage(dsVo.getConfigurationDescription());
        this.enabled = dsVo.isEnabled();
    }

    JsonPointLocator(PointLocatorVO result) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    /**
     * @return the name
     */
    public String getName() {
        return name;
    }

    public String getNodeType() {
        return "PointLocator";
    }
    
    /**
     * @param name the name to set
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     * @return the typeKey
     */
    public String getTypeKey() {
        return typeKey;
    }

    /**
     * @param typeKey the typeKey to set
     */
    public void setTypeKey(String typeKey) {
        this.typeKey = typeKey;
    }

    
    
    /**
     * @return the id
     */
    public String getId() {
        return "pointLocators/" + plId;
    }

    /**
     * @param id the id to set
     */
    public void setId(String id) {
        this.plId = Integer.valueOf(id.substring("pointLocators/".length()));
    }

    /**
     * @return the plId
     */
    public int getPlId() {
        return plId;
    }

    /**
     * @param plId the plId to set
     */
    public void setPlId(int plId) {
        this.plId = plId;
    }

    /**
     * @return the enabled
     */
    public boolean isEnabled() {
        return enabled;
    }

    /**
     * @param enabled the enabled to set
     */
    public void setEnabled(boolean enabled) {
        this.enabled = enabled;
    }
    
    
}
