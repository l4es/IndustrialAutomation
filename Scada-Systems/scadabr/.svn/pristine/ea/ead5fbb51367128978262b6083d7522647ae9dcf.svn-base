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
import com.serotonin.mango.rt.dataImage.AlphaNumericValueTime;

import com.serotonin.mango.rt.dataImage.IDataPoint;
import com.serotonin.mango.view.stats.ValueChangeCounter;

/**
 * @author Matthew Lohbihler
 */
public class AlphanumericPointWrapper extends AbstractPointWrapper<AlphaNumericValueTime> {

    public AlphanumericPointWrapper(IDataPoint<AlphaNumericValueTime> point, WrapperContext context) {
        super(point, context);
    }

    public String getValue() {
        AlphaNumericValueTime value = getValueImpl();
        if (value == null) {
            return "";
        }
        return value.getValue();
    }

    @Override
    public String toString() {
        return "{value=" + getValue() + ", ago(periodType, count)}";
    }

    public String ago(TimePeriods periodType) {
        return ago(periodType, 1);
    }

    public String ago(TimePeriods periodType, int count) {
        throw new ImplementMeException();
        /*
         long from = periodType.minus(context.getRuntime(), count);
         PointValueTime pvt = point.getPointValueBefore(from);
         if (pvt == null) {
         return null;
         }
         return pvt.getValue().getStringValue();
         */
    }

    public ValueChangeCounter past(TimePeriods periodType) {
        return past(periodType, 1);
    }

    public ValueChangeCounter past(TimePeriods periodType, int count) {
        long to = context.getRuntime();
        long from = periodType.minus(to, count);
        return getStats(from, to);
    }

    public ValueChangeCounter prev(TimePeriods periodType) {
        return previous(periodType, 1);
    }

    public ValueChangeCounter prev(TimePeriods periodType, int count) {
        return previous(periodType, count);
    }

    public ValueChangeCounter previous(TimePeriods periodType) {
        return previous(periodType, 1);
    }

    public ValueChangeCounter previous(TimePeriods periodType, int count) {
        long to = periodType.truncate(context.getRuntime());
        long from = periodType.minus(to, count);
        return getStats(from, to);
    }

    private ValueChangeCounter getStats(long from, long to) {
        throw new ImplementMeException();
        /*
         PointValueTime start = point.getPointValueBefore(from);
         List<PointValueTime> values = point.getPointValuesBetween(from, to);
         ValueChangeCounter stats = new ValueChangeCounter(start, values);
         return stats;
         */
    }
}
