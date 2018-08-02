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
package com.serotonin.mango.rt.dataSource.http;

import br.org.scadabr.rt.RT;
import br.org.scadabr.rt.WrongEdgeTypeException;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.vo.EdgeType;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.serotonin.mango.Common;
import com.serotonin.mango.rt.dataSource.EventDataSource;
import com.serotonin.mango.vo.dataSource.http.HttpImageDataSourceVO;
import com.serotonin.mango.vo.dataSource.http.HttpReceiverDataSourceVO;
import br.org.scadabr.vo.EdgeConsumer;

/**
 * @author Matthew Lohbihler
 */
public class HttpReceiverDataSourceRT extends EventDataSource<HttpReceiverDataSourceVO> implements HttpMulticastListener {

    private final Log log = LogFactory.getLog(HttpReceiverDataSourceRT.class);
    private String[] ipWhiteList;
    private String[] deviceIdWhiteList;
    
    public HttpReceiverDataSourceRT(HttpReceiverDataSourceVO vo) {
        super(vo, true);
        ipWhiteList = vo.getIpWhiteList();
        deviceIdWhiteList = vo.getDeviceIdWhiteList();
    }

    //
    // /
    // / Lifecycle
    // /
    //
    @Override
    public void initialize() {
        Common.ctx.getHttpReceiverMulticaster().addListener(this);
        super.initialize();
    }

    @Override
    public void terminate() {
        super.terminate();
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
        log.warn("Error checking white list: " + message);
    }

    @Override
    public void data(HttpReceiverData data) {
        throw new ImplementMeException();
        /*
        // Match data points in the received set with point locators.
        synchronized (pointListChangeLock) {
            for (DataPointRT dp : dataPoints) {
                HttpReceiverPointLocatorVO locator = ((HttpReceiverPointLocatorRT) dp.getPointLocator()).getVo();
                String paramName = locator.getParameterName();

                for (HttpReceiverPointSample sample : data.getData()) {
                    if (sample.getKey().equals(paramName)) {
                        // This just informs the data object that the key was used so that it knows what was used
                        // and what wasn't.
                        data.consume(paramName);

                        String valueStr = sample.getValue();
                        long time = sample.getTime();
                        if (time == 0) {
                            time = data.getTime();
                        }

                        MangoValue value;
                        if (locator.getDataTypeId() == DataTypes.BINARY
                                && !locator.getBinary0Value().isEmpty()) {
                            if (valueStr.equalsIgnoreCase(locator.getBinary0Value())) {
                                value = BinaryValue.ZERO;
                            } else {
                                value = BinaryValue.ONE;
                            }
                        } else {
                            value = MangoValue.stringToValue(valueStr, locator.getDataTypeId());
                        }

                        dp.updatePointValue(new PointValueTime(value, time));
                    }
                }
            }
        }
                */
    }

    @Override
    public void wireEdgeAsSrc(RT<?> dest, EdgeType edgeType) throws WrongEdgeTypeException {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void wireEdgeAsDest(RT<?> src, EdgeType edgeType) throws WrongEdgeTypeException {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void iterateEdgesAsSrc(EdgeConsumer edgeIterator, EdgeType... edgeTypes) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void iterateEdgesAsDest(EdgeConsumer edgeIterator, EdgeType... edgeTypes) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void patch(HttpReceiverDataSourceVO vo) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public HttpReceiverDataSourceVO getVO() {
        final HttpReceiverDataSourceVO result = new HttpReceiverDataSourceVO();
        fillVO(result);
        return result;
    }
}
