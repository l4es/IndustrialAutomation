/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.mvc.controller.datasources;

import com.serotonin.mango.vo.dataSource.DataSourceVO;
import java.io.Serializable;

/**
 * Tree node 
 * @author aploese
 */
public class JsonDataSourceWrapper implements Serializable {

    private final DataSourceVO dsVo;

    public JsonDataSourceWrapper(DataSourceVO<?> dsVo) {
        this.dsVo = dsVo;
    }

    /**
     * @return the name
     */
    public String getName() {
        return dsVo.getName();
    }

    public String getNodeType() {
        return "DataSource";
    }
    
    /**
     * @return the typeKey
     */
    public String getTypeKey() {
        return dsVo.getTypeKey();
    }

    /**
     * @return the id
     */
    public int getId() {
        return dsVo.getId();
    }

    /**
     * @return the enabled
     */
    public boolean isEnabled() {
        return dsVo.isEnabled();
    }

}
