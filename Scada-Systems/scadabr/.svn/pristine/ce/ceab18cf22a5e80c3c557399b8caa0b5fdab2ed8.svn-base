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
package com.serotonin.mango.vo.dataSource.galil;

import br.org.scadabr.DataType;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;



import com.serotonin.mango.rt.dataSource.galil.PointTypeRT;
import com.serotonin.mango.rt.dataSource.galil.TellPositionPointTypeRT;
import com.serotonin.mango.rt.event.type.AuditEventType;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;

/**
 * @author Matthew Lohbihler
 */

public class TellPositionPointTypeVO extends PointTypeVO {

    
    private String axis;
    
    private double scaleRawLow = 0;
    
    private double scaleRawHigh = 1;
    
    private double scaleEngLow = 0;
    
    private double scaleEngHigh = 1;
    
    private boolean roundToInteger;

    @Override
    public PointTypeRT createRuntime() {
        return new TellPositionPointTypeRT(this);
    }

    @Override
    public int typeId() {
        return Types.TELL_POSITION;
    }

    @Override
    public DataType getDataType() {
        return DataType.NUMERIC;
    }

    @Override
    public LocalizableMessage getDescription() {
        return new LocalizableMessageImpl("dsEdit.galil.pointType.tellPosition");
    }

    @Override
    public boolean isSettable() {
        return false;
    }

    @Override
    public void validate(DwrResponseI18n response) {
        if (!"A".equals(axis) && !"B".equals(axis) && !"C".equals(axis) && !"D".equals(axis) && !"E".equals(axis)
                && !"F".equals(axis) && !"G".equals(axis) && !"H".equals(axis)) {
            response.addContextual("tellPositionPointType.axis", "validate.axis.invalid");
        }
        if (scaleRawHigh <= scaleRawLow) {
            response.addContextual("tellPositionPointType.scaleRawHighId", "validate.greaterThanRawLow");
        }
        if (scaleEngHigh <= scaleEngLow) {
            response.addContextual("tellPositionPointType.scaleEngHighId", "validate.greaterThanEngLow");
        }
    }

    public void setAxis(String axis) {
        this.axis = axis;
    }

    public String getAxis() {
        return axis;
    }

    public double getScaleRawLow() {
        return scaleRawLow;
    }

    public void setScaleRawLow(double scaleRawLow) {
        this.scaleRawLow = scaleRawLow;
    }

    public double getScaleRawHigh() {
        return scaleRawHigh;
    }

    public void setScaleRawHigh(double scaleRawHigh) {
        this.scaleRawHigh = scaleRawHigh;
    }

    public double getScaleEngLow() {
        return scaleEngLow;
    }

    public void setScaleEngLow(double scaleEngLow) {
        this.scaleEngLow = scaleEngLow;
    }

    public double getScaleEngHigh() {
        return scaleEngHigh;
    }

    public void setScaleEngHigh(double scaleEngHigh) {
        this.scaleEngHigh = scaleEngHigh;
    }

    public boolean isRoundToInteger() {
        return roundToInteger;
    }

    public void setRoundToInteger(boolean roundToInteger) {
        this.roundToInteger = roundToInteger;
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.galil.axis", axis);
        AuditEventType.addPropertyMessage(list, "dsEdit.galil.scaleLow", scaleRawLow);
        AuditEventType.addPropertyMessage(list, "dsEdit.galil.scaleHigh", scaleRawHigh);
        AuditEventType.addPropertyMessage(list, "dsEdit.galil.engLow", scaleEngLow);
        AuditEventType.addPropertyMessage(list, "dsEdit.galil.engHigh", scaleEngHigh);
        AuditEventType.addPropertyMessage(list, "dsEdit.galil.round", roundToInteger);
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, Object o) {
        TellPositionPointTypeVO from = (TellPositionPointTypeVO) o;
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.galil.axis", from.axis, axis);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.galil.scaleLow", from.scaleRawLow, scaleRawLow);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.galil.scaleHigh", from.scaleRawHigh, scaleRawHigh);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.galil.engLow", from.scaleEngLow, scaleEngLow);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.galil.engHigh", from.scaleEngHigh, scaleEngHigh);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.galil.round", from.roundToInteger, roundToInteger);
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
        SerializationHelper.writeSafeUTF(out, axis);
        out.writeDouble(scaleRawLow);
        out.writeDouble(scaleRawHigh);
        out.writeDouble(scaleEngLow);
        out.writeDouble(scaleEngHigh);
        out.writeBoolean(roundToInteger);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            axis = SerializationHelper.readSafeUTF(in);
            scaleRawLow = in.readDouble();
            scaleRawHigh = in.readDouble();
            scaleEngLow = in.readDouble();
            scaleEngHigh = in.readDouble();
            roundToInteger = in.readBoolean();
        }
    }
}
