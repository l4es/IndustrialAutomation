/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.serotonin.mango.rt.dataImage;

import br.org.scadabr.DataType;
import com.serotonin.mango.rt.dataImage.types.DoubleValue;

/**
 *
 * @author aploese
 */
public class DoubleValueTime extends NumberValueTime {

    private final double value; 

    public DoubleValueTime(double value, int dataPointId, long timestamp) {
        super(dataPointId, timestamp);
        this.value = value;
    }
    
    public double getDoubleValue() {
        return value;
    }

    @Override
    public Double getValue() {
        return value;
    }

    @Override
    public DataType getDataType() {
        return DataType.DOUBLE;
    }

    @Override
    public DoubleValue toMangoValue() {
        return new DoubleValue(value);
    }
    
}
