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
package com.serotonin.mango.rt.dataSource.virtual;

import com.serotonin.mango.rt.dataImage.types.AlphanumericValue;
import com.serotonin.mango.rt.dataImage.types.BinaryValue;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.rt.dataImage.types.MultistateValue;
import com.serotonin.mango.rt.dataImage.types.NumericValue;
import com.serotonin.mango.rt.dataSource.PointLocatorRT;
import com.serotonin.mango.vo.dataSource.virtual.VirtualPointLocatorVO;

public class VirtualPointLocatorRT extends PointLocatorRT<VirtualPointLocatorVO> {

    private final ChangeTypeRT changeType;
    private MangoValue currentValue;

    public VirtualPointLocatorRT(VirtualPointLocatorVO vo) {
        super(vo);
        changeType = vo.getChangeType().createRuntime();
        String startValue = vo.getChangeType().getStartValue();
        switch (vo.getDataType()) {
            case BINARY:
                currentValue = BinaryValue.parseBinary(startValue);
                break;
            case MULTISTATE:
                try {
                    currentValue = MultistateValue.parseMultistate(startValue);
                } catch (NumberFormatException e) {
                    currentValue = new MultistateValue(0);
                }
                break;
            case NUMERIC:
                try {
                    currentValue = NumericValue.parseNumeric(startValue);
                } catch (NumberFormatException e) {
                    currentValue = new NumericValue(0);
                }
                break;
            default:
                if (startValue == null) {
                    currentValue = new AlphanumericValue("");
                } else {
                    currentValue = new AlphanumericValue(startValue);
                }
        }
    }

    public ChangeTypeRT getChangeType() {
        return changeType;
    }

    public MangoValue getCurrentValue() {
        return currentValue;
    }

    public void setCurrentValue(MangoValue currentValue) {
        this.currentValue = currentValue;
    }

    public void change() {
        currentValue = changeType.change(currentValue);
    }

}
