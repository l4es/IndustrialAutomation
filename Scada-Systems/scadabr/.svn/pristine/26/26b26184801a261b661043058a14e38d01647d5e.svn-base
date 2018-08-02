/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.mvc.controller.rest;

import br.org.scadabr.dao.DataSourceDao;
import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.rt.UserRT;
import br.org.scadabr.web.l10n.RequestContextAwareLocalizer;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.dataSource.meta.MetaDataSourceVO;
import com.serotonin.mango.vo.permission.Permissions;
import com.serotonin.mango.web.UserSessionContextBean;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.logging.Logger;
import javax.inject.Inject;
import org.springframework.context.annotation.Scope;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;

/**
 *
 * @author aploese
 */
@RestController
@Scope("request")
@RequestMapping(value = "/rest/dataSources/")
public class RestDataSourcesController {

    private static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_WEB);

    @Inject
    private UserSessionContextBean userSessionContextBean;
    @Inject
    private RuntimeManager runtimeManager;
    @Inject
    private DataSourceDao dataSourceDao;
    
    @Inject 
    private transient RequestContextAwareLocalizer localizer;


    @RequestMapping(value = "lazyTree/", method = RequestMethod.GET)
    public List<JsonDataSource> getDataSources() {
        LOG.severe("CALLED: getDataSources");
        
        final UserRT user = userSessionContextBean.getUser();
        List<JsonDataSource> result = new ArrayList<>();
        for (DataSourceVO<?> ds : runtimeManager.getDataSources()) {
            if (Permissions.hasDataSourcePermission(user, ds.getId())) {
                result.add(new JsonDataSource(ds, localizer));
            }
        }
        return result;
    }

    
    /**
     * Get folder node by its id
     * @param id of the node
     * @return the folder
     */
    @RequestMapping(value = "lazyTree/{id}", method = RequestMethod.GET)
    public DataSourceVO<?> getFolder(@PathVariable("id") int id) {
        return dataSourceDao.getDataSource(id);
    }

    @RequestMapping(value = "lazyTree/{id}", method = RequestMethod.PUT)
    public DataSourceVO<?> putFolder(@PathVariable("id") int id, @RequestBody DataSourceVO<?> vo) {
        dataSourceDao.saveDataSource(vo);
        return vo;
    }

    @RequestMapping(value = "lazyTree/", method = RequestMethod.POST)
    public DataSourceVO<?> postFolder(@RequestBody Map<String, String> newDsType) {
        LOG.severe("DSTYPE : " + newDsType.get("dsType") );
        DataSourceVO result = new MetaDataSourceVO();
        dataSourceDao.saveDataSource(result);
        return result;
    }
    
}
