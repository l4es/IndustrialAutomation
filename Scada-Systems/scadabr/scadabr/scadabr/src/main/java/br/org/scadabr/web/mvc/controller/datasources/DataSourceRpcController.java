/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.mvc.controller.datasources;

import br.org.scadabr.DataType;
import br.org.scadabr.dao.DataSourceDao;
import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.vo.datasource.PointLocatorVO;
import br.org.scadabr.vo.datasource.DataSourcesRegistry;
import br.org.scadabr.web.l10n.RequestContextAwareLocalizer;
//import com.googlecode.jsonrpc4j.JsonRpcService;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.dataSource.meta.MetaPointLocatorVO;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.inject.Inject;
import javax.inject.Named;
import org.springframework.context.annotation.Scope;
import org.springframework.context.annotation.ScopedProxyMode;

/**
 *
 * @author aploese
 */
@Named
//@JsonRpcService("/dataSources/rpc/")
@Scope(value = "request", proxyMode = ScopedProxyMode.TARGET_CLASS)
public class DataSourceRpcController {

    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_WEB);

    @Inject
    private DataSourceDao dataSourceDao;
    @Inject
    private RequestContextAwareLocalizer localizer;
    @Inject
    private RuntimeManager runtimeManager;
    @Inject
    private DataSourcesRegistry dataSourcesRegistry;

    public JsonDataSourceWrapper addDataSource(String type) {
        DataSourceVO result = dataSourcesRegistry.createDataSourceVO(type);
        dataSourceDao.saveDataSource(result);
        return new JsonDataSourceWrapper(result);
    }

    public boolean deleteDataSource(int id) {
        try {
            runtimeManager.deleteDataSource(id);
            return true;
        } catch (Exception e) {
            LOG.log(Level.SEVERE, "Error saving DataSource with id: " + id, e);
            return false;
        }
    }

    public <T extends PointValueTime> PointLocatorVO addPointLocator(int dataSourceId, int pointLocatorId) {
        PointLocatorVO<T> result = new MetaPointLocatorVO<>(DataType.DOUBLE);
        result.setDataSourceId(dataSourceId);
        result.setPointLocatorFolderId(pointLocatorId);
        dataSourceDao.savePointLocator(result);
        return result;
    }

    public JsonDataSourceWrapper startDataSource(int id) {
        DataSourceVO dsVo = dataSourceDao.getDataSource(id);
        dsVo.setEnabled(true);
        runtimeManager.saveDataSource(dsVo);
        return new JsonDataSourceWrapper(dsVo);
    }

    public JsonDataSourceWrapper stopDataSource(int id) {
        DataSourceVO dsVo = dataSourceDao.getDataSource(id);
        dsVo.setEnabled(false);
        runtimeManager.saveDataSource(dsVo);
        return new JsonDataSourceWrapper(dsVo);
    }

    public JsonPointLocator startPointLocator(int id) {
        PointLocatorVO plvo = dataSourceDao.getPointLocator(id);
        plvo.setEnabled(true);
        dataSourceDao.savePointLocator(plvo);
        return new JsonPointLocator(plvo, localizer);
    }

    public JsonPointLocator stopPointLocator(int id) {
        PointLocatorVO plvo = dataSourceDao.getPointLocator(id);
        plvo.setEnabled(false);
        dataSourceDao.savePointLocator(plvo);
        return new JsonPointLocator(plvo, localizer);
    }

}
