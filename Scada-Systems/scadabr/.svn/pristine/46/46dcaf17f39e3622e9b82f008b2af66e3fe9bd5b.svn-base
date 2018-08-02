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
package com.serotonin.mango.view.chart;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

import br.org.scadabr.utils.TimePeriods;

abstract public class TimePeriodChartRenderer extends BaseChartRenderer {

    private TimePeriods timePeriod;
    
    private int numberOfPeriods;

    /**
     * Convenience method for getting the start time of the chart period.
     */
    public long getStartTime(long timestamp) {
        return timestamp - getDuration();
    }

    /**
     * Convenience method for getting the start time of the chart period.
     */
    public long getEndTime(long timestamp) {
        return timestamp;
    }

    /**
     * Convenience method for getting the duration of the chart period.
     */
    public long getDuration() {
        return timePeriod.getMillis(numberOfPeriods);
    }

    public TimePeriodChartRenderer() {
        // no op
    }

    public TimePeriodChartRenderer(TimePeriods timePeriod, int numberOfPeriods) {
        this.timePeriod = timePeriod;
        this.numberOfPeriods = numberOfPeriods;
    }

    public int getNumberOfPeriods() {
        return numberOfPeriods;
    }

    public void setNumberOfPeriods(int numberOfPeriods) {
        this.numberOfPeriods = numberOfPeriods;
    }

    public TimePeriods getTimePeriod() {
        return timePeriod;
    }

    public void setTimePeriod(TimePeriods timePeriod) {
        this.timePeriod = timePeriod;
    }

    //
    // /
    // / Serialization
    // /
    //
    private static final long serialVersionUID = -1;
    private static final int version = 1;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(version);
        out.writeInt(timePeriod.getId());
        out.writeInt(numberOfPeriods);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            timePeriod = timePeriod.fromId(in.readInt());
            numberOfPeriods = in.readInt();
        }
    }

}
