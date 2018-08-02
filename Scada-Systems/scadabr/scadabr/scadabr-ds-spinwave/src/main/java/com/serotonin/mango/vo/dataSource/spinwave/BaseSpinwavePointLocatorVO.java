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

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;


import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.rt.dataSource.PointLocatorRT;
import com.serotonin.mango.rt.dataSource.spinwave.SpinwavePointLocatorRT;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.vo.dataSource.AbstractPointLocatorVO;
import com.serotonin.spinwave.SwMessage;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;

/**
 * @author Matthew Lohbihler
 */
abstract public class BaseSpinwavePointLocatorVO extends AbstractPointLocatorVO {

    @Override
    public boolean isSettable() {
        return false;
    }

    @Override
    public PointLocatorRT createRuntime() {
        return new SpinwavePointLocatorRT(this);
    }

    @Override
    public LocalizableMessage getConfigurationDescription() {
        return new LocalizableMessageImpl("dsEdit.spinwave.dpconn", sensorAddress, new LocalizableMessageImpl(
                getAttributeDescription()));
    }

    abstract public MangoValue getValue(SwMessage msg);

    abstract public String getAttributeDescription();

    
    private int sensorAddress;
    
    private int attributeId;
    
    private boolean convertToCelsius;

    public int getAttributeId() {
        return attributeId;
    }

    public void setAttributeId(int attributeId) {
        this.attributeId = attributeId;
    }

    public boolean isConvertToCelsius() {
        return convertToCelsius;
    }

    public void setConvertToCelsius(boolean convertToCelsius) {
        this.convertToCelsius = convertToCelsius;
    }

    public int getSensorAddress() {
        return sensorAddress;
    }

    public void setSensorAddress(int sensorAddress) {
        this.sensorAddress = sensorAddress;
    }

    @Override
    public void validate(DwrResponseI18n response) {
        if (sensorAddress <= 1) {
            response.addContextual("sensorAddress", "validate.required");
        }
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.spinwave.sensorAddress", sensorAddress);
        AuditEventType.addPropertyMessage(list, "dsEdit.spinwave.attribute", attributeId);
        AuditEventType.addPropertyMessage(list, "dsEdit.spinwave.convert", convertToCelsius);
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, Object o) {
        BaseSpinwavePointLocatorVO from = (BaseSpinwavePointLocatorVO) o;
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.spinwave.sensorAddress", from.sensorAddress,
                sensorAddress);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.spinwave.attribute", from.attributeId, attributeId);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.spinwave.convert", from.convertToCelsius,
                convertToCelsius);
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
        out.writeInt(sensorAddress);
        out.writeInt(attributeId);
        out.writeBoolean(convertToCelsius);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            sensorAddress = in.readInt();
            attributeId = in.readInt();
            convertToCelsius = in.readBoolean();
        }
    }
}
