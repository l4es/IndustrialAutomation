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
package com.serotonin.mango.rt.dataSource.spinwave;

import br.org.scadabr.utils.ImplementMeException;
import java.util.List;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.serotonin.io.serial.SerialParameters;
import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.rt.dataSource.EventDataSource;
import com.serotonin.mango.vo.dataSource.spinwave.BaseSpinwavePointLocatorVO;
import com.serotonin.mango.vo.dataSource.spinwave.SpinwaveDataSourceVO;
import com.serotonin.spinwave.SpinwaveReceiver;
import com.serotonin.spinwave.SwListener;
import com.serotonin.spinwave.SwMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;

/**
 * @author Matthew Lohbihler
 *
 */
public class SpinwaveDataSourceRT extends EventDataSource<SpinwaveDataSourceVO> implements SwListener {

    public static final int DATA_SOURCE_EXCEPTION_EVENT = 1;
    public static final int SENSOR_HEARTBEAT_EVENT = 2;
    public static final int UNKNOWN_SENSOR_EVENT = 3;
    public static final int ATTRIBUTE_NOT_FOUND_EVENT = 4;

    private final Log log = LogFactory.getLog(SpinwaveDataSourceRT.class);

    private SpinwaveReceiver spinwaveReceiver;

    public SpinwaveDataSourceRT(SpinwaveDataSourceVO vo) {
        super(vo, true);
    }

    //
    // /
    // / Lifecycle
    // /
    //
    @Override
    public void initialize() {
        SerialParameters params = new SerialParameters();
        params.setCommPortId(vo.getCommPortId());
        params.setPortOwnerName("Mango Spinwave Data Source");

        spinwaveReceiver = new SpinwaveReceiver(params, vo.getMessageVersion());
        spinwaveReceiver.setListener(this);
        spinwaveReceiver.setHeartbeatTimeout(vo.getHeartbeatTimeout() * 1000);

        // Add the known addresses
        for (long a : vo.getSensorAddresses()) {
            spinwaveReceiver.addSensorAddress(a);
        }

        try {
            spinwaveReceiver.initialize();

            // Deactivate any existing event.
            returnToNormal(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis());
        } catch (Exception e) {
            raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis(), true, wrapSerialException(e, vo
                    .getCommPortId()));
            log.debug("Error while initializing data source", e);
            return;
        }

        super.initialize();
    }

    @Override
    public void terminate() {
        super.terminate();
        spinwaveReceiver.terminate();
    }

    //
    // /
    // / SwListener
    // /
    //
    @Override
    public void receivedException(Exception e) {
        log.error("Exception from spinwave receiver", e);
    }

    public void receivedMessageMismatchException(Exception e) {
        log.error("Exception from spinwave receiver", e);
    }

    public void receivedResponseException(Exception e) {
        log.error("Exception from spinwave receiver", e);
    }

    @Override
    public void receivedHeartbeat(long sensorAddress, boolean active) {
        // We don't use the given information because if two sensors are currently in timeout and one comes back,
        // the timeout on the other will be lost unless we maintain a list of sensors in timeout in this class.
        // Since the list is already available from the receiver, just use that.
        List<Long> timeouts = spinwaveReceiver.getSensorTimeouts();
        if (timeouts.size() > 0) {
            raiseEvent(SENSOR_HEARTBEAT_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl(
                    "event.spinwave.heartbeat", timeouts.get(0)));
        } else // Deactivate any existing event.
        {
            returnToNormal(SENSOR_HEARTBEAT_EVENT, System.currentTimeMillis());
        }
    }

    @Override
    public void receivedMessage(SwMessage message) {
        throw  new ImplementMeException();
        /*
        // Find points that are interested in this sensor.
        BaseSpinwavePointLocatorVO locator;
        boolean found = false;
        synchronized (pointListChangeLock) {
            for (DataPointRT dp : dataPoints) {
                locator = ((SpinwavePointLocatorRT) dp.getPointLocator()).getVo();

                if (locator.getSensorAddress() == message.getSensorAddress()) {
                    found = true;

                    MangoValue value = locator.getValue(message);
                    if (value == null) {
                        raiseEvent(ATTRIBUTE_NOT_FOUND_EVENT, message.getTime(), false, new LocalizableMessageImpl(
                                "event.spinwave.attrNotFound", locator.getAttributeDescription(), message
                                .getSensorAddress()));
                    } else {
                        dp.updatePointValue(new PointValueTime(value, message.getTime()));
                    }
                }
            }
        }

        if (!found) // No points are interested in this sensor, so raise an event to indicate an unused sensor.
        {
            raiseEvent(UNKNOWN_SENSOR_EVENT, message.getTime(), false, new LocalizableMessageImpl(
                    "event.spinwave.unknownSensor", message.getSensorAddress()));
        }
                */
    }
}
