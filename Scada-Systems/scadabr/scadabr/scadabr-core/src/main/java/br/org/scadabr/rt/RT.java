/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.rt;

import br.org.scadabr.vo.EdgeType;
import br.org.scadabr.vo.NodeType;
import br.org.scadabr.vo.VO;
import br.org.scadabr.vo.EdgeConsumer;

/**
 *
 * @author aploese
 * @param <T>
 */
public interface RT<T extends VO<T>> {

    int getId();
    
    String getName();

    T getVO();

    void wireEdgeAsSrc(RT<?> dest, EdgeType edgeType) throws WrongEdgeTypeException;

    void wireEdgeAsDest(RT<?> src, EdgeType edgeType) throws WrongEdgeTypeException;

    void iterateEdgesAsSrc(EdgeConsumer edgeIterator, EdgeType... edgeTypes);

    void iterateEdgesAsDest(EdgeConsumer edgeIterator, EdgeType... edgeTypes);

    /**
     * Iterate over all edges first this as src then this as dest
     * 
     * @param edgeIterator
     * @param edgeTypes 
     */
    default void iterateEdges(EdgeConsumer edgeIterator, EdgeType... edgeTypes) {
        iterateEdgesAsSrc((int src, int dest, EdgeType edgeType) -> {
            edgeIterator.accept(src, dest, edgeType);
        }, edgeTypes);
        iterateEdgesAsDest((int src, int dest, EdgeType edgeType) -> {
            edgeIterator.accept(src, dest, edgeType);
        }, edgeTypes);
    }

    NodeType getNodeType();
    
    void patch(T vo);

}
