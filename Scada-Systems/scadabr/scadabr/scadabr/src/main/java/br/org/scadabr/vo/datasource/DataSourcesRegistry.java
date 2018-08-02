/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo.datasource;

import br.org.scadabr.logger.LogUtils;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.inject.Named;

/**
 * This bean registers and hold the various datasources.
 * @author aploese
 */
@Named
public class DataSourcesRegistry {
    
    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_CORE);
    
    Map<String, DataSourceType> dataSourceTypes = new TreeMap<>();
    
    public void registerDataSource(DataSourceType dataSourceType) {
        LOG.log(Level.INFO, "Register \"{0}\" with class: \"{1}\"", new Object[]{dataSourceType.getTypeKey(), dataSourceType.getTypeClass().getCanonicalName()});
        if (dataSourceTypes.put(dataSourceType.getTypeKey(), dataSourceType) != null) {
            LOG.log(Level.SEVERE, "Not registered! \"{0}\" with class: \"{1}\" already registered", new Object[]{dataSourceType.getTypeKey(), dataSourceType.getTypeClass().getCanonicalName()});
        }
    }
    
    public void removeDataSource(DataSourceType dataSourceType) {
        LOG.log(Level.INFO, "Remove \"{0}\" with class: \"{1}\"", new Object[]{dataSourceType.getTypeKey(), dataSourceType.getTypeClass().getCanonicalName()});
        if (dataSourceTypes.remove(dataSourceType.getTypeKey()) == null) {
            LOG.log(Level.SEVERE, "Not removed! \"{0}\" with class: \"{1}\" was not registered", new Object[]{dataSourceType.getTypeKey(), dataSourceType.getTypeClass().getCanonicalName()});
        }
    }

    public Collection<DataSourceType> getDataSourceTypes() {
        return dataSourceTypes.values();
    }

    public DataSourceVO createDataSourceVO(String type) {
        final DataSourceType dsType = dataSourceTypes.get(type);
        if (dsType == null) {
            LOG.severe("No such datasource type: " + type);
            throw new IllegalArgumentException("No such datasource type: " + type);
        }
        return dsType.createDataSourceVO();
    }
    
    
}
