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
package com.serotonin.mango.rt.dataSource.onewire;

import br.org.scadabr.utils.ImplementMeException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.dalsemi.onewire.OneWireException;
import com.dalsemi.onewire.adapter.OneWireIOException;
import com.dalsemi.onewire.container.ADContainer;
import com.dalsemi.onewire.container.HumidityContainer;
import com.dalsemi.onewire.container.OneWireContainer;
import com.dalsemi.onewire.container.OneWireContainer1D;
import com.dalsemi.onewire.container.OneWireSensor;
import com.dalsemi.onewire.container.PotentiometerContainer;
import com.dalsemi.onewire.container.SwitchContainer;
import com.dalsemi.onewire.container.TemperatureContainer;
import com.dalsemi.onewire.utils.Address;
import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.timer.cron.CronExpression;
import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.SetPointSource;
import com.serotonin.mango.rt.dataImage.types.BinaryValue;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.rt.dataImage.types.MultistateValue;
import com.serotonin.mango.rt.dataImage.types.NumericValue;
import com.serotonin.mango.rt.dataSource.PollingDataSource;
import com.serotonin.mango.vo.dataSource.onewire.OneWireDataSourceVO;
import com.serotonin.mango.vo.dataSource.onewire.OneWirePointLocatorVO;
import br.org.scadabr.utils.i18n.LocalizableException;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import java.text.ParseException;

/**
 * @author Matthew Lohbihler
 */
public class OneWireDataSourceRT extends PollingDataSource<OneWireDataSourceVO> {

    private static final Log LOG = LogFactory.getLog(OneWireDataSourceRT.class);

    public static final int DATA_SOURCE_EXCEPTION_EVENT = 1;
    public static final int POINT_READ_EXCEPTION_EVENT = 2;
    public static final int POINT_WRITE_EXCEPTION_EVENT = 3;

    private Network network;
    private long nextRescan = 0;

    public OneWireDataSourceRT(OneWireDataSourceVO vo) {
        super(vo, true);
        setPollingPeriod(vo.getUpdatePeriodType(), vo.getUpdatePeriods(), false);
    }

    public Network getNetwork() {
        return network;
    }

    @Override
    public void doPoll(long time) {
        updateChangedPoints();
        if (vo.isRescan()) {
            if (nextRescan == 0) {
                updateNextRescan(time);
            }

            if (time >= nextRescan) {
                terminateNetwork();
                try {
                    Thread.sleep(2000);
                } catch (InterruptedException e) {
                    // no op
                }
                initializeNetwork();
                updateNextRescan(time);
            }
        }

        if (network == null) {
            initializeNetwork();
            if (network == null) {
                return;
            }
        }

        // Create a local list of points so that we can remove those that we're done with.
        List<DataPointRT> points = new ArrayList<>(enabledDataPoints.values());

        LocalizableMessage exceptionMessage = null;
        try {
            exceptionMessage = tryRead(points, time, true);
        } catch (NetworkReloadException e) {
            LOG.info("", e);

            // Keep the exception message if there is one.
            exceptionMessage = e;

            // There was an exception during reading. Reinitialize the network and try to continue, but this time
            // fail if an error occurs again.
            try {
                terminateNetwork();
                try {
                    Thread.sleep(2000);
                } catch (InterruptedException e2) {
                    // no op
                }
                initializeNetwork();

                try {
                    network.lock();
                    network.quickInitialize();
                } finally {
                    network.unlock();
                }

                try {
                    LocalizableMessage msg = tryRead(points, time, false);
                    if (exceptionMessage == null) {
                        exceptionMessage = msg;
                    }
                } catch (NetworkReloadException e1) {
                    // no op
                }
            } catch (Exception e2) {
                LOG.info("", e2);
                raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis(), true,
                        wrapSerialException(e2, vo.getCommPortId()));
                terminateNetwork();
                return;
            }
        }

        if (exceptionMessage == null && points.size() > 0) // If any points are left in the list, they count as exceptions.
        {
            exceptionMessage = new LocalizableMessageImpl("event.1wire.noPointData", points.get(0).getVoName());
        }

        // Event handling.
        if (exceptionMessage != null) {
            raiseEvent(POINT_READ_EXCEPTION_EVENT, time, true, exceptionMessage);
        } else {
            returnToNormal(POINT_READ_EXCEPTION_EVENT, time);
        }
    }

    private LocalizableMessage tryRead(List<DataPointRT> points, long time, boolean throwToReload)
            throws NetworkReloadException {
        LocalizableMessage exceptionMessage = null;

        try {
            network.lock();

            // Get the list of network addresses, optimally sorted according to the known network topology. We traverse
            // the
            // network according to its topology (as opposed to more convenient ways) because the network is by far the
            // performance bottleneck.
            List<Long> addresses = network.getAddresses();

            NetworkPath lastPath = null;
            for (Long address : addresses) {
                // Find out if any points care about this address.
                if (!arePointsThatNeedAddress(address, points)) {
                    continue;
                }

                // Get the device data
                NetworkPath path = network.getNetworkPath(address);
                OneWireContainer owc = path.getTarget();

                try {
                    path.open(lastPath);
                    readSensor(owc, points, address, time);
                    lastPath = path;
                } catch (Exception e) {
                    // Make sure the path gets closed.
                    try {
                        path.close();
                    } catch (OneWireException e1) {
                        // no op
                    }

                    lastPath = null;

                    // Check if we're allowed to throw an exception to prompt a network reload.
                    if (throwToReload) {
                        throw new NetworkReloadException(exceptionMessage.getI18nKey(), exceptionMessage.getArgs());
                    }

                    // Remove all points that want to know this device.
                    Iterator<DataPointRT> iter = points.iterator();
                    while (iter.hasNext()) {
                        DataPointRT point = iter.next();
                        OneWirePointLocatorRT locator = point.getPointLocator();
                        if (locator.getAddress().equals(address)) {
                            iter.remove();
                        }
                    }

                    if (exceptionMessage == null) {
                        exceptionMessage = new LocalizableMessageImpl("event.1wire.deviceRead", Address.toString(address),
                                e.getMessage());
                    }
                }
            }

            if (lastPath != null) {
                lastPath.close();
            }
        } catch (OneWireException e) {
            if (exceptionMessage == null) {
                exceptionMessage = new LocalizableMessageImpl("event.1wire.networkRead", e.getMessage());
            }
        } finally {
            network.unlock();
        }

        return exceptionMessage;
    }

    private boolean arePointsThatNeedAddress(Long address, List<DataPointRT> points) {
        for (DataPointRT point : points) {
            OneWirePointLocatorRT locator = point.getPointLocator();
            if (locator.getAddress().equals(address)) {
                return true;
            }
        }
        return false;
    }

    private void readSensor(OneWireContainer owc, List<DataPointRT> points, Long address, long time)
            throws OneWireIOException, OneWireException {
        byte[] state = null;
        if (owc instanceof OneWireSensor) {
            state = ((OneWireSensor) owc).readDevice();
        }

        // Find the points that want this data.
        Iterator<DataPointRT> iter = points.iterator();
        while (iter.hasNext()) {
            DataPointRT point = iter.next();
            OneWirePointLocatorRT locator = point.getPointLocator();

            if (locator.getAddress().equals(address)) {
                // This point wants the data.
                int attributeId = locator.getVo().getAttributeId();
                int index = locator.getVo().getIndex();
                MangoValue result = null;

                if (attributeId == OneWirePointLocatorVO.AttributeTypes.TEMPURATURE) {
                    TemperatureContainer tc = (TemperatureContainer) owc;
                    tc.doTemperatureConvert(state);
                    result = new NumericValue(tc.getTemperature(state));
                } else if (attributeId == OneWirePointLocatorVO.AttributeTypes.HUMIDITY) {
                    HumidityContainer hc = (HumidityContainer) owc;
                    hc.doHumidityConvert(state);
                    result = new NumericValue(hc.getHumidity(state));
                } else if (attributeId == OneWirePointLocatorVO.AttributeTypes.AD_VOLTAGE) {
                    ADContainer ac = (ADContainer) owc;
                    ac.doADConvert(index, state);
                    result = new NumericValue(ac.getADVoltage(index, state));
                } else if (attributeId == OneWirePointLocatorVO.AttributeTypes.LATCH_STATE) {
                    SwitchContainer sc = (SwitchContainer) owc;
                    result = new BinaryValue(sc.getLatchState(index, state));
                } else if (attributeId == OneWirePointLocatorVO.AttributeTypes.WIPER_POSITION) {
                    PotentiometerContainer pc = (PotentiometerContainer) owc;
                    pc.setCurrentWiperNumber(index, state);
                    result = new MultistateValue(pc.getWiperPosition());
                } else if (attributeId == OneWirePointLocatorVO.AttributeTypes.COUNTER) {
                    OneWireContainer1D c1d = (OneWireContainer1D) owc;
                    result = new NumericValue(c1d.readCounter(index));
                }

                if (result != null) {
                    // The value was correctly extracted from the device.
                    if (result.getDataType() != locator.getDataType()) // Huh?
                    {
                        throw new ShouldNeverHappenException("Got " + result.getDataType() + ", expected "
                                + locator.getDataType());
                    }

                    // Update the data image with the new value.
                    point.updatePointValue(new PointValueTime(result, time));

                    // Remove this point from the list.
                    iter.remove();
                }

                // If the value was not properly retrieved, the point stays in the point list, and so will
                // be reported as an event.
            }
        }
    }

    @Override
    public void setPointValue(DataPointRT dataPoint, PointValueTime valueTime, SetPointSource source) {
        throw new ImplementMeException();
        /*
         LocalizableMessage exceptionMessage = null;

         Network localNetwork = network;
         if (localNetwork == null) {
         return;
         }

         // Ensure that the write doesn't conflict with a read.
         synchronized (pointListChangeLock) {
         OneWirePointLocatorRT locator = dataPoint.getPointLocator();

         NetworkPath path = null;
         try {
         localNetwork.lock();

         path = localNetwork.getNetworkPath(locator.getAddress());
         if (path == null) {
         exceptionMessage = new LocalizableMessageImpl("event.1wire.noDevice", Address.toString(locator
         .getAddress()), dataPoint.getVoName());
         } else {
         path.open();

         int attributeId = locator.getVo().getAttributeId();
         int index = locator.getVo().getIndex();

         if (attributeId == OneWirePointLocatorVO.AttributeTypes.LATCH_STATE) {
         SwitchContainer sc = (SwitchContainer) path.getTarget();
         byte[] state = sc.readDevice();
         boolean value = valueTime.getBooleanValue();
         sc.setLatchState(index, value, sc.hasSmartOn(), state);
         sc.writeDevice(state);
         } else if (attributeId == OneWirePointLocatorVO.AttributeTypes.WIPER_POSITION) {
         PotentiometerContainer pc = (PotentiometerContainer) path.getTarget();
         byte[] state = pc.readDevice();
         int value = valueTime.getIntegerValue();
         pc.setCurrentWiperNumber(index, state);
         boolean success = pc.setWiperPosition(value);
         if (success) {
         pc.writeDevice(state);
         } else {
         exceptionMessage = new LocalizableMessageImpl("event.1wire.setWiper", Address.toString(locator
         .getAddress()), dataPoint.getVoName());
         }
         }
         }
         } catch (Exception e) {
         exceptionMessage = wrapSerialException(e, vo.getCommPortId());
         } finally {
         try {
         if (path != null) {
         path.close();
         }
         } catch (Exception e) {
         // no op
         }

         localNetwork.unlock();
         }

         // Event handling.
         if (exceptionMessage != null) {
         raiseEvent(POINT_WRITE_EXCEPTION_EVENT, System.currentTimeMillis(), false, exceptionMessage);
         } else {
         dataPoint.setPointValue(valueTime, source);
         }
         }
         */
    }

    //
    // /
    // / Lifecycle
    // /
    //
    @Override
    public void initialize() {
        initializeNetwork();
        super.initialize();
    }

    @Override
    public void terminate() {
        super.terminate();
        terminateNetwork();
    }

    private void initializeNetwork() {
        try {
            network = new Network(vo.getCommPortId());
            try {
                network.lock();
                network.quickInitialize();
            } finally {
                network.unlock();
            }

            // Deactivate any existing event.
            returnToNormal(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis());
        } catch (Exception e) {
            LOG.info("", e);
            raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis(), true,
                    wrapSerialException(e, vo.getCommPortId()));
            terminateNetwork();
        }
    }

    private void terminateNetwork() {
        if (network != null) {
            try {
                network.terminate();
            } catch (OneWireException e1) {
                // no op
            }
            network = null;
        }
    }

    class NetworkReloadException extends LocalizableException {

        private static final long serialVersionUID = -1;

        public NetworkReloadException(String i18nKey, Object... args) {
            super(i18nKey, args);
        }
    }

    private void updateNextRescan(long time) {
        nextRescan = time + vo.getRescanPeriodType().getMillis(vo.getRescanPeriods());
    }

    @Override
    protected CronExpression getCronExpression() throws ParseException {
        throw new ImplementMeException();
    }
}
