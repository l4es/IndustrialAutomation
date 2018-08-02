/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.rt;

import br.org.scadabr.vo.EdgeType;

/**
 *
 * @author aploese
 */
public class WrongEdgeTypeException extends Exception {

    public WrongEdgeTypeException(RT<?> node, RT<?> src, RT<?> dest, EdgeType edgeType) {
        super("Can't wire \"" + edgeType + "\"nodeId: " + node.getId() + "  src:\n" + src + "\nwith dest:\n" + dest  );
    }
    
}
