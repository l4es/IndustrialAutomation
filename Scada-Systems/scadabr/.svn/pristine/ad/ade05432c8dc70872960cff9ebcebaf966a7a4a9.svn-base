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
import java.util.Map;

import com.serotonin.mango.rt.dataSource.DataSourceRT;
import com.serotonin.mango.rt.dataSource.http.HttpRetrieverDataSourceRT;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.util.StringUtils;
import br.org.scadabr.utils.TimePeriods;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.datasource.http.HttpRetrieverDataSourceEventKey;
import br.org.scadabr.vo.datasource.http.HttpRetrieverDataSourceType;
import java.util.EnumMap;
import java.util.EnumSet;
import java.util.Set;
import org.springframework.validation.Errors;

/**
 * @author Matthew Lohbihler
 */

public class HttpRetrieverDataSourceVO extends DataSourceVO<HttpRetrieverDataSourceVO> {

    @Override
    public Set<HttpRetrieverDataSourceEventKey> createEventKeySet() {
        return EnumSet.allOf(HttpRetrieverDataSourceEventKey.class);
    }

    @Override
    public Map<HttpRetrieverDataSourceEventKey, ?> createEventKeyMap() {
        return new EnumMap(HttpRetrieverDataSourceEventKey.class);
    }

    /*TODO Validation
    public static class HttpRetrieverDataSourceVOValidator extends DataSourceValidator {

        @Override
        public boolean supports(Class<?> clazz) {
            return HttpRetrieverDataSourceVO.class.isAssignableFrom(clazz);
        }

        @Override
        public void validate(Object target, Errors errors) {
            super.validate(target, errors);
            final HttpRetrieverDataSourceVO vo = (HttpRetrieverDataSourceVO) target;
            if (vo.url.isEmpty()) {
                errors.rejectValue("url", "validate.required");
            }
            if (vo.updatePeriods <= 0) {
                errors.rejectValue("updatePeriods", "validate.greaterThanZero");
            }
            if (vo.timeoutSeconds <= 0) {
                errors.rejectValue("updatePeriods", "validate.greaterThanZero");
            }
            if (vo.retries < 0) {
                errors.rejectValue("retries", "validate.cannotBeNegative");
            }
        }

    }
*/
    @Override
    public LocalizableMessage getConnectionDescription() {
        return new LocalizableMessageImpl("common.default", StringUtils.truncate(url, 30, " ..."));
    }


    @Override
    public String getDataSourceTypeKey() {
        return HttpRetrieverDataSourceType.KEY;
    }

    @Override
    public int getDataSourceTypeId() {
        return HttpRetrieverDataSourceType.DB_ID;
    }

    @Override
    public HttpRetrieverDataSourceRT createRT() {
        return new HttpRetrieverDataSourceRT(this);
    }

    private String url;
    private TimePeriods updatePeriodType = TimePeriods.MINUTES;
    
    private int updatePeriods = 5;
    
    private int timeoutSeconds = 30;
    
    private int retries = 2;

    public String getUrl() {
        return url;
    }

    public void setUrl(String url) {
        this.url = url;
    }

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

    public int getTimeoutSeconds() {
        return timeoutSeconds;
    }

    public void setTimeoutSeconds(int timeoutSeconds) {
        this.timeoutSeconds = timeoutSeconds;
    }

    public int getRetries() {
        return retries;
    }

    public void setRetries(int retries) {
        this.retries = retries;
    }

    @Override
    protected void addPropertiesImpl(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.updatePeriod", updatePeriodType.getPeriodDescription(updatePeriods));
        AuditEventType.addPropertyMessage(list, "dsEdit.httpRetriever.url", url);
        AuditEventType.addPropertyMessage(list, "dsEdit.httpRetriever.timeout", timeoutSeconds);
        AuditEventType.addPropertyMessage(list, "dsEdit.httpRetriever.retries", retries);
    }

    @Override
    protected void addPropertyChangesImpl(List<LocalizableMessage> list, HttpRetrieverDataSourceVO from) {
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.updatePeriod",
                from.updatePeriodType.getPeriodDescription(from.updatePeriods),
                updatePeriodType.getPeriodDescription(updatePeriods));
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.httpRetriever.url", from.url, url);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.httpRetriever.timeout", from.timeoutSeconds,
                timeoutSeconds);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.httpRetriever.retries", from.retries, retries);
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
        SerializationHelper.writeSafeUTF(out, url);
        out.writeInt(updatePeriodType.getId());
        out.writeInt(updatePeriods);
        out.writeInt(timeoutSeconds);
        out.writeInt(retries);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            url = SerializationHelper.readSafeUTF(in);
            updatePeriodType = TimePeriods.fromId(in.readInt());
            updatePeriods = in.readInt();
            timeoutSeconds = in.readInt();
            retries = in.readInt();
        }
    }

}
