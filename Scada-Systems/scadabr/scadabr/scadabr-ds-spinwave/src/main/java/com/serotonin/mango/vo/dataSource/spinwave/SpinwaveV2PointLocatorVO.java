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
package com.serotonin.mango.vo.dataSource.spinwave;

import br.org.scadabr.DataType;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

import br.org.scadabr.db.IntValuePair;

import com.serotonin.mango.rt.dataImage.types.BinaryValue;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.rt.dataImage.types.NumericValue;
import com.serotonin.mango.view.conversion.Conversions;
import com.serotonin.spinwave.SwMessage;
import com.serotonin.spinwave.v2.SensorValue;
import com.serotonin.spinwave.v2.SwMessageV2;

/**
 * @author Matthew Lohbihler
 */

public class SpinwaveV2PointLocatorVO extends BaseSpinwavePointLocatorVO {

    public static IntValuePair[] attributeTypes = {
        new IntValuePair(SensorValue.TYPE_TEMPERATURE, "dsEdit.spinwave.v2Attr.temp"),
        new IntValuePair(SensorValue.TYPE_SETPOINT, "dsEdit.spinwave.v2Attr.setPoint"),
        new IntValuePair(SensorValue.TYPE_BATTERY, "dsEdit.spinwave.v2Attr.battery"),
        new IntValuePair(SensorValue.TYPE_BATTERY_ALARM, "dsEdit.spinwave.v2Attr.batteryAlarm"),
        new IntValuePair(SensorValue.TYPE_OVERRIDE, "dsEdit.spinwave.v2Attr.override"),
        new IntValuePair(SensorValue.TYPE_HUMIDITY, "dsEdit.spinwave.v2Attr.humidity"),
        new IntValuePair(SensorValue.TYPE_VOLTAGE, "dsEdit.spinwave.v2Attr.voltage"),
        new IntValuePair(SensorValue.TYPE_AIRFLOW, "dsEdit.spinwave.v2Attr.airflow"),
        new IntValuePair(SensorValue.TYPE_KWH, "dsEdit.spinwave.v2Attr.kwhours"),
        new IntValuePair(SensorValue.TYPE_OCCUPANCY, "dsEdit.spinwave.v2Attr.occupancy"),
        new IntValuePair(SensorValue.TYPE_CO2, "dsEdit.spinwave.v2Attr.co2"),
        new IntValuePair(SensorValue.TYPE_VOC, "dsEdit.spinwave.v2Attr.voc"),
        new IntValuePair(SensorValue.TYPE_IAQ, "dsEdit.spinwave.v2Attr.iaq"),
        new IntValuePair(SensorValue.TYPE_CO, "dsEdit.spinwave.v2Attr.co"),
        new IntValuePair(SensorValue.TYPE_FREQUENCY, "dsEdit.spinwave.v2Attr.freq"),
        new IntValuePair(SensorValue.TYPE_PULSECOUNTER, "dsEdit.spinwave.v2Attr.counter"),};

    public static DataType getAttributeDataType(int attributeId) {
        if (attributeId == SensorValue.TYPE_BATTERY_ALARM || attributeId == SensorValue.TYPE_OVERRIDE
                || attributeId == SensorValue.TYPE_OCCUPANCY) {
            return DataType.BINARY;
        }
        return DataType.NUMERIC;
    }

    public static String getAttributeDescription(int attributeId) {
        for (IntValuePair attr : attributeTypes) {
            if (attributeId == attr.getKey()) {
                return attr.getValue();
            }
        }
        return "Unknown";
    }

    public IntValuePair[] getAttributeTypes() {
        return attributeTypes;
    }

    @Override
    public String getAttributeDescription() {
        return getAttributeDescription(getAttributeId());
    }

    public DataType getDataType() {
        return getAttributeDataType(getAttributeId());
    }

    @Override
    public MangoValue getValue(SwMessage msg) {
        SwMessageV2 message = (SwMessageV2) msg;
        SensorValue value = message.getValue(getAttributeId());

        if (value == null) {
            return null;
        }

        if (getAttributeDataType(getAttributeId()) == DataType.BINARY) {
            return new BinaryValue(value.getBinary());
        }

        if (isConvertToCelsius()) {
            return new NumericValue(Conversions.fahrenheitToCelsius(value.getNumeric()));
        }

        return new NumericValue(value.getNumeric());
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
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            // no op
        }
    }
}
