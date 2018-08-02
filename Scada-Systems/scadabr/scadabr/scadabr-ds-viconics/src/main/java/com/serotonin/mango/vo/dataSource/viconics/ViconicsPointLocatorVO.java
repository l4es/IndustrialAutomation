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
package com.serotonin.mango.vo.dataSource.viconics;

import br.org.scadabr.DataType;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;


import com.serotonin.mango.rt.dataSource.PointLocatorRT;
import com.serotonin.mango.rt.dataSource.viconics.ViconicsPointLocatorRT;
import com.serotonin.mango.vo.dataSource.AbstractPointLocatorVO;
import com.serotonin.viconics.config.StatPoint;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;

/**
 * @author Matthew Lohbihler
 */

public class ViconicsPointLocatorVO extends AbstractPointLocatorVO {

    @Override
    public PointLocatorRT createRuntime() {
        return new ViconicsPointLocatorRT(this);
    }

    @Override
    public LocalizableMessage getConfigurationDescription() {
        return new LocalizableMessageImpl("common.default", StatPoint.addressToString(pointAddress) + "@"
                + deviceCommAddress);
    }

    private byte[] deviceIeee;
    private int deviceCommAddress;
    private int pointAddress;
    private DataType dataType;
    private boolean settable;

    public byte[] getDeviceIeee() {
        return deviceIeee;
    }

    public void setDeviceIeee(byte[] deviceIeee) {
        this.deviceIeee = deviceIeee;
    }

    public int getDeviceCommAddress() {
        return deviceCommAddress;
    }

    public void setDeviceCommAddress(int deviceCommAddress) {
        this.deviceCommAddress = deviceCommAddress;
    }

    public int getPointAddress() {
        return pointAddress;
    }

    public void setPointAddress(int pointAddress) {
        this.pointAddress = pointAddress;
    }

    @Override
    public DataType getDataType() {
        return dataType;
    }

    public void setDataType(DataType dataType) {
        this.dataType = dataType;
    }

    @Override
    public boolean isSettable() {
        return settable;
    }

    public void setSettable(boolean settable) {
        this.settable = settable;
    }

    @Override
    public void validate(DwrResponseI18n response) {
        // Nothing to change; nothing to validate
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        // no op
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, Object o) {
        // no op
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
        out.writeObject(deviceIeee);
        out.writeInt(deviceCommAddress);
        out.writeInt(pointAddress);
        out.writeInt(dataType.mangoDbId);
        out.writeBoolean(settable);
    }

    private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            deviceIeee = (byte[]) in.readObject();
            deviceCommAddress = in.readInt();
            pointAddress = in.readInt();
            dataType = DataType.fromMangoDbId(in.readInt());
            settable = in.readBoolean();
        }
    }
}
