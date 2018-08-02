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
package com.serotonin.mango.rt.dataSource.nmea;

import br.org.scadabr.utils.ImplementMeException;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.serotonin.io.serial.SerialParameters;
import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.rt.dataSource.DataSourceUtils;
import com.serotonin.mango.rt.dataSource.EventDataSource;
import com.serotonin.mango.util.timeout.RunClient;
import com.serotonin.mango.vo.dataSource.nmea.NmeaDataSourceVO;
import br.org.scadabr.utils.i18n.LocalizableException;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import com.serotonin.mango.util.timeout.SystemRunTask;

/**
 * @author Matthew Lohbihler
 */
public class NmeaDataSourceRT extends EventDataSource<NmeaDataSourceVO> implements NmeaMessageListener, RunClient {

    public static final int DATA_SOURCE_EXCEPTION_EVENT = 1;
    public static final int PARSE_EXCEPTION_EVENT = 2;

    private final Log log = LogFactory.getLog(NmeaDataSourceRT.class);

    private NmeaReceiver nmeaReceiver;
    private SystemRunTask resetTask;

    public NmeaDataSourceRT(NmeaDataSourceVO vo) {
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
        params.setBaudRate(vo.getBaudRate());
        params.setPortOwnerName("Mango NMEA Data Source");

        nmeaReceiver = new NmeaReceiver(this, params);

        if (initNmea()) {
            scheduleTimeout();
            super.initialize();
        }
    }

    @Override
    public void terminate() {
        super.terminate();
        unscheduleTimeout();
        termNmea();
    }

    synchronized private boolean initNmea() {
        try {
            nmeaReceiver.initialize();

            // Deactivate any existing event.
            returnToNormal(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis());
        } catch (Exception e) {
            LocalizableMessage message = wrapSerialException(e, vo.getCommPortId());
            raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis(), true, message);
            log.debug("Error while initializing data source", e);
            return false;
        }

        return true;
    }

    synchronized private void termNmea() {
        nmeaReceiver.terminate();
    }

    //
    // /
    // / MessagingConnectionListener
    // /
    //
    @Override
    public void receivedException(Exception e) {
        log.error("Exception from nmea receiver", e);
    }

    @Override
    public void receivedMessage(NmeaMessage message) {
        throw new ImplementMeException();
        /*
         long time = System.currentTimeMillis();

         unscheduleTimeout();
         scheduleTimeout();

         LocalizableMessage parseError = null;

         synchronized (pointListChangeLock) {
         for (DataPointRT dp : dataPoints) {
         try {
         receivedMessageImpl(dp, message, time);
         } catch (LocalizableException e) {
         if (parseError == null) {
         parseError = e;
         }
         } catch (Exception e) {
         if (parseError == null) {
         parseError = new LocalizableMessageImpl("event.exception2", dp.getVoName(), e.getMessage());
         }
         }
         }
         }

         if (parseError != null) {
         raiseEvent(PARSE_EXCEPTION_EVENT, time, false, parseError);
         }
         */
    }

    private void receivedMessageImpl(DataPointRT dp, NmeaMessage message, long time) throws Exception {
        NmeaPointLocatorRT locator = dp.getPointLocator();

        String messageName = message.getName();
        if (messageName == null) {
            return;
        }

        if (messageName.equals(locator.getMessageName())) {
            // Message name match. Check if the field index is in bounds.
            if (locator.getFieldIndex() <= message.getFieldCount()) {
                // Get the field by index.
                String valueStr = message.getField(locator.getFieldIndex());

                // Convert the value
                MangoValue value = DataSourceUtils.getValue(valueStr, locator.getDataType(),
                        locator.getBinary0Value(), dp.getVo().getTextRenderer(), null, dp.getVoName());

                // Save the new value
                dp.updatePointValue(new PointValueTime(value, time));
            } else {
                throw new Exception("Field index " + locator.getFieldIndex()
                        + " is out of bounds. Message field count is " + message.getFieldCount());
            }
        }
    }

    //
    // /
    // / TimeoutClient
    // /
    //
    @Override
    public void run(long fireTime) {
        // We haven't heard from the device for too long. Restart the listener.
        termNmea();
        if (initNmea()) {
            scheduleTimeout();
        }
    }

    private void scheduleTimeout() {
        throw new ImplementMeException(); //resetTask = new TimeoutTask(vo.getResetTimeout() * 1000, this);
    }

    private void unscheduleTimeout() {
        // do we need to copy?? - what for???
        SystemRunTask tt = resetTask;
        if (tt != null) {
            tt.cancel();
        }
    }
}
