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

import br.org.scadabr.utils.TimePeriods;

/**
 * @author Matthew Lohbihler
 */
public class WrapperContext {

    private final long runtime;

    public WrapperContext(long runtime) {
        this.runtime = runtime;
    }

    public long getRuntime() {
        return runtime;
    }

    public long millisInPrev(TimePeriods periodType) {
        return millisInPrevious(periodType, 1);
    }

    public long millisInPrevious(TimePeriods periodType) {
        return millisInPrevious(periodType, 1);
    }

    public long millisInPrev(TimePeriods periodType, int count) {
        return millisInPrevious(periodType, count);
    }

    public long millisInPrevious(TimePeriods periodType, int count) {
        long to = periodType.truncate(runtime);
        long from = periodType.minus(to, count);
        return to - from;
    }

    public long millisInPast(TimePeriods periodType) {
        return millisInPast(periodType, 1);
    }

    public long millisInPast(TimePeriods periodType, int count) {
        long from = periodType.minus(runtime, count);
        return runtime - from;
    }

    @Override
    public String toString() {
        return "{millisInPast(periodType, count), millisInPrev(periodType, count), "
                + "millisInPrevious(periodType, count)}";
    }

    public String getHelp() {
        return toString();
    }
}
