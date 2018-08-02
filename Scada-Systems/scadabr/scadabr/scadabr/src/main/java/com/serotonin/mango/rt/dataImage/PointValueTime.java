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
package com.serotonin.mango.rt.dataImage;

import br.org.scadabr.DataType;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import java.io.Serializable;

import java.text.MessageFormat;
import java.util.Date;
import java.util.Objects;

/**
 * The simple value of a point at a given time.
 *
 * @see AnnotatedPointValueTime
 * @author Matthew Lohbihler
 */
public abstract class PointValueTime  implements Serializable {

    private static final long serialVersionUID = -1;

    static boolean equalValues(PointValueTime oldValue, PointValueTime newValue) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    public static PointValueTime fromMangoValue(MangoValue value, int targetPointId, long fireTimestamp) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    private final long timestamp;
    private final int dataPointId;

    public PointValueTime(int dataPointId, long timestamp) {
        this.dataPointId = dataPointId;
        this.timestamp = timestamp;
    }

    public long getTimestamp() {
        return timestamp;
    }

    public abstract Object getValue();

    @Override
    public boolean equals(Object o) {
        if (!(o instanceof PointValueTime)) {
            return false;
        }
        PointValueTime that = (PointValueTime) o;
        if (timestamp != that.timestamp) {
            return false;
        }
        return Objects.equals(getValue(), that.getValue());
    }

    @Override
    public int hashCode() {
        int hash = 5;
        hash = 29 * hash + Objects.hashCode(this.getValue());
        hash = 29 * hash + (int) (this.timestamp ^ (this.timestamp >>> 32));
        return hash;
    }

    @Override
    public String toString() {
        return MessageFormat.format("PointValueTime( {0} @{1})", getValue(), new Date(timestamp));
    }

    public abstract DataType getDataType();
    
    public int getDataPointId() {
        return dataPointId;
    }

    @Deprecated
    public abstract MangoValue toMangoValue();
    
}
