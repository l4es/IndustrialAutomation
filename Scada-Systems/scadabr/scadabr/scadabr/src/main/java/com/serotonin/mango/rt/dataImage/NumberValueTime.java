/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.serotonin.mango.rt.dataImage;

/**
 *
 * @author aploese
 */
public abstract class NumberValueTime extends PointValueTime {

    public NumberValueTime(int dataPointId, long timestamp) {
        super(dataPointId, timestamp);
    }
    
    @Override
    public abstract Number getValue();

}
