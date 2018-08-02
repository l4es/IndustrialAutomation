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
package br.org.scadabr.rt.scripting;


import com.serotonin.mango.rt.dataImage.IDataPoint;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.types.MangoValue;

/**
 * @author Matthew Lohbihler
 */
abstract public class AbstractPointWrapper<T extends PointValueTime> {

    protected IDataPoint<T> point;
    protected WrapperContext context;

    AbstractPointWrapper(IDataPoint point, WrapperContext context) {
        this.point = point;
        this.context = context;
    }

    public T getValueImpl() {
        return point.getPointValue();
    }

    public long getTimestamp() {
        PointValueTime pvt = point.getPointValue();
        if (pvt == null) {
            return -1;
        }
        return pvt.getTimestamp();
    }

    public String getHelp() {
        return toString();
    }
}
