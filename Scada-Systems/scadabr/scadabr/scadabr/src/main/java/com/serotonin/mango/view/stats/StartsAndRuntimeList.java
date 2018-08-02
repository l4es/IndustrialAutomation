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

import com.serotonin.mango.rt.dataImage.DoubleValueTime;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.serotonin.mango.rt.dataImage.types.DoubleValue;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import java.util.Objects;

/**
 * @author Matthew Lohbihler
 */
public class StartsAndRuntimeList implements StatisticsGenerator<DoubleValueTime> {

    private final List<StartsAndRuntime> data = new ArrayList<>();

    private final long end;
    private long lastTime = -1;
    private long realStart = -1;
    private DoubleValue lastValue;
    private StartsAndRuntime sar;

    public StartsAndRuntimeList(DoubleValueTime startValue, List<? extends DoubleValueTime> values, long start, long end) {
        this(startValue == null ? null : startValue.toMangoValue(), values, start, end);
    }

    public StartsAndRuntimeList(DoubleValue startValue, List<? extends DoubleValueTime> values, long start, long end) {
        this(startValue, start, end);
        for (DoubleValueTime vt : values) {
            addValueTime(vt);
        }
        done();
    }

    public StartsAndRuntimeList(DoubleValue startValue, long start, long end) {
        this.end = end;
        if (startValue != null) {
            lastTime = start;
            lastValue = startValue;
            sar = get(startValue);
        }
    }

    @Override
    public void addValueTime(DoubleValueTime vt) {
        if (lastTime == -1) {
            lastTime = vt.getTimestamp();
        }

        if (realStart == -1) {
            realStart = lastTime;
        }

        if (!Objects.equals(vt.getValue(), lastValue)) {
            // Update the last value stats, if any.
            if (sar != null) {
                sar.runtime += vt.getTimestamp()- lastTime;
            }

            lastValue = vt.toMangoValue();
            lastTime = vt.getTimestamp();

            sar = get(lastValue);
            sar.starts++;
        }
    }

    @Override
    public void done() {
        if (sar != null) {
            sar.runtime += end - lastTime;
        }

        if (realStart == -1) {
            realStart = lastTime;
        }

        // Calculate runtime percentages.
        for (StartsAndRuntime s : data) {
            s.calculateRuntimePercentage(end - realStart);
        }

        // Sort by value.
        Collections.sort(data, new Comparator<StartsAndRuntime>() {
            @Override
            public int compare(StartsAndRuntime o1, StartsAndRuntime o2) {
                return o1.value.compareTo(o2.value);
            }
        });
    }

    public long getEnd() {
        return end;
    }

    public long getRealStart() {
        return realStart;
    }

    public Map<Object, StartsAndRuntime> getStartsAndRuntime() {
        Map<Object, StartsAndRuntime> result = new HashMap<>();
        for (StartsAndRuntime sar : data) {
            result.put(sar.getValue(), sar);
        }
        return result;
    }

    public List<StartsAndRuntime> getData() {
        return data;
    }

    public StartsAndRuntime get(Object value) {
        return get(MangoValue.objectToValue(value));
    }

    public StartsAndRuntime get(MangoValue value) {
        for (StartsAndRuntime sar : data) {
            if (Objects.equals(sar.value, value)) {
                return sar;
            }
        }

        StartsAndRuntime sar = new StartsAndRuntime();
        sar.value = value;
        data.add(sar);

        return sar;
    }

    public String getHelp() {
        return toString();
    }

    @Override
    public String toString() {
        return "{realStart: " + realStart + ", end: " + end + ", data: " + data.toString() + "}";
    }

}
