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
import com.serotonin.mango.rt.dataSource.http.HttpReceiverPointLocatorRT;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.vo.dataSource.AbstractPointLocatorVO;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.datasource.PointLocatorVO;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import org.springframework.validation.Errors;
import org.springframework.validation.Validator;

/**
 * @author Matthew Lohbihler
 */
public class HttpReceiverPointLocatorVO<T extends PointValueTime> extends AbstractPointLocatorVO<T> {

    public HttpReceiverPointLocatorVO(DataType dataType) {
        super(dataType);
    }

    public static class HttpReceiverPointLocatorVOValidator implements Validator {

        @Override
        public boolean supports(Class<?> clazz) {
            return HttpReceiverPointLocatorVO.class.isAssignableFrom(clazz);
        }

        @Override
        public void validate(Object target, Errors errors) {
            final HttpReceiverPointLocatorVO vo = (HttpReceiverPointLocatorVO) target;
            if (vo.parameterName.isEmpty()) {
                errors.rejectValue("parameterName", "validate.required");
            }
        }

    }

    @Override
    public boolean isSettable() {
        return false;
    }

    @Override
    public PointLocatorRT createRuntime() {
        return new HttpReceiverPointLocatorRT(this);
    }

    @Override
    public LocalizableMessage getConfigurationDescription() {
        return new LocalizableMessageImpl("dsEdit.httpReceiver.dpconn", parameterName);
    }

    private String parameterName;

    private String binary0Value;

    public String getParameterName() {
        return parameterName;
    }

    public void setParameterName(String parameterName) {
        this.parameterName = parameterName;
    }

    public String getBinary0Value() {
        return binary0Value;
    }

    public void setBinary0Value(String binary0Value) {
        this.binary0Value = binary0Value;
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        super.addProperties(list);
        AuditEventType.addPropertyMessage(list, "dsEdit.httpReceiver.httpParamName", parameterName);
        AuditEventType.addPropertyMessage(list, "dsEdit.httpReceiver.binaryZeroValue", binary0Value);
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, PointLocatorVO<T> o) {
        super.addPropertyChanges(list, o);
        HttpReceiverPointLocatorVO from = (HttpReceiverPointLocatorVO) o;
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.httpReceiver.httpParamName", from.parameterName,
                parameterName);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.httpReceiver.binaryZeroValue", from.binary0Value,
                binary0Value);
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
        out.writeObject(parameterName);
        out.writeObject(binary0Value);
    }

    private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            parameterName = (String)in.readObject();
            binary0Value = (String)in.readObject();
        }
    }

    @Override
    public String getName() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }
}
