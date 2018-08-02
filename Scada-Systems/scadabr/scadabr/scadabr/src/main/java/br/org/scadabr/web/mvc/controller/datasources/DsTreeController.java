/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.mvc.controller.datasources;

import br.org.scadabr.dao.DataSourceDao;
import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.rt.UserRT;
import br.org.scadabr.vo.datasource.PointLocatorFolderVO;
import br.org.scadabr.vo.datasource.PointLocatorVO;
import br.org.scadabr.web.l10n.RequestContextAwareLocalizer;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.permission.Permissions;
import com.serotonin.mango.web.UserSessionContextBean;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.logging.Logger;
import javax.inject.Inject;
import org.springframework.context.annotation.Scope;
import org.springframework.context.annotation.ScopedProxyMode;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;

/**
 * Rest Controller for the DataSources Tree
 * @author aploese
 */
@Deprecated //??? use RestDataSourceController...???
@RestController
@RequestMapping(value = "/configTree/")
@Scope(value = "request", proxyMode = ScopedProxyMode.TARGET_CLASS)
public class DsTreeController {

    private static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_WEB);

    @Inject
    private UserSessionContextBean userSessionContextBean;
    @Inject
    private RuntimeManager runtimeManager;
    @Inject
    private DataSourceDao dataSourceDao;
    
    @Inject 
    private RequestContextAwareLocalizer localizer;

//TODO Use view @JsonView(View.Summary.class)
    // ?? @JsonAppend(attrs = @JsonAppend.Attr
    @RequestMapping(value = "dataSources", method = RequestMethod.GET)
    public Iterable<JsonDataSourceWrapper> getDataSources() {
        final UserRT user = userSessionContextBean.getUser();
        List<JsonDataSourceWrapper> result = new ArrayList<>();
        for (DataSourceVO<?> ds : runtimeManager.getDataSources()) {
            if (Permissions.hasDataSourcePermission(user, ds.getId())) {
                result.add(new JsonDataSourceWrapper(ds));
            }
        }
        return result;
    }
    
    /**
     * Get folder node by its id
     * @param id of the node
     * @return the folder
     */
    @RequestMapping(value = "dataSources/{id}", method = RequestMethod.GET)
    public JsonDataSourceWrapper getDataSource(@PathVariable("id") int id) {
        return new JsonDataSourceWrapper(runtimeManager.getDataSourceVO(id));
    }

    /**
     * 
     * @param id
     * @return 
     * 
     * get root plf nodes of ds
     * 
     */
    @RequestMapping(value = {"dataSources/{id}/pointLocators"}, method = RequestMethod.GET)
//    @RequestMapping(params = {"dsId", "parentFolderId"}, method = RequestMethod.GET)
    public List<Serializable> getPointLocatorsOfDataSource(@PathVariable("id") int id) {
        List<Serializable> result = new LinkedList<>();
        for (PointLocatorFolderVO folderVO : dataSourceDao.getPointLocatorsFolderByParent(id, 0)) {
            result.add(folderVO);
        }
        for (PointLocatorVO locatorVO : dataSourceDao.getPointLocatorsByParent(id, 0)) {
            result.add(new JsonPointLocator(locatorVO, localizer));
        }
        return result;
    }
    /**
     * get all child nodes (folders and datapoints) of the folder
     * @param id
     * @return All childnodes
     */
    @RequestMapping(value = {"pointLocatorFolders/{id}/pointLocators"}, method = RequestMethod.GET)
//    @RequestMapping(params = {"dsId", "parentFolderId"}, method = RequestMethod.GET)
    public List<Serializable> getPointLocators(@PathVariable("id") int id) {
        List<Serializable> result = new LinkedList<>();
        for (PointLocatorFolderVO folderVO : dataSourceDao.getPointLocatorsFolderByParent(id)) {
            result.add(folderVO);
        }
        for (PointLocatorVO locatorVO : dataSourceDao.getPointLocatorsByParent(id)) {
            result.add(new JsonPointLocator(locatorVO, localizer));
        }
        return result;
    }

}
