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

import br.org.scadabr.DataType;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;

import com.serotonin.mango.rt.dataSource.PointLocatorRT;
import com.serotonin.mango.rt.dataSource.http.HttpImagePointLocatorRT;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.vo.dataSource.AbstractPointLocatorVO;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.utils.i18n.LocalizableEnum;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.datasource.PointLocatorVO;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import org.springframework.validation.Errors;
import org.springframework.validation.Validator;

/**
 * @author Matthew Lohbihler
 */
public class HttpImagePointLocatorVO<T extends PointValueTime> extends AbstractPointLocatorVO<T> {

    public HttpImagePointLocatorVO(DataType dataType) {
        super(DataType.IMAGE);
    }

    public static class HttpImagePointLocatorVoValidato implements Validator {

        @Override
        public boolean supports(Class<?> clazz) {
            return HttpImagePointLocatorVO.class.isAssignableFrom(clazz);
        }

        @Override
        public void validate(Object target, Errors errors) {
            final HttpImagePointLocatorVO vo = (HttpImagePointLocatorVO) target;
            if (vo.url.isEmpty()) {
                errors.rejectValue("url", "validate.required");
            }
            if (vo.timeoutSeconds <= 0) {
                errors.rejectValue("timeoutSeconds", "validate.greaterThanZero");
            }
            if (vo.retries < 0) {
                errors.rejectValue("retries", "validate.cannotBeNegative");
            }
            switch (vo.scaleType) {
                case PERCENT:
                    if (vo.scalePercent <= 0) {
                        errors.rejectValue("scalePercent", "validate.greaterThanZero");
                    } else if (vo.scalePercent > 100) {
                        errors.rejectValue("scalePercent", "validate.lessThan100");
                    }
                    break;
                case BOX:
                    if (vo.scaleWidth <= 0) {
                        errors.rejectValue("scaleWidth", "validate.greaterThanZero");
                    }
                    if (vo.scaleHeight <= 0) {
                        errors.rejectValue("scaleHeight", "validate.greaterThanZero");
                    }
            }

            if (vo.readLimit <= 0) {
                errors.rejectValue("readLimit", "validate.greaterThanZero");
            }
        }
    }

    public enum Scale implements LocalizableEnum<Scale> {

        NONE(0, "dsEdit.httpImage.scalingType.none"),
        PERCENT(1, "dsEdit.httpImage.scalingType.percent"),
        BOX(2, "dsEdit.httpImage.scalingType.box");
        private final int id;
        private final String i18nKey;

        private Scale(int id, String i18nKey) {
            this.id = id;
            this.i18nKey = i18nKey;
        }

        public int getId() {
            return id;
        }

        public static Scale fromId(int id) {
            switch (id) {
                case 0:
                    return NONE;
                case 1:
                    return PERCENT;
                case 2:
                    return BOX;
                default:
                    throw new IndexOutOfBoundsException("Cant get Scale from id: " + id);
            }
        }

        @Override
        public String getName() {
            return name();
        }

        @Override
        public String getI18nKey() {
            return i18nKey;
        }

        @Override
        public Object[] getArgs() {
            return null;
        }

    }

    @Override
    public boolean isSettable() {
        return false;
    }

    @Override
    public PointLocatorRT createRuntime() {
        return new HttpImagePointLocatorRT(this);
    }

    @Override
    public LocalizableMessage getConfigurationDescription() {
        return new LocalizableMessageImpl("common.default", url);
    }

    private String url;

    private int timeoutSeconds = 30;

    private int retries = 2;
    private Scale scaleType;

    private int scalePercent = 25;

    private int scaleWidth = 100;

    private int scaleHeight = 100;

    private int readLimit = 10000;

    private String webcamLiveFeedCode;

    public String getUrl() {
        return url;
    }

    public void setUrl(String url) {
        this.url = url;
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

    public Scale getScaleType() {
        return scaleType;
    }

    public void setScaleType(Scale scaleType) {
        this.scaleType = scaleType;
    }

    public int getScalePercent() {
        return scalePercent;
    }

    public void setScalePercent(int scalePercent) {
        this.scalePercent = scalePercent;
    }

    public int getScaleWidth() {
        return scaleWidth;
    }

    public void setScaleWidth(int scaleWidth) {
        this.scaleWidth = scaleWidth;
    }

    public int getScaleHeight() {
        return scaleHeight;
    }

    public void setScaleHeight(int scaleHeight) {
        this.scaleHeight = scaleHeight;
    }

    public int getReadLimit() {
        return readLimit;
    }

    public void setReadLimit(int readLimit) {
        this.readLimit = readLimit;
    }

    public String getWebcamLiveFeedCode() {
        return webcamLiveFeedCode;
    }

    public void setWebcamLiveFeedCode(String webcamLiveFeedCode) {
        this.webcamLiveFeedCode = webcamLiveFeedCode;
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        super.addProperties(list);
        AuditEventType.addPropertyMessage(list, "dsEdit.httpImage.url", url);
        AuditEventType.addPropertyMessage(list, "dsEdit.httpImage.timeout", timeoutSeconds);
        AuditEventType.addPropertyMessage(list, "dsEdit.httpImage.retries", retries);
        AuditEventType.addPropertyMessage(list, "dsEdit.httpImage.scalingType", scaleType);
        AuditEventType.addPropertyMessage(list, "dsEdit.httpImage.scalePercent", scalePercent);
        AuditEventType.addPropertyMessage(list, "dsEdit.httpImage.scaleWidth", scaleWidth);
        AuditEventType.addPropertyMessage(list, "dsEdit.httpImage.scaleHeight", scaleHeight);
        AuditEventType.addPropertyMessage(list, "dsEdit.httpImage.readLimit", readLimit);
        AuditEventType.addPropertyMessage(list, "dsEdit.httpImage.liveFeed", webcamLiveFeedCode);
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, PointLocatorVO<T> o) {
        super.addPropertyChanges(list, o);
        HttpImagePointLocatorVO from = (HttpImagePointLocatorVO) o;
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.httpImage.url", from.url, url);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.httpImage.timeout", from.timeoutSeconds, timeoutSeconds);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.httpImage.retries", from.retries, retries);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.httpImage.scalingType", from.scaleType, scaleType);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.httpImage.scalePercent", from.scalePercent, scalePercent);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.httpImage.scaleWidth", from.scaleWidth, scaleWidth);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.httpImage.scaleHeight", from.scaleHeight, scaleHeight);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.httpImage.readLimit", from.readLimit, readLimit);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.httpImage.liveFeed", from.webcamLiveFeedCode, webcamLiveFeedCode);
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
        out.writeInt(timeoutSeconds);
        out.writeInt(retries);
        out.writeInt(scaleType.getId());
        out.writeInt(scalePercent);
        out.writeInt(scaleWidth);
        out.writeInt(scaleHeight);
        out.writeInt(readLimit);
        SerializationHelper.writeSafeUTF(out, webcamLiveFeedCode);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            url = SerializationHelper.readSafeUTF(in);
            timeoutSeconds = in.readInt();
            retries = in.readInt();
            scaleType = Scale.fromId(in.readInt());
            scalePercent = in.readInt();
            scaleWidth = in.readInt();
            scaleHeight = in.readInt();
            readLimit = in.readInt();
            webcamLiveFeedCode = SerializationHelper.readSafeUTF(in);
        }
    }

    @Override
    public String getName() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }
}
