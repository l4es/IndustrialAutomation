/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.mvc.controller.rpc;

import br.org.scadabr.DataType;
import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.rt.DataPointNodeRT;
import com.serotonin.mango.rt.NodeNotFoundException;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import java.util.EnumMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.inject.Inject;
import org.springframework.http.HttpStatus;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.ResponseStatus;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.util.UriComponentsBuilder;

/**
 *
 * @author aploese
 */
@RestController
@RequestMapping("/RPC/DataPoints")
public class RpcDataPointController {

    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_WEB_RPC);

    @Inject
    private RuntimeManager runtimeManager;

    @RequestMapping(path = "DataTypes", method = RequestMethod.GET)
    public Map<DataType, String> dataTypes() {
        Map result = new EnumMap(DataType.class);
        for (DataType dt : DataType.values()) {
            result.put(dt, dt.getI18nKey());
        }
        return result;
    }

    @RequestMapping(value = "{id}/treePath", method = RequestMethod.GET)
    public List<Integer> getTreePath(@PathVariable("id") int id, UriComponentsBuilder ucb) {
        DataPointNodeRT dpnRt = (DataPointNodeRT) runtimeManager.getNode(id);
        LinkedList<Integer> result = new LinkedList<>();
        while (dpnRt != null) {
            result.addFirst(dpnRt.getId());
            dpnRt = dpnRt.getParent();
        }
        return result;
    }

    @RequestMapping(value = "{id}/start", method = RequestMethod.PUT)
    public void start(@PathVariable("id") int id, UriComponentsBuilder ucb) {
        DataSourceVO dsVo = (DataSourceVO) runtimeManager.getNode(id).getVO();
        dsVo.setEnabled(true);
        runtimeManager.saveDataSource(dsVo);
    }

    @RequestMapping(value = "{id}/stop", method = RequestMethod.PUT)
    public void stop(@PathVariable("id") int id, UriComponentsBuilder ucb) {
        DataSourceVO dsVo = (DataSourceVO) runtimeManager.getNode(id).getVO();
        dsVo.setEnabled(true);
        runtimeManager.saveDataSource(dsVo);
    }

    @ExceptionHandler(Throwable.class)
    @ResponseStatus(HttpStatus.INTERNAL_SERVER_ERROR)
    public Throwable onError(Throwable t) {
        LOG.log(Level.SEVERE, "Rest Req", t);
        return t;
    }

    @ExceptionHandler(NodeNotFoundException.class)
    @ResponseStatus(HttpStatus.NOT_FOUND)
    public String onError(NodeNotFoundException e) {
        LOG.log(Level.SEVERE, "Node not found", e);
        return "Node ID not found: " + e.getId();
    }

}
