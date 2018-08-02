/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.rt;

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.vo.EdgeType;
import br.org.scadabr.vo.NodeType;
import br.org.scadabr.vo.datapoints.DataPointNodeVO;
import br.org.scadabr.vo.datapoints.PointFolderVO;
import com.serotonin.mango.rt.dataImage.DataPointRT;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import javax.validation.ValidationException;
import br.org.scadabr.vo.EdgeConsumer;
import java.util.stream.Collectors;
import java.util.stream.Stream;

/**
 * @author Matthew Lohbihler
 *
 */
public class PointFolderRT 
        extends AbstractRT<PointFolderVO>
        implements DataPointNodeRT<PointFolderVO> {

    private final List<PointFolderRT> childFolders;

    private final List<DataPointRT<?, ?>> dataPoints;

    private PointFolderRT parentFolder;


    public PointFolderRT(PointFolderVO vo) {
        super(vo);
        childFolders = new LinkedList<>();
        dataPoints = new LinkedList<>();
    }

    @Override
    public PointFolderVO getVO() {
        final PointFolderVO result = new PointFolderVO();
        fillVO(result);
        return result;
    }

    public Iterable<PointFolderRT> getChildFolders() {
        return childFolders;
    }

    public int getChildFolderSize() {
        return childFolders.size();
    }

    public PointFolderRT getChildFolder(int index) {
        return childFolders.get(index);
    }

    public Iterable<DataPointRT<?, ?>> getDataPoints() {
        return dataPoints;
    }

    /**
     * Add the DataPointNodeRT to childFolders or dataPoints and set the parent
     * of the node tho this.
     *
     * @param node
     */
    public void addChild(DataPointNodeRT node) {
        switch (node.getNodeType()) {
            case POINT_FOLDER:
                childFolders.add((PointFolderRT) node);
                break;
            case DATA_POINT:
                dataPoints.add((DataPointRT<?, ?>) node);
                break;
            default:
                throw new ShouldNeverHappenException("");
        }
        node.setParent(this);
    }

    public Stream<DataPointNodeVO<?>> getChildren() {
        return (Stream<DataPointNodeVO<?>>)(Stream)Stream.concat((Stream<DataPointNodeRT>) (Stream)childFolders.stream(), (Stream<DataPointNodeRT>) (Stream)dataPoints.stream())
                .map(DataPointNodeRT::getVO);
    }

    public void addFolder(PointFolderRT childFolder) {
        childFolders.add(childFolder);
        childFolder.setParent(this);
    }

    public void addDataPoint(DataPointRT dataPoint) {
        dataPoints.add(dataPoint);
        dataPoint.setParent(this);
    }

    @Override
    public PointFolderRT getParent() {
        return parentFolder;
    }

    @Override
    public void setParent(PointFolderRT parent) {
        this.parentFolder = parent;
    }

    @Override
    public void wireEdgeAsSrc(RT<?> dest, EdgeType edgeType) throws WrongEdgeTypeException {
        switch (edgeType) {
            case TREE_PARENT_TO_CHILD:
                try {
                    addChild((DataPointNodeRT) dest);
                } catch (Throwable t) {
                    throw new WrongEdgeTypeException(this, this, dest, edgeType);
                }
        }
    }

    @Override
    public void wireEdgeAsDest(RT<?> src, EdgeType edgeType) throws WrongEdgeTypeException {
        switch (edgeType) {
            case TREE_PARENT_TO_CHILD:
                throw new ShouldNeverHappenException("Wirering is done by src");
            default:
                throw new WrongEdgeTypeException(this, src, this, edgeType);
        }
    }

    @Override
    public void iterateEdgesAsSrc(EdgeConsumer edgeIterator, EdgeType... edgeTypes) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void iterateEdgesAsDest(EdgeConsumer edgeIterator, EdgeType... edgeTypes) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public NodeType getNodeType() {
        return NodeType.POINT_FOLDER;
    }

    public void validateChildName(final String name) {

        childFolders.forEach((node) -> {
            if (name.equals(node.getName())) {
                throw new ValidationException();
            }
        });
        dataPoints.forEach((node) -> {
            if (name.equals(node.getName())) {
                throw new ValidationException();
            }
        });
    }

    public boolean removeChild(DataPointNodeRT node) {
        if (this != node.getParent()) {
            return false;
        } 
        node.setParent(null);
        switch (node.getNodeType()) {
            case DATA_POINT:
                return dataPoints.remove((DataPointRT)node);
            case POINT_FOLDER:
                return childFolders.remove((PointFolderRT)node);
            default:
                throw new ShouldNeverHappenException("");
        }
    }

}
