/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.serotonin.mango.rt.dataImage.types;

import br.org.scadabr.DataType;

/**
 *
 * @author aploese
 */
public class DoubleValue extends NumericValue {

    private final double value;

    public DoubleValue(double value) {
        this.value = value;
    }
    
    @Override
    public double getDoubleValue() {
        return value;
    }

    @Override
    public float getFloatValue() {
        return (float) value;
    }

    @Override
    public Double getValue() {
        return value;
    }

    @Override
    public byte getByteValue() {
        return (byte) value;
    }

    @Override
    public short getShortValue() {
        return (short) value;
    }

    @Override
    public int getIntegerValue() {
        return (int) value;
    }

    @Override
    public long getLongValue() {
        return (long) value;
    }

    @Override
    public DataType getDataType() {
        return DataType.DOUBLE;
    }

    @Override
    public String toString() {
        return Double.toString(value);
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        long temp;
        temp = Double.doubleToLongBits(value);
        result = prime * result + (int) (temp ^ (temp >>> 32));
        return result;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final DoubleValue other = (DoubleValue) obj;
        return Double.doubleToLongBits(value) == Double.doubleToLongBits(other.value);
    }

}
