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
package com.serotonin.mango.rt.event.detectors;

import br.org.scadabr.utils.ImplementMeException;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.vo.event.DoublePointEventDetectorVO;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;

public class PointChangeDetectorRT extends PointEventDetectorRT<PointValueTime> {

    private MangoValue oldValue;
    private MangoValue newValue;

    public PointChangeDetectorRT(DoublePointEventDetectorVO vo) {
        this.vo = vo;
    }

    @Override
    protected LocalizableMessage getMessage() {
        return new LocalizableMessageImpl("event.detector.changeCount", vo.njbGetDataPoint().getName(),
                formatValue(oldValue), formatValue(newValue));
    }

    private String formatValue(MangoValue value) {
        throw new ImplementMeException();
        /* TODO half localized stuff
        return vo.njbGetDataPoint().getTextRenderer().getText(value, TextRenderer.HINT_SPECIFIC);
*/
    }

    @Override
    public void pointChanged(PointValueTime oldValue, PointValueTime newValue) {
        this.oldValue = oldValue.toMangoValue();
        this.newValue = newValue.toMangoValue();
        raiseAlarm(newValue.getTimestamp(), createEventContext());
    }

    @Override
    public boolean isEventActive() {
        return false;
    }
}
