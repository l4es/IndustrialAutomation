/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.mvc.controller.rest;

import br.org.scadabr.l10n.Localizer;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import java.io.Serializable;

/**
 *
 * @author aploese
 */
public class JsonDataSource implements Serializable {

    private String name;
    private String typeKey;
    private int id;

    public JsonDataSource(DataSourceVO<?> dsVo, Localizer localizer) {
        this.name = dsVo.getName();
        this.id = dsVo.getId();
        this.typeKey = dsVo.getDataSourceTypeKey(); //TODO localizer.getMessage(dsVo.getDataSourceTypeI18n());
    }

    /**
     * @return the name
     */
    public String getName() {
        return name;
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
    public int getId() {
        return id;
    }

    /**
     * @param id the id to set
     */
    public void setId(int id) {
        this.id = id;
    }
}
