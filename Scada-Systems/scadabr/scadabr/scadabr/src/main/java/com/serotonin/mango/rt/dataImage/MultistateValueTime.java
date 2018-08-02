/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.serotonin.mango.rt.dataImage;

import br.org.scadabr.DataType;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.rt.dataImage.types.MultistateValue;

/**
 *
 * @author aploese
 */
public class MultistateValueTime extends NumberValueTime {

    private final byte value; 

    public MultistateValueTime(byte value, int dataPointId, long timestamp) {
        super(dataPointId, timestamp);
        this.value = value;
    }
    
    public byte getByteValue() {
        return value;
    }

    @Override
    public Byte getValue() {
        return value;
    }

    @Override
    public DataType getDataType() {
        return DataType.MULTISTATE;
    }

    @Override
    @Deprecated
    public MultistateValue toMangoValue() {
        return new MultistateValue(value);
    }
    
}
