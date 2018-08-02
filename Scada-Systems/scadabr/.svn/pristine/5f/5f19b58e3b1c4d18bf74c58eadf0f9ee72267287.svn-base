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
package com.serotonin.mango.rt.dataSource.galil;

import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.rt.dataSource.PointLocatorRT;
import br.org.scadabr.utils.i18n.LocalizableException;
import com.serotonin.mango.vo.dataSource.galil.GalilPointLocatorVO;
import com.serotonin.mango.vo.dataSource.galil.PointTypeVO;

/**
 * @author Matthew Lohbihler
 */
public class GalilPointLocatorRT extends PointLocatorRT<GalilPointLocatorVO> {

    private final PointTypeRT pointType;

    public GalilPointLocatorRT(GalilPointLocatorVO vo, PointTypeVO ptvo) {
        super(vo);
        this.pointType = ptvo.createRuntime();
    }

    public PointTypeRT getPointType() {
        return pointType;
    }

    @Override
    public boolean isSettable() {
        return pointType.isSettable();
    }

    public GalilRequest getPollRequest() {
        return pointType.getPollRequest();
    }

    public MangoValue parsePollResponse(String data, String pointName) throws LocalizableException {
        return pointType.parsePollResponse(data, pointName);
    }

    public GalilRequest getSetRequest(MangoValue value) {
        return pointType.getSetRequest(value);
    }

    public MangoValue parseSetResponse(String data) throws LocalizableException {
        return pointType.parseSetResponse(data);
    }
}
