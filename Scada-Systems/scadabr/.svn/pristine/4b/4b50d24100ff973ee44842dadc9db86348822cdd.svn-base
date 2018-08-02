/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.mvc.controller.rest;

import br.org.scadabr.dao.DataPointDao;
import br.org.scadabr.dao.WatchListDao;
import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.web.l10n.RequestContextAwareLocalizer;
import br.org.scadabr.web.mvc.controller.jsonrpc.JsonWatchList;
import br.org.scadabr.web.mvc.controller.jsonrpc.JsonWatchListPoint;
import com.serotonin.mango.rt.RuntimeManager;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.inject.Inject;
import javax.servlet.http.HttpServletRequest;
import org.springframework.context.annotation.Scope;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;

/**
 *
 * @author aploese
 */
@RestController
@Scope("request")
@RequestMapping(value = "/rest/watchLists/")
public class RestWatchListController {

    private static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_WEB);

    @Inject
    private transient DataPointDao dataPointDao;
    @Inject
    private transient WatchListDao watchListDao;
    @Inject
    private transient RuntimeManager runtimeManager;
    @Inject
    private RequestContextAwareLocalizer localizer;

    @RequestMapping(params = "id", method = RequestMethod.GET)
    public JsonWatchList getWatchList(int id, HttpServletRequest request) {
        LOG.log(Level.SEVERE, "CALLED: getWatchList {0}", id);
        //TODO use JDK 8 clojure???
        final JsonWatchList result = new JsonWatchList(watchListDao.getWatchList(id), dataPointDao, runtimeManager, localizer);
        for (JsonWatchListPoint jwp : result) {
            LOG.log(Level.SEVERE, "JWP: {0}", jwp.getCanonicalName());
        }
        return result;
    }

}
