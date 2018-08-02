/*
 Mango - Open Source M2M - http://mango.serotoninsoftware.com
 Copyright (C) 2006-2011 Serotonin Software Technologies Inc.
 @author Matthew Lohbihler
    
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
package com.serotonin.mango.rt.dataImage.types;

import br.org.scadabr.DataType;

/**
 * @author Matthew Lohbihler
 */
public class MultistateValue extends MangoValue {

    public static MultistateValue parseMultistate(String s) {
        if (s == null) {
            return new MultistateValue((byte)0);
        }
        try {
            return new MultistateValue((byte)Short.parseShort(s));
        } catch (NumberFormatException e) {
            // no op
        }
        return new MultistateValue((byte)0);
    }

    private final byte value;

    public MultistateValue(byte value) {
        this.value = value;
    }

    @Override
    public Byte getValue() {
        return value;
    }

    public byte getByteValue() {
        return (byte) value;
    }

    @Override
    public DataType getDataType() {
        return DataType.MULTISTATE;
    }

    @Override
    public String toString() {
        return Integer.toString(value);
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + value;
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
        final MultistateValue other = (MultistateValue) obj;
        if (value != other.value) {
            return false;
        }
        return true;
    }

}
