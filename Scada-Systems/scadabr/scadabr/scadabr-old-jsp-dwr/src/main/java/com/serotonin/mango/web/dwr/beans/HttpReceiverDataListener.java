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
package com.serotonin.mango.web.dwr.beans;

import br.org.scadabr.l10n.AbstractLocalizer;
import java.util.ResourceBundle;

import com.serotonin.mango.Common;
import com.serotonin.mango.rt.dataSource.http.HttpMulticastListener;
import com.serotonin.mango.rt.dataSource.http.HttpReceiverData;
import br.org.scadabr.web.i18n.I18NUtils;
import br.org.scadabr.l10n.Localizer;

/**
 * @author Matthew Lohbihler
 */
public class HttpReceiverDataListener implements HttpMulticastListener, TestingUtility {

    final ResourceBundle bundle;
    private final String[] ipWhiteList;
    private final String[] deviceIdWhiteList;
    String message;
    private HttpReceiverData data;

    // Auto shut-off stuff
    private final AutoShutOff autoShutOff;

    public HttpReceiverDataListener(ResourceBundle bundle, String[] ipWhiteList, String[] deviceIdWhiteList) {
        this.bundle = bundle;
        message = AbstractLocalizer.localizeI18nKey("dsEdit.httpReceiver.tester.listening", bundle);

        this.ipWhiteList = ipWhiteList;
        this.deviceIdWhiteList = deviceIdWhiteList;
        Common.ctx.getHttpReceiverMulticaster().addListener(this);

        autoShutOff = new AutoShutOff() {
            @Override
            void shutOff() {
                message = AbstractLocalizer.localizeI18nKey("dsEdit.httpReceiver.tester.auto", HttpReceiverDataListener.this.bundle);
                HttpReceiverDataListener.this.cancel();
            }
        };
    }

    public HttpReceiverData getData() {
        autoShutOff.update();
        return data;
    }

    public String getMessage() {
        autoShutOff.update();
        return message;
    }

    @Override
    public void cancel() {
        autoShutOff.cancel();
        Common.ctx.getHttpReceiverMulticaster().removeListener(this);
    }

    //
    // /
    // / HttpMulticastListener
    // /
    //
    @Override
    public String[] getDeviceIdWhiteList() {
        return deviceIdWhiteList;
    }

    @Override
    public String[] getIpWhiteList() {
        return ipWhiteList;
    }

    @Override
    public void ipWhiteListError(String message) {
        this.message = AbstractLocalizer.localizeI18nKey("dsEdit.httpReceiver.tester.whiteList", bundle, message);
    }

    @Override
    public void data(HttpReceiverData data) {
        message = AbstractLocalizer.localizeI18nKey("dsEdit.httpReceiver.tester.data", bundle);
        this.data = data;
    }
}
