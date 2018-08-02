/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.serotonin.mango.rt.dataImage;

import br.org.scadabr.DataType;
import com.serotonin.mango.rt.dataImage.types.AlphanumericValue;
import com.serotonin.mango.rt.dataImage.types.MangoValue;

/**
 *
 * @author aploese
 */
public class AlphaNumericValueTime extends PointValueTime {

    private final String value; 

    public AlphaNumericValueTime(String value, int dataPointId, long timestamp) {
        super(dataPointId, timestamp);
        this.value = value;
    }
    
    @Override
    public String getValue() {
        return value;
    }

    @Override
    public DataType getDataType() {
        return DataType.ALPHANUMERIC;
    }

    @Override
    @Deprecated
    public AlphanumericValue toMangoValue() {
        return new AlphanumericValue(value);
    }
    
}
