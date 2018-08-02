/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.serotonin.mango.rt.dataImage;

import br.org.scadabr.DataType;
import com.serotonin.mango.rt.dataImage.types.ImageValue;

/**
 *
 * @author aploese
 */
public class ImageValueTime extends PointValueTime {

    private final byte[] value; 

    public ImageValueTime(byte[] value, int dataPointId, long timestamp) {
        super(dataPointId, timestamp);
        this.value = value;
    }

    public ImageValueTime(byte[] data, int type, int id, long time) {
        super(id, time);
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }
    
    @Override
    public byte[] getValue() {
        return value;
    }

    @Override
    public DataType getDataType() {
        return DataType.IMAGE;
    }

    @Override
    public ImageValue toMangoValue() {
        return new ImageValue(value);
    }

    public Object getTypeExtension() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }
    
}
