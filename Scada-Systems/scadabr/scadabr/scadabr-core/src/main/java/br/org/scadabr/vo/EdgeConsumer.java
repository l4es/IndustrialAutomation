/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo;

import java.util.Objects;

/**
 *
 * @author aploese
 */
public interface EdgeConsumer {
    
    void accept(int src, int dest, EdgeType edgeType);
    
    default EdgeConsumer andThen(EdgeConsumer after) {
        Objects.requireNonNull(after);

        return (src, dest, edgeType) -> {
            accept(src, dest, edgeType);
            after.accept(src, dest, edgeType);
        };
    }
}
