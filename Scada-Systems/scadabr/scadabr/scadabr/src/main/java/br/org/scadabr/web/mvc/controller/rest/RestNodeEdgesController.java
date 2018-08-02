/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.mvc.controller.rest;

import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.util.ScadaBrObjectMapper;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.vo.EdgeType;
import br.org.scadabr.vo.VO;
import br.org.scadabr.vo.datapoints.DataPointNodeVO;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.serotonin.mango.rt.NodeNotFoundException;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import java.net.URI;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.stream.Collectors;
import javax.inject.Inject;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestBody;
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
@RequestMapping("/REST/")
public class RestNodeEdgesController {

    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_WEB_REST);

    @Inject
    private RuntimeManager runtimeManager;
    @Inject
    private ScadaBrObjectMapper objectMapper;

    /**
     * get the root of the pointHierarchy tree
     *
     * @return the rood node
     */
    @RequestMapping(path = "pointFolders/children", method = RequestMethod.GET)
    public List<DataPointNodeVO<?>> getRootPointFoldersROOT() {
        return runtimeManager.getRootPointFolders();
    }

    @RequestMapping(path = "dataSources", method = RequestMethod.GET)
    public Iterable<DataSourceVO<?>> getDataSources() {
        return runtimeManager.getDataSources();
    }

    /**
     * Get folder node by its id
     *
     * @param id of the node
     * @return the folder TODO make this generic
     */
    @RequestMapping(value = "{id}", method = RequestMethod.GET)
    public VO getNode(@PathVariable("id") int id) {
        return runtimeManager.getNode(id).getVO();
    }

    /**
     *
     * @param id
     */
    @RequestMapping(path = "{id}", method = RequestMethod.DELETE)
    public void deleteNode(@PathVariable("id") int id) {
        runtimeManager.deleteNode(id);
    }

    @RequestMapping(method = RequestMethod.POST)
    public ResponseEntity<VO<?>> post(@RequestBody ObjectNode data, UriComponentsBuilder ucb) {
        VO<?> newNode = runtimeManager.addNode(objectMapper.convertValue(data, VO.class));
        HttpHeaders headers = new HttpHeaders();
        URI locationUri
                = ucb.path("/REST/{id}")
                .buildAndExpand(newNode.getId())
                .toUri();
        headers.setLocation(locationUri);
        return new ResponseEntity<>(newNode, headers, HttpStatus.CREATED);
    }

    @RequestMapping(method = RequestMethod.PUT)
    public VO<?> put(@RequestBody ObjectNode data) {
        VO<?> node = objectMapper.convertValue(data, VO.class);
        if (node.isNew()) {
            throw new RuntimeException("Use POST for new Node");
        }
        return runtimeManager.saveNode(node);
    }

    @RequestMapping(path = "{id}/children", method = RequestMethod.POST)
    public ResponseEntity<?> postChild(@PathVariable("id") int id, @RequestBody ObjectNode data, UriComponentsBuilder ucb) {
        VO<?> newNode;
        switch (data.findValue("nodeType").textValue()) {
            case "POINT_FOLDER":
            case "DATA_POINT": ;
                newNode = objectMapper.convertValue(data, VO.class);
                runtimeManager.addNewNodeOnEdge(id, EdgeType.TREE_PARENT_TO_CHILD, newNode);
                break;
            default:
                throw new ImplementMeException();
        }
        HttpHeaders headers = new HttpHeaders();
        URI locationUri
                = ucb.path("/REST/{id}")
                .buildAndExpand(newNode.getId())
                .toUri();
        headers.setLocation(locationUri);
        return new ResponseEntity<>(newNode, headers, HttpStatus.CREATED);
    }

    /**
     * get all child nodes (folders and datapoints) of the folder
     *
     * @param id the folderId of the parent
     * @return All child nodes
     */
    //TODO funny error if return is Iterable<VO<?>> the property @JsonTypeInfo =>>scadaBRType<< will NOT be written toi the output... spring/fasterxmnl error???
    @RequestMapping(path = "{id:\\d+}/{edgeName}", method = RequestMethod.GET)
    public List<VO<?>> getChildNodesById(@PathVariable("id") int id, @PathVariable("edgeName") String edgeName) {
        switch (edgeName) {
            case "children":
                return runtimeManager.getPointFolder(id).getChildren().collect(Collectors.toList());
            default:
                throw new ImplementMeException();
        }
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
