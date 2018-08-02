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
import br.org.scadabr.util.StringUtils;

/**
 * @author Matthew Lohbihler
 */
public class AlphanumericValue extends MangoValue {

    private final String value;

    public AlphanumericValue(String value) {
        this.value = StringUtils.escapeLT_GT(value);
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
    public String toString() {
        return value;
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((value == null) ? 0 : value.hashCode());
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
        final AlphanumericValue other = (AlphanumericValue) obj;
        if (value == null) {
            if (other.value != null) {
                return false;
            }
        } else if (!value.equals(other.value)) {
            return false;
        }
        return true;
    }

    public int compareTo(AlphanumericValue that) {
        if (value == null || that.value == null) {
            return 0;
        }
        if (value == null) {
            return -1;
        }
        if (that.value == null) {
            return 1;
        }
        return value.compareTo(that.value);
    }

}
