/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.serotonin.mango.rt.dataImage;

import br.org.scadabr.DataType;
import com.serotonin.mango.rt.dataImage.types.BooleanValue;
import com.serotonin.mango.rt.dataImage.types.MangoValue;

/**
 *
 * @author aploese
 */
public class BooleanValueTime extends PointValueTime {

    private final boolean value; 

    public BooleanValueTime(boolean value, int dataPointId, long timestamp) {
        super(dataPointId, timestamp);
        this.value = value;
    }
    
    public boolean getBooleanValue() {
        return value;
    }

    @Override
    public Boolean getValue() {
        return value;
    }

    @Override
    public DataType getDataType() {
        return DataType.BOOLEAN;
    }

    @Override
    public BooleanValue toMangoValue() {
        return new BooleanValue(value);
    }
    
}
