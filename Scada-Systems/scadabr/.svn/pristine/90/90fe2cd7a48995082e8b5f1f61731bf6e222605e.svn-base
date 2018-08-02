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
package com.serotonin.mango.view.stats;

import java.util.List;

import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import java.util.Objects;

/**
 * @author Matthew Lohbihler
 */
public class ValueChangeCounter implements StatisticsGenerator<PointValueTime> {

    // Calculated values.
    private int changes;

    // State values
    private MangoValue lastValue;

    public ValueChangeCounter(PointValueTime startValue, List<? extends PointValueTime> values) {
        this((MangoValue)(startValue == null ? null : startValue.toMangoValue()), values);
    }

    public ValueChangeCounter(MangoValue startValue, List<? extends PointValueTime> values) {
        this(startValue);
        for (PointValueTime p : values) {
            addValueTime(p);
        }
        done();
    }

    public ValueChangeCounter(MangoValue startValue) {
        lastValue = startValue;
    }

    @Override
    public void addValueTime(PointValueTime vt) {
        if (!Objects.equals(lastValue, vt.getValue())) {
            changes++;
            lastValue = vt.toMangoValue();
        }
    }

    @Override
    public void done() {
        // no op
    }

    public int getChangeCount() {
        return changes;
    }

    public String getHelp() {
        return toString();
    }

    @Override
    public String toString() {
        return "{changeCount: " + changes + "}";
    }

}
