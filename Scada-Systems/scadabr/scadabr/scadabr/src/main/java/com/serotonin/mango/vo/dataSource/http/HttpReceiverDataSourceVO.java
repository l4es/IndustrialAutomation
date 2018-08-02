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
import java.util.Arrays;
import java.util.List;



import com.serotonin.mango.rt.dataSource.DataSourceRT;
import com.serotonin.mango.rt.dataSource.http.HttpReceiverDataSourceRT;
import com.serotonin.mango.rt.event.type.AuditEventType;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import br.org.scadabr.util.ArrayUtils;
import br.org.scadabr.util.IpAddressUtils;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.datasource.http.HttpReceiverDataSourceEventKey;
import br.org.scadabr.vo.datasource.http.HttpReceiverDataSourceType;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import org.springframework.validation.Errors;

/**
 * @author Matthew Lohbihler
 */

public class HttpReceiverDataSourceVO extends DataSourceVO<HttpReceiverDataSourceVO> {

    @Override
    public Set<HttpReceiverDataSourceEventKey> createEventKeySet() {
        return new HashSet<>();
    }

    @Override
    public Map<HttpReceiverDataSourceEventKey, ?> createEventKeyMap() {
        return new HashMap<>();
    }
/* TODO Validation
    public static class HttpReceiverDataSourceVOValidator extends DataSourceValidator {

        @Override
        public boolean supports(Class<?> clazz) {
            return HttpReceiverDataSourceVO.class.isAssignableFrom(clazz);
        }

        @Override
        public void validate(Object target, Errors errors) {
            super.validate(target, errors);
            final HttpReceiverDataSourceVO vo = (HttpReceiverDataSourceVO) target;

            for (String ipmask : vo.ipWhiteList) {
                String msg = IpAddressUtils.checkIpMask(ipmask);
                if (msg != null) {
                    errors.rejectValue("ipWhiteList", "common.default", new Object[]{msg}, "common.default");
                }
            }

            for (String deviceId : vo.deviceIdWhiteList) {
                if (deviceId.isEmpty()) {
                    errors.rejectValue("deviceIdWhiteList", "validate.missingDeviceId");
                }
            }
        }

    }
*/
    @Override
    public String getDataSourceTypeKey() {
        return HttpReceiverDataSourceType.KEY;
    }

    @Override
    public int getDataSourceTypeId() {
        return HttpReceiverDataSourceType.DB_ID;
    }

    @Override
    public LocalizableMessage getConnectionDescription() {
        if (ipWhiteList.length == 0 || deviceIdWhiteList.length == 0) {
            return new LocalizableMessageImpl("dsEdit.httpReceiver.dsconn.blocked");
        }

        if (ArrayUtils.contains(deviceIdWhiteList, "*")) {
            if (ipWhiteList.length == 1) {
                return new LocalizableMessageImpl("dsEdit.httpReceiver.dsconn", ipWhiteList[0]);
            }
            return new LocalizableMessageImpl("dsEdit.httpReceiver.dsconn", ipWhiteList[0] + ", ...");
        }

        if (ArrayUtils.contains(ipWhiteList, "*.*.*.*")) {
            if (deviceIdWhiteList.length == 1) {
                return new LocalizableMessageImpl("dsEdit.httpReceiver.dsconn", deviceIdWhiteList[0]);
            }
            return new LocalizableMessageImpl("dsEdit.httpReceiver.dsconn", deviceIdWhiteList[0] + ", ...");
        }

        return new LocalizableMessageImpl("dsEdit.httpReceiver.dsconn.combo");
    }

    @Override
    public HttpReceiverDataSourceRT createRT() {
        return new HttpReceiverDataSourceRT(this);
    }

    private String[] ipWhiteList = new String[]{"*.*.*.*"};
    
    private String[] deviceIdWhiteList = new String[]{"*"};

    public String[] getIpWhiteList() {
        return ipWhiteList;
    }

    public void setIpWhiteList(String[] ipWhiteList) {
        this.ipWhiteList = ipWhiteList;
    }

    public String[] getDeviceIdWhiteList() {
        return deviceIdWhiteList;
    }

    public void setDeviceIdWhiteList(String[] deviceIdWhiteList) {
        this.deviceIdWhiteList = deviceIdWhiteList;
    }

    @Override
    protected void addPropertiesImpl(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.httpReceiver.ipWhiteList", Arrays.toString(ipWhiteList));
        AuditEventType.addPropertyMessage(list, "dsEdit.httpReceiver.deviceWhiteList", Arrays
                .toString(deviceIdWhiteList));
    }

    @Override
    protected void addPropertyChangesImpl(List<LocalizableMessage> list, HttpReceiverDataSourceVO from) {
        if (Arrays.equals(from.ipWhiteList, ipWhiteList)) {
            AuditEventType.addPropertyChangeMessage(list, "dsEdit.httpReceiver.ipWhiteList", Arrays
                    .toString(from.ipWhiteList), Arrays.toString(ipWhiteList));
        }
        if (Arrays.equals(from.deviceIdWhiteList, deviceIdWhiteList)) {
            AuditEventType.addPropertyChangeMessage(list, "dsEdit.httpReceiver.deviceWhiteList", Arrays
                    .toString(from.deviceIdWhiteList), Arrays.toString(deviceIdWhiteList));
        }
    }

    //
    // /
    // / Serialization
    // /
    //
    private static final long serialVersionUID = -1;
    private static final int version = 2;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(version);
        out.writeObject(ipWhiteList);
        out.writeObject(deviceIdWhiteList);
    }

    private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            ipWhiteList = (String[]) in.readObject();
            deviceIdWhiteList = new String[]{"*"};
        } else if (ver == 2) {
            ipWhiteList = (String[]) in.readObject();
            deviceIdWhiteList = (String[]) in.readObject();
        }
    }
}
