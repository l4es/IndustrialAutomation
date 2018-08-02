/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package br.org.scadabr.web.mvc.controller.jsonrpc;

/**
 *
 * @author aploese
 */
public class XYDataSet {
    private final long x;
    private final double y;
    
    public XYDataSet(final long timestamp, final double value) {
        this.x = timestamp;
        this.y = value;
    }

    /**
     * @return the x
     */
    public long getX() {
        return x;
    }

    /**
     * @return the y
     */
    public double getY() {
        return y;
    }
}
