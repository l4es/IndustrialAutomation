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
package com.serotonin.mango.rt.dataSource.jmx;

import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.timer.cron.CronExpression;
import java.io.IOException;
import java.lang.management.ManagementFactory;
import java.net.MalformedURLException;

import javax.management.Attribute;
import javax.management.MBeanAttributeInfo;
import javax.management.MBeanInfo;
import javax.management.MBeanServerConnection;
import javax.management.MalformedObjectNameException;
import javax.management.ObjectName;
import javax.management.openmbean.CompositeData;
import javax.management.openmbean.OpenType;
import javax.management.remote.JMXConnector;
import javax.management.remote.JMXConnectorFactory;
import javax.management.remote.JMXServiceURL;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.SetPointSource;
import com.serotonin.mango.rt.dataSource.PollingDataSource;
import com.serotonin.mango.vo.dataSource.jmx.JmxDataSourceVO;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import java.text.ParseException;

/**
 * @author Matthew Lohbihler
 */
public class JmxDataSourceRT extends PollingDataSource<JmxDataSourceVO> {

    public static final int DATA_SOURCE_EXCEPTION_EVENT = 1;
    public static final int POINT_READ_EXCEPTION_EVENT = 2;
    public static final int POINT_WRITE_EXCEPTION_EVENT = 3;

    private final Log log = LogFactory.getLog(JmxDataSourceRT.class);

    private JMXConnector connector;
    private MBeanServerConnection server;

    public JmxDataSourceRT(JmxDataSourceVO vo) {
        super(vo, true);
        setPollingPeriod(vo.getUpdatePeriodType(), vo.getUpdatePeriods(), vo.isQuantize());
    }

    //
    //
    // Lifecycle
    //
    @Override
    public void initialize() {
        super.initialize();
    }

    @Override
    public void terminate() {
        super.terminate();
        closeServerConnection();
    }

    @Override
    public void doPoll(long time) {
        updateChangedPoints();
        openServerConnection();
        if (server == null) {
            return;
        }

        for (DataPointRT dprt : enabledDataPoints.values()) {
            if (!updateDataPoint(dprt)) {
                continue;
            }

            JmxPointLocatorRT loc = dprt.getPointLocator();

            Object attr;
            try {
                attr = server.getAttribute(loc.getObjectName(), loc.getVo().getAttributeName());
            } catch (Exception e) {
                raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                        "common.default", e.getMessage()));
                return;
            }

            Object value;
            if (loc.isComposite()) {
                if (!(attr instanceof CompositeData)) {
                    // Should have already been checked, so just write a warning and continue.
                    log.warn("CompositeData attribute was expected. Received " + attr);
                    continue;
                }

                CompositeData cd = (CompositeData) attr;
                value = cd.get(loc.getVo().getCompositeItemName());
            } else {
                value = attr;
            }

            PointValueTime pvt = new PointValueTime(loc.managementValueToMangoValue(value), time);
            dprt.updatePointValue(pvt, true);
        }
    }

    @Override
    public void setPointValue(DataPointRT dataPoint, PointValueTime valueTime, SetPointSource source) {
        if (server == null) {
            raiseEvent(POINT_WRITE_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                    "dsEdit.jmx.writeFailed", dataPoint.getVoName()));
            return;
        }

        updateDataPoint(dataPoint);
        JmxPointLocatorRT loc = dataPoint.getPointLocator();
        if (loc.getObjectName() == null) {
            raiseEvent(POINT_WRITE_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                    "dsEdit.jmx.writeFailed", dataPoint.getVoName()));
            return;
        }

        if (loc.isComposite()) {
            raiseEvent(POINT_WRITE_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                    "dsEdit.jmx.writeFailed.composite", dataPoint.getVoName()));
            return;
        }

        Attribute attr = new Attribute(loc.getVo().getAttributeName(),
                loc.mangoValueToManagementValue(valueTime.getValue()));
        try {
            server.setAttribute(loc.getObjectName(), attr);
        } catch (Exception e) {
            raiseEvent(POINT_WRITE_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                    "dsEdit.jmx.writeFailed.msg", dataPoint.getVoName(), e.getMessage()));
        }
    }

    private void openServerConnection() {
        if (server == null) {
            if (vo.isUseLocalServer()) {
                server = ManagementFactory.getPlatformMBeanServer();
            } else {
                String url = "service:jmx:rmi:///jndi/rmi://" + vo.getRemoteServerAddr() + "/jmxrmi";
                try {
                    connector = JMXConnectorFactory.connect(new JMXServiceURL(url), null);
                    server = connector.getMBeanServerConnection();
                } catch (MalformedURLException e) {
                    raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                            "common.default", e.getMessage()));
                    return;
                } catch (IOException e) {
                    raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                            "common.default", e.getMessage()));
                    return;
                }
            }

            returnToNormal(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis());
        }
    }

    private void closeServerConnection() {
        if (connector != null) {
            try {
                connector.close();
            } catch (IOException e) {
                // ignore
            }
            connector = null;
        }

        if (server != null) {
            server = null;
        }
    }

    private boolean updateDataPoint(DataPointRT dp) {
        JmxPointLocatorRT loc = dp.getPointLocator();

        boolean updated = false;

        if (loc.getObjectName() == null) {
            try {
                loc.setObjectName(new ObjectName(loc.getVo().getObjectName()));
            } catch (MalformedObjectNameException e) {
                raiseEvent(POINT_READ_EXCEPTION_EVENT, System.currentTimeMillis(), true,
                        new LocalizableMessageImpl("dsEdit.jmx.objectNameError", loc.getVo().getObjectName(),
                                dp.getVoName(), e.getMessage()));
                return false;
            } catch (NullPointerException e) {
                raiseEvent(POINT_READ_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                        "dsEdit.jmx.objectNameNotFound", loc.getVo().getObjectName(), dp.getVoName()));
                return false;
            }
            updated = true;
        }

        if (loc.getType() == null && server != null) {
            MBeanInfo info;
            try {
                info = server.getMBeanInfo(loc.getObjectName());
            } catch (Exception e) {
                raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                        "common.default", e.getMessage()));
                return false;
            }

            MBeanAttributeInfo attr = getAttributeInfo(info, loc.getVo().getAttributeName());
            if (attr == null) {
                raiseEvent(POINT_READ_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                        "dsEdit.jmx.attributeNameNotFound", loc.getVo().getAttributeName(), dp.getVoName()));
                return false;
            }

            String type = null;
            if (!loc.isComposite()) {
                type = attr.getType();

                if (!JmxPointLocatorRT.isValidType(type)) {
                    raiseEvent(POINT_READ_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                            "dsEdit.jmx.attributeTypeNotSupported", type, dp.getVoName()));
                    return false;
                }

            } else {
                if (!attr.getType().equals("javax.management.openmbean.CompositeData")) {
                    raiseEvent(POINT_READ_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                            "dsEdit.jmx.attributeNotComposite", loc.getVo().getAttributeName(), dp.getVoName()));
                    return false;
                }

                CompositeData cd;
                try {
                    cd = (CompositeData) server.getAttribute(loc.getObjectName(), attr.getName());
                } catch (Exception e) {
                    raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                            "common.default", e.getMessage()));
                    return false;
                }

                OpenType<?> openType = cd.getCompositeType().getType(loc.getVo().getCompositeItemName());
                if (openType == null) {
                    raiseEvent(POINT_READ_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                            "dsEdit.jmx.compositeNameNotFound", loc.getVo().getCompositeItemName(), dp
                            .getVoName()));
                    return false;
                }

                type = openType.getTypeName();

                if (!JmxPointLocatorRT.isValidType(type)) {
                    raiseEvent(POINT_READ_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                            "dsEdit.jmx.compositeTypeNotSupported", type, dp.getVoName()));
                    return false;
                }
            }

            loc.setType(type);
            updated = true;
        }

        if (updated) {
            returnToNormal(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis());
            returnToNormal(POINT_READ_EXCEPTION_EVENT, System.currentTimeMillis());
        }

        return true;
    }

    private MBeanAttributeInfo getAttributeInfo(MBeanInfo info, String attributeName) {
        for (MBeanAttributeInfo attr : info.getAttributes()) {
            if (attr.getName().equals(attributeName)) {
                return attr;
            }
        }
        return null;
    }
    @Override
    protected CronExpression getCronExpression() throws ParseException {
        throw new ImplementMeException();
    }
}
