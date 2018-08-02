/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo.datasource;

import br.org.scadabr.utils.i18n.LocalizableMessage;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import java.util.Objects;
import javax.annotation.PostConstruct;
import javax.annotation.PreDestroy;
import javax.inject.Inject;

/**
 *
 * @author aploese
 */
public abstract class DataSourceType implements LocalizableMessage {

    @Inject
    private DataSourcesRegistry dataSourcesRegistry;
    
    @PostConstruct
    public void register() {
        dataSourcesRegistry.registerDataSource(this);
    }
    
    @PreDestroy
    public void remove() {
        dataSourcesRegistry.removeDataSource(this);
    }
    
    public abstract String getTypeKey();
    public abstract Class getTypeClass();
    public abstract DataSourceVO<?> createDataSourceVO();

    @Override
    public int hashCode() {
        int hash = 5;
        hash = 59 * hash + Objects.hashCode(getTypeKey());
        return hash;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final DataSourceType other = (DataSourceType) obj;
        if (!Objects.equals(this.getTypeKey(), other.getTypeKey())) {
            return false;
        }
        return true;
    }
    
    
    
    @Override
    public String toString() {
        return "DataSourceType[typeName=" + getTypeKey() + "]";
    }
    
}
