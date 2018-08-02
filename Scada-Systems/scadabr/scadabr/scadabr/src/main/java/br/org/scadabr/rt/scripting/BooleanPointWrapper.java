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

import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.utils.TimePeriods;
import com.serotonin.mango.rt.dataImage.BooleanValueTime;
import com.serotonin.mango.rt.dataImage.IDataPoint;
import com.serotonin.mango.rt.dataImage.types.BooleanValue;

/**
 * @author Matthew Lohbihler
 */
public class BooleanPointWrapper extends DistinctPointWrapper<BooleanValueTime> {

    public BooleanPointWrapper(IDataPoint<BooleanValueTime> point, WrapperContext context) {
        super(point, context);
    }

    public boolean getValue() {
        BooleanValueTime value = getValueImpl();
        if (value == null) {
            return false;
        }
        return value.getBooleanValue();
    }

    @Override
    public String toString() {
        return "{value="
                + getValue()
                + ", ago(periodType, count), past(periodType, count), prev(periodType, count), "
                + "previous(periodType, count)}";
    }

    public boolean ago(TimePeriods periodType) {
        return ago(periodType, 1);
    }

    public boolean ago(TimePeriods periodType, int count) {
        throw new ImplementMeException();
        /*
         long from = periodType.minus(context.getRuntime(), count);
         PointValueTime pvt = point.getPointValueBefore(from);
         if (pvt == null) {
         return false;
         }
         return pvt.getBooleanValue();
         */
    }
}
