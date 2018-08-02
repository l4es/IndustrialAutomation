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
import br.org.scadabr.utils.ImplementMeException;

/**
 * @author Matthew Lohbihler
 */
abstract public class MangoValue implements Comparable<MangoValue> {

    @Deprecated
    public static MangoValue stringToValue(String valueStr, DataType dataType) {
        throw new ImplementMeException();
        /*
        switch (dataType) {
            case BINARY:
                return BooleanValue.parseBinary(valueStr);
            case MULTISTATE:
                return MultistateValue.parseMultistate(valueStr);
            case NUMERIC:
                return NumericValue.parseNumeric(valueStr);
            case IMAGE:
                try {
                    return new ImageValue(valueStr);
                } catch (InvalidArgumentException e) {
                    // no op
                }
                return null;
            case ALPHANUMERIC:
                return new AlphanumericValue(valueStr);
        }
        throw new ShouldNeverHappenException("Invalid data type " + dataType + ". Cannot instantiate MangoValue");
                */
    }

    @Deprecated
    public static MangoValue objectToValue(Object value) {
        throw new ImplementMeException();
        /*
        if (value instanceof Boolean) {
            return new BooleanValue((Boolean) value);
        }
        if (value instanceof Integer) {
            return new MultistateValue((Integer) value);
        }
        if (value instanceof Double) {
            return new NumericValue((Double) value);
        }
        if (value instanceof String) {
            return new AlphanumericValue((String) value);
        }
        throw new ShouldNeverHappenException("Unrecognized object type " + (value == null ? "null" : value.getClass())
                + ". Cannot instantiate MangoValue");
                */
    }

    abstract public Object getValue();

    abstract public DataType getDataType();

    @Override
    public int compareTo(MangoValue that) {
        if (getValue() == null || that.getValue() == null) {
            return 0;
        }
        if (getValue() == null) {
            return -1;
        }
        if (that.getValue() == null) {
            return 1;
        }
        return ((Comparable)getValue()).compareTo(that.getValue());
    }
}
