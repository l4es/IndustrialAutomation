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
package com.serotonin.mango.vo.dataSource.http;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;


import br.org.scadabr.utils.TimePeriods;
import com.serotonin.mango.rt.dataSource.DataSourceRT;
import com.serotonin.mango.rt.dataSource.http.HttpImageDataSourceRT;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.datasource.http.HttpImageDataSourceEventKey;
import br.org.scadabr.vo.datasource.http.HttpImageDataSourceType;
import java.util.EnumMap;
import java.util.EnumSet;
import java.util.Map;
import java.util.Set;
import org.springframework.validation.Errors;

/**
 * @author Craig McFetridge
 * @author Matthew Lohbihler
 */

public class HttpImageDataSourceVO extends DataSourceVO<HttpImageDataSourceVO> {

    @Override
    public Set<HttpImageDataSourceEventKey> createEventKeySet() {
        return EnumSet.allOf(HttpImageDataSourceEventKey.class);
    }

    @Override
    public Map<HttpImageDataSourceEventKey, ?> createEventKeyMap() {
        return new EnumMap(HttpImageDataSourceEventKey.class);
    }

    /** TODO Validation
    public static class HttpImageDataSourceValidator extends DataSourceValidator {

        @Override
        public boolean supports(Class<?> clazz) {
            return HttpImageDataSourceVO.class.isAssignableFrom(clazz);
        }

        @Override
        public void validate(Object target, Errors errors) {
            super.validate(target, errors);
            final HttpImageDataSourceVO vo = (HttpImageDataSourceVO) target;
            if (vo.updatePeriods <= 0) {
                errors.rejectValue("updatePeriods", "validate.greaterThanZero");
            }
        }

    }
*/
    @Override
    public LocalizableMessage getConnectionDescription() {
        return new LocalizableMessageImpl("dsEdit.httpImage.dsconn", updatePeriodType.getPeriodDescription(updatePeriods));
    }

    @Override
    public String getDataSourceTypeKey() {
        return HttpImageDataSourceType.KEY;
    }

    @Override
    public int getDataSourceTypeId() {
        return HttpImageDataSourceType.DB_ID;
    }

    @Override
    public HttpImageDataSourceRT createRT() {
        return new HttpImageDataSourceRT(this);
    }

    private TimePeriods updatePeriodType = TimePeriods.MINUTES;
    
    private int updatePeriods = 5;

    public TimePeriods getUpdatePeriodType() {
        return updatePeriodType;
    }

    public void setUpdatePeriodType(TimePeriods updatePeriodType) {
        this.updatePeriodType = updatePeriodType;
    }

    public int getUpdatePeriods() {
        return updatePeriods;
    }

    public void setUpdatePeriods(int updatePeriods) {
        this.updatePeriods = updatePeriods;
    }

    @Override
    protected void addPropertiesImpl(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.updatePeriod", updatePeriodType.getPeriodDescription(updatePeriods));
    }

    @Override
    protected void addPropertyChangesImpl(List<LocalizableMessage> list, HttpImageDataSourceVO from) {
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.updatePeriod",
                from.updatePeriodType.getPeriodDescription(from.updatePeriods),
                updatePeriodType.getPeriodDescription(updatePeriods));
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
        out.writeInt(updatePeriodType.getId());
        out.writeInt(updatePeriods);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            updatePeriodType = TimePeriods.fromId(in.readInt());
            updatePeriods = in.readInt();
        }
    }

}
