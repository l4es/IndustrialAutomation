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


import com.serotonin.mango.rt.dataImage.types.BinaryValue;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.rt.dataImage.types.NumericValue;
import com.serotonin.mango.view.conversion.Conversions;
import com.serotonin.spinwave.SwMessage;
import com.serotonin.spinwave.v1.SwMessageV1;

/**
 * @author Matthew Lohbihler
 */

public class SpinwaveV1PointLocatorVO extends BaseSpinwavePointLocatorVO {

    public interface AttributeTypes {

        int TEMPURATURE = 1;
        int SET_POINT = 2;
        int BATTERY = 3;
        int OVERRIDE = 4;
    }

    public static String getAttributeDescription(int attributeId) {
        if (attributeId == AttributeTypes.TEMPURATURE) {
            return "dsEdit.spinwave.v1Attr.temp";
        }
        if (attributeId == AttributeTypes.SET_POINT) {
            return "dsEdit.spinwave.v1Attr.setPoint";
        }
        if (attributeId == AttributeTypes.BATTERY) {
            return "dsEdit.spinwave.v1Attr.battery";
        }
        if (attributeId == AttributeTypes.OVERRIDE) {
            return "dsEdit.spinwave.v1Attr.override";
        }
        return "Unknown";
    }

    public static DataType getAttributeDataType(int attributeId) {
        if (attributeId == AttributeTypes.OVERRIDE) {
            return DataType.BINARY;
        }
        return DataType.NUMERIC;
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
        SwMessageV1 message = (SwMessageV1) msg;

        if (getAttributeId() == AttributeTypes.TEMPURATURE) {
            if (isConvertToCelsius()) {
                return new NumericValue(Conversions.fahrenheitToCelsius(message.getTemperature()));
            }
            return new NumericValue(message.getTemperature());
        }

        if (getAttributeId() == AttributeTypes.SET_POINT) {
            if (isConvertToCelsius()) {
                return new NumericValue(Conversions.fahrenheitToCelsius(message.getSetPoint()));
            }
            return new NumericValue(message.getSetPoint());
        }

        if (getAttributeId() == AttributeTypes.BATTERY) {
            return new NumericValue(message.getBatteryVoltage());
        }

        if (getAttributeId() == AttributeTypes.OVERRIDE) {
            return new BinaryValue(message.isOverride());
        }

        return null;
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
