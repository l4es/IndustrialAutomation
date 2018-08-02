/*
 *   Mango - Open Source M2M - http://mango.serotoninsoftware.com
 *   Copyright (C) 2010 Arne Pl\u00f6se
 *   @author Arne Pl\u00f6se
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
package br.org.scadabr.rt.datasource.fhz4j;

import br.org.scadabr.DataType;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.utils.TimePeriods;
import br.org.scadabr.vo.datasource.fhz4j.EmPointLocator;
import br.org.scadabr.vo.datasource.fhz4j.FS20PointLocator;
import br.org.scadabr.vo.datasource.fhz4j.FhtMultiMsgPointLocator;
import br.org.scadabr.vo.datasource.fhz4j.FhtPointLocator;
import br.org.scadabr.vo.datasource.fhz4j.Fhz4JDataSourceVO;
import br.org.scadabr.vo.datasource.fhz4j.Fhz4JPointLocatorVO;
import br.org.scadabr.vo.datasource.fhz4j.HmsPointLocator;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.LoggingTypes;
import java.util.ArrayList;
import java.util.EnumMap;
import java.util.HashMap;
import java.util.Map;
import net.sf.fhz4j.FhzDataListener;
import net.sf.fhz4j.FhzParser;
import net.sf.fhz4j.FhzProtocol;
import net.sf.fhz4j.FhzWriter;
import net.sf.fhz4j.fht.FhtMessage;
import net.sf.fhz4j.fht.FhtProperty;
import net.sf.fhz4j.hms.HmsMessage;
import net.sf.fhz4j.hms.HmsProperty;
import net.sf.fhz4j.scada.ScadaProperty;
import net.sf.fhz4j.scada.ScadaValueAccessor;

import com.serotonin.mango.Common;
import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.SetPointSource;
import com.serotonin.mango.rt.dataSource.DataSourceRT;
import com.serotonin.mango.view.chart.ImageChartRenderer;
import com.serotonin.mango.view.text.AnalogRenderer;
import com.serotonin.mango.view.text.BinaryTextRenderer;
import com.serotonin.mango.view.text.MultistateRenderer;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.event.PointEventDetectorVO;
import java.awt.Color;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.LogRecord;
import java.util.logging.Logger;
import net.sf.atmodem4j.spsw.SerialPortSocket;
import net.sf.fhz4j.em.EmMessage;
import net.sf.fhz4j.em.EmProperty;
import net.sf.fhz4j.fht.Fht80bModes;
import net.sf.fhz4j.fht.Fht80bWarnings;
import net.sf.fhz4j.fht.FhtMultiMsgMessage;
import net.sf.fhz4j.fht.FhtMultiMsgProperty;
import net.sf.fhz4j.fs20.FS20Message;
import net.sf.fhz4j.fs20.FS20CommandValues;
import net.sf.fhz4j.fs20.FS20DeviceType;

/**
 *
 * TODO datatype NUMERIC_INT is missing TODO Starttime for timpepoints ???
 *
 */
public class Fhz4JDataSourceRT extends DataSourceRT<Fhz4JDataSourceVO> implements FhzDataListener {

    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCARABR_DS_FHZ4J);
    public static final int DATA_SOURCE_EXCEPTION_EVENT = 1;
    public static final int POINT_READ_EXCEPTION_EVENT = 2;
    public static final int POINT_WRITE_EXCEPTION_EVENT = 3;
    // private final long nextRescan = 0;
    private SerialPortSocket sPort;
    private FhzParser parser;
    private FhzWriter writer;

    private final Map<Short, Map<EmProperty, DataPointRT>> emPoints = new HashMap();
    private final Map<Short, Map<Byte, DataPointRT>> fs20Points = new HashMap();
    private final Map<Short, Map<HmsProperty, DataPointRT>> hmsPoints = new HashMap();
    private final Map<Short, Map<FhtProperty, DataPointRT>> fhtPoints = new HashMap();
    private final Map<Short, Map<FhtMultiMsgProperty, DataPointRT>> fhtMultiMsgPoints = new HashMap();

    private final Map<Short, Map<EmProperty, DataPointVO>> emDisabledPoints = new HashMap();
    private final Map<Short, Map<Byte, DataPointVO>> fs20DisabledPoints = new HashMap();
    private final Map<Short, Map<HmsProperty, DataPointVO>> hmsDisabledPoints = new HashMap();
    private final Map<Short, Map<FhtProperty, DataPointVO>> fhtDisabledPoints = new HashMap();
    private final Map<Short, Map<FhtMultiMsgProperty, DataPointVO>> fhtMultiMsgDisabledPoints = new HashMap();

    public Fhz4JDataSourceRT(Fhz4JDataSourceVO vo) {
        super(vo, true);
    }

    @Override
    public void initialize() {
        parser = new FhzParser(this);
        try {
            sPort = FhzParser.openPort(vo.getCommPort());
            parser.setInputStream(sPort.getInputStream());
            writer = new FhzWriter();
            writer.setOutputStream(sPort.getOutputStream());
            try {
                Thread.sleep(100);
            } catch (InterruptedException ex) {
            }
            writer.initFhz(vo.getFhzHousecode());
            if (vo.isFhtMaster()) {
                writer.initFhtReporting(getFhtDeviceHousecodes());
                writer.syncFhtClocks(getFhtDeviceHousecodes());
            }
            returnToNormal(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis());
        } catch (IOException ex) {
            raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis(), true,
                    new LocalizableMessageImpl("event.exception2", vo.getName(), ex.getMessage()));
            final LogRecord lr = new LogRecord(Level.SEVERE, "FHZ open serialport: {0}");
            lr.setParameters(new Object[]{sPort.getPortName()});
            lr.setThrown(ex);
            LOG.log(lr);
        }
        super.initialize();
    }

    @Override
    public void terminate() {
        try {
            parser.close();
            if (sPort != null) {
                sPort.close();
            }
        } catch (InterruptedException | IOException ex) {
            raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, System.currentTimeMillis(), true,
                    new LocalizableMessageImpl("event.exception2", vo.getName(), ex.getMessage()));
            final LogRecord lr = new LogRecord(Level.SEVERE, "FHZ serialport: {0} unexpected closed");
            lr.setParameters(new Object[]{sPort.getPortName()});
            lr.setThrown(ex);
            LOG.log(lr);
        }
        super.terminate();
    }

    @Override
    public void dataPointEnabled(DataPointRT dataPoint) {
        addToEnabledDataPoint(dataPoint);
        removeFromDisabledDataPoint(dataPoint.getVo());
    }

    @Override
    public void dataPointDisabled(DataPointVO dataPoint) {
        removeFromEnabledDataPoint(dataPoint);
        addToDisabledDataPoint(dataPoint);
    }

    @Override
    public void dataPointDeleted(DataPointVO point) {
        removeFromDisabledDataPoint(point);
    }

    private void addToEnabledDataPoint(DataPointRT dataPoint) {
        synchronized (dataPointsCacheLock) {
            final Fhz4JPointLocatorRT locator = (Fhz4JPointLocatorRT) dataPoint.getPointLocator();
            switch (locator.getFhzProtocol()) {
                case FS20:
                    FS20PointLocator fs20Locator = (FS20PointLocator) locator.getVo().getProtocolLocator();
                    Map<Byte, DataPointRT> fs20PropertyMap = fs20Points.get(fs20Locator.getHousecode());
                    if (fs20PropertyMap == null) {
                        fs20PropertyMap = new HashMap<>();
                        fs20Points.put(fs20Locator.getHousecode(), fs20PropertyMap);
                    }
                    fs20PropertyMap.put(fs20Locator.getOffset(), dataPoint);
                    break;
                case EM:
                    EmPointLocator emLocator = (EmPointLocator) locator.getVo().getProtocolLocator();
                    Map<EmProperty, DataPointRT> emPropertyMap = emPoints.get(emLocator.getAddress());
                    if (emPropertyMap == null) {
                        emPropertyMap = new EnumMap<>(EmProperty.class);
                        emPoints.put(emLocator.getAddress(), emPropertyMap);
                    }
                    emPropertyMap.put(emLocator.getProperty(), dataPoint);
                    break;
                case FHT:
                    FhtPointLocator fhtLocator = (FhtPointLocator) locator.getVo().getProtocolLocator();
                    Map<FhtProperty, DataPointRT> fhzPropertyMap = fhtPoints.get(fhtLocator.getHousecode());
                    if (fhzPropertyMap == null) {
                        fhzPropertyMap = new EnumMap<>(FhtProperty.class);
                        fhtPoints.put(fhtLocator.getHousecode(), fhzPropertyMap);
                    }
                    fhzPropertyMap.put(fhtLocator.getProperty(), dataPoint);
                    break;
                case HMS:
                    HmsPointLocator hmsLocator = (HmsPointLocator) locator.getVo().getProtocolLocator();
                    Map<HmsProperty, DataPointRT> hmsPropertyMap = hmsPoints.get(hmsLocator.getHousecode());
                    if (hmsPropertyMap == null) {
                        hmsPropertyMap = new EnumMap<>(HmsProperty.class);
                        hmsPoints.put(hmsLocator.getHousecode(), hmsPropertyMap);
                    }
                    hmsPropertyMap.put(hmsLocator.getProperty(), dataPoint);
                    break;
                case FHT_MULTI_MSG:
                    FhtMultiMsgPointLocator fhtMultiMsgLocator = (FhtMultiMsgPointLocator) locator.getVo().getProtocolLocator();
                    Map<FhtMultiMsgProperty, DataPointRT> fhtMultiMsgPropertyMap = fhtMultiMsgPoints.get(fhtMultiMsgLocator.getHousecode());
                    if (fhtMultiMsgPropertyMap == null) {
                        fhtMultiMsgPropertyMap = new EnumMap<>(FhtMultiMsgProperty.class);
                        fhtMultiMsgPoints.put(fhtMultiMsgLocator.getHousecode(), fhtMultiMsgPropertyMap);
                    }
                    fhtMultiMsgPropertyMap.put(fhtMultiMsgLocator.getProperty(), dataPoint);
                    break;
                default:
                    throw new RuntimeException("Unknown fhz protocol");
            }
        }
    }

    /**
     * let the runtime know of all disabled DataSources
     *
     * @param vo
     */
    public void addToDisabledDataPoint(DataPointVO vo) {
        synchronized (dataPointsCacheLock) {
            final Fhz4JPointLocatorVO locator = (Fhz4JPointLocatorVO) vo.getPointLocator();
            switch (locator.getFhzProtocol()) {
                case FS20:
                    FS20PointLocator fs20Locator = (FS20PointLocator) locator.getProtocolLocator();
                    Map<Byte, DataPointVO> fs20PropertyMap = fs20DisabledPoints.get(fs20Locator.getHousecode());
                    if (fs20PropertyMap == null) {
                        fs20PropertyMap = new HashMap<>();
                        fs20DisabledPoints.put(fs20Locator.getHousecode(), fs20PropertyMap);
                    }
                    fs20PropertyMap.put(fs20Locator.getOffset(), vo);
                    break;
                case EM:
                    EmPointLocator emLocator = (EmPointLocator) locator.getProtocolLocator();
                    Map<EmProperty, DataPointVO> emPropertyMap = emDisabledPoints.get(emLocator.getAddress());
                    if (emPropertyMap == null) {
                        emPropertyMap = new EnumMap<>(EmProperty.class);
                        emDisabledPoints.put(emLocator.getAddress(), emPropertyMap);
                    }
                    emPropertyMap.put(emLocator.getProperty(), vo);
                    break;
                case FHT:
                    FhtPointLocator fhtLocator = (FhtPointLocator) locator.getProtocolLocator();
                    Map<FhtProperty, DataPointVO> fhtPropertyMap = fhtDisabledPoints.get(fhtLocator.getHousecode());
                    if (fhtPropertyMap == null) {
                        fhtPropertyMap = new EnumMap<>(FhtProperty.class);
                        fhtDisabledPoints.put(fhtLocator.getHousecode(), fhtPropertyMap);
                    }
                    fhtPropertyMap.put(fhtLocator.getProperty(), vo);
                    break;
                case HMS:
                    HmsPointLocator hmsLocator = (HmsPointLocator) locator.getProtocolLocator();
                    Map<HmsProperty, DataPointVO> hmsPropertyMap = hmsDisabledPoints.get(hmsLocator.getHousecode());
                    if (hmsPropertyMap == null) {
                        hmsPropertyMap = new EnumMap<>(HmsProperty.class);
                        hmsDisabledPoints.put(hmsLocator.getHousecode(), hmsPropertyMap);
                    }
                    hmsPropertyMap.put(hmsLocator.getProperty(), vo);
                    break;
                case FHT_MULTI_MSG:
                    FhtMultiMsgPointLocator fhtMultiMsgLocator = (FhtMultiMsgPointLocator) locator.getProtocolLocator();
                    Map<FhtMultiMsgProperty, DataPointVO> fhtMultiMsgPropertyMap = fhtMultiMsgDisabledPoints.get(fhtMultiMsgLocator.getHousecode());
                    if (fhtMultiMsgPropertyMap == null) {
                        fhtMultiMsgPropertyMap = new EnumMap<>(FhtMultiMsgProperty.class);
                        fhtMultiMsgDisabledPoints.put(fhtMultiMsgLocator.getHousecode(), fhtMultiMsgPropertyMap);
                    }
                    fhtMultiMsgPropertyMap.put(fhtMultiMsgLocator.getProperty(), vo);
                    break;
                default:
                    throw new RuntimeException("Unknown fhz protocol");
            }
        }
    }

    public void removeFromEnabledDataPoint(DataPointVO dataPoint) {
        synchronized (dataPointsCacheLock) {
            final Fhz4JPointLocatorVO locator = (Fhz4JPointLocatorVO) dataPoint.getPointLocator();
            switch (locator.getFhzProtocol()) {
                case FS20:
                    FS20PointLocator fs20Locator = (FS20PointLocator) locator.getProtocolLocator();
                    Map<Byte, DataPointRT> fs20PropertyMap = fs20Points.get(fs20Locator.getHousecode());
                    fs20PropertyMap.remove(fs20Locator.getOffset());
                    if (fs20PropertyMap.isEmpty()) {
                        fhtPoints.remove(fs20Locator.getHousecode());
                    }
                    break;
                case EM:
                    EmPointLocator emLocator = (EmPointLocator) locator.getProtocolLocator();
                    Map<EmProperty, DataPointRT> emPropertyMap = emPoints.get(emLocator.getAddress());
                    emPropertyMap.remove(emLocator.getProperty());
                    if (emPropertyMap.isEmpty()) {
                        emPoints.remove(emLocator.getAddress());
                    }
                    break;
                case FHT:
                    FhtPointLocator fhtLocator = (FhtPointLocator) locator.getProtocolLocator();
                    Map<FhtProperty, DataPointRT> fhtPropertyMap = fhtPoints.get(fhtLocator.getHousecode());
                    if (fhtPropertyMap == null) {
                        return;//TODO Whats going on here ???
                    }
                    fhtPropertyMap.remove(fhtLocator.getProperty());
                    if (fhtPropertyMap.isEmpty()) {
                        fhtPoints.remove(fhtLocator.getHousecode());
                    }
                    break;
                case HMS:
                    HmsPointLocator hmsLocator = (HmsPointLocator) locator.getProtocolLocator();
                    Map<HmsProperty, DataPointRT> hmsPropertyMap = hmsPoints.get(hmsLocator.getHousecode());
                    hmsPropertyMap.remove(hmsLocator.getProperty());
                    if (hmsPropertyMap.isEmpty()) {
                        hmsPoints.remove(hmsLocator.getHousecode());
                    }
                    break;
                case FHT_MULTI_MSG:
                    FhtMultiMsgPointLocator fhtMultiMsgLocator = (FhtMultiMsgPointLocator) locator.getProtocolLocator();
                    Map<FhtMultiMsgProperty, DataPointRT> fhtMultiMsgPropertyMap = fhtMultiMsgPoints.get(fhtMultiMsgLocator.getHousecode());
                    fhtMultiMsgPropertyMap.remove(fhtMultiMsgLocator.getProperty());
                    if (fhtMultiMsgPropertyMap.isEmpty()) {
                        fhtPoints.remove(fhtMultiMsgLocator.getHousecode());
                    }
                    break;
                default:
                    throw new ShouldNeverHappenException("Unknown fhz protocol");
            }
        }
    }

    private void removeFromDisabledDataPoint(DataPointVO vo) {
        synchronized (dataPointsCacheLock) {
            final Fhz4JPointLocatorVO locator = (Fhz4JPointLocatorVO) vo.getPointLocator();
            switch (locator.getFhzProtocol()) {
                case FS20:
                    FS20PointLocator fs20Locator = (FS20PointLocator) locator.getProtocolLocator();
                    Map<Byte, DataPointVO> fs20PropertyMap = fs20DisabledPoints.get(fs20Locator.getHousecode());
                    if (fs20PropertyMap == null) {
                        return;
                    }
                    fs20PropertyMap.remove(fs20Locator.getOffset());
                    if (fs20PropertyMap.isEmpty()) {
                        fs20DisabledPoints.remove(fs20Locator.getHousecode());
                    }
                    break;
                case EM:
                    EmPointLocator emLocator = (EmPointLocator) locator.getProtocolLocator();
                    Map<EmProperty, DataPointVO> emPropertyMap = emDisabledPoints.get(emLocator.getAddress());
                    if (emPropertyMap == null) {
                        return;
                    }
                    emPropertyMap.remove(emLocator.getProperty());
                    if (emPropertyMap.isEmpty()) {
                        emDisabledPoints.remove(emLocator.getAddress());
                    }
                    break;
                case FHT:
                    FhtPointLocator fhtLocator = (FhtPointLocator) locator.getProtocolLocator();
                    Map<FhtProperty, DataPointVO> fhtPropertyMap = fhtDisabledPoints.get(fhtLocator.getHousecode());
                    if (fhtPropertyMap == null) {
                        return;
                    }
                    fhtPropertyMap.remove(fhtLocator.getProperty());
                    if (fhtPropertyMap.isEmpty()) {
                        fhtDisabledPoints.remove(fhtLocator.getHousecode());
                    }
                    break;
                case HMS:
                    HmsPointLocator hmsLocator = (HmsPointLocator) locator.getProtocolLocator();
                    Map<HmsProperty, DataPointVO> hmsPropertyMap = hmsDisabledPoints.get(hmsLocator.getHousecode());
                    if (hmsPropertyMap == null) {
                        return;
                    }
                    hmsPropertyMap.remove(hmsLocator.getProperty());
                    if (hmsPropertyMap.isEmpty()) {
                        hmsDisabledPoints.remove(hmsLocator.getHousecode());
                    }
                    break;
                case FHT_MULTI_MSG:
                    FhtMultiMsgPointLocator fhtMultiMsgLocator = (FhtMultiMsgPointLocator) locator.getProtocolLocator();
                    Map<FhtMultiMsgProperty, DataPointVO> fhtMultiMsgPropertyMap = fhtMultiMsgDisabledPoints.get(fhtMultiMsgLocator.getHousecode());
                    if (fhtMultiMsgPropertyMap == null) {
                        return;
                    }
                    fhtMultiMsgPropertyMap.remove(fhtMultiMsgLocator.getProperty());
                    if (fhtMultiMsgPropertyMap.isEmpty()) {
                        fhtMultiMsgDisabledPoints.remove(fhtMultiMsgLocator.getHousecode());
                    }
                    break;
                default:
                    throw new ShouldNeverHappenException("Unknown fhz protocol");
            }
        }
    }

    @Override
    public void setPointValue(DataPointRT dataPoint, PointValueTime valueTime, SetPointSource source) {
        try {
            final Fhz4JPointLocatorRT locator = (Fhz4JPointLocatorRT) dataPoint.getPointLocator();
            switch (locator.getFhzProtocol()) {
                case FHT:
                    setFhtValue(valueTime, (FhtPointLocator) locator.getVo().getProtocolLocator());
                    break;
                default:
                    throw new ShouldNeverHappenException("Cant set point");
            }
        } catch (IOException e) {
            raiseEvent(POINT_WRITE_EXCEPTION_EVENT, System.currentTimeMillis(), true, null); //TODO
        }
    }

    public void addFoundFhtMessage(FhtMessage fhtMessage) {

        // this value was not found, so create one
        final Fhz4JPointLocatorVO<FhtProperty> fhzLocator = (Fhz4JPointLocatorVO<FhtProperty>) vo.createPointLocator(FhzProtocol.FHT);
        final FhtPointLocator fhtLocator = (FhtPointLocator) fhzLocator.getProtocolLocator();
        fhtLocator.setHousecode(fhtMessage.getHousecode());
        fhtLocator.setFhtDeviceType(fhtMessage.getCommand().getTagetDevice());
        fhtLocator.setProperty(fhtMessage.getCommand());
        fhtLocator.setSettable(false);

        DataPointVO dp = new DataPointVO();
        dp.setName(fhtLocator.defaultName());
        dp.setDataSourceId(vo.getId());
        dp.setXid(String.format("FHT-%04x-%s", fhtMessage.getHousecode(), fhtMessage.getCommand().getName()));
        dp.setPointLocator(fhzLocator);
        dp.setEnabled(true);
        dp.setLoggingType(LoggingTypes.ALL);
        dp.setEventDetectors(new ArrayList<PointEventDetectorVO>());

        switch (dp.getDataType()) {
            case NUMERIC:
                dp.setTextRenderer(new AnalogRenderer("#,##0.0", fhtMessage.getCommand().getUnitOfMeasurement()));
                dp.setChartRenderer(new ImageChartRenderer(TimePeriods.DAYS, 1));
                break;
            case MULTISTATE:
                MultistateRenderer mr = new MultistateRenderer();
                switch (fhtLocator.getProperty()) {
                    case MODE:
                        mr.addMultistateValue(Fht80bModes.AUTO.getValue(), Fht80bModes.AUTO.getLabel(), Color.GREEN.darker());
                        mr.addMultistateValue(Fht80bModes.MANUAL.getValue(), Fht80bModes.MANUAL.getLabel(), Color.BLACK);
                        mr.addMultistateValue(Fht80bModes.HOLIDAY.getValue(), Fht80bModes.HOLIDAY.getLabel(), Color.RED);
                        mr.addMultistateValue(Fht80bModes.PARTY.getValue(), Fht80bModes.PARTY.getLabel(), Color.RED);
                        break;
                    case WARNINGS:
                        mr.addMultistateValue(Fht80bWarnings.NONE.getValue(), Fht80bWarnings.NONE.getLabel(), Color.GREEN.darker());
                        mr.addMultistateValue(Fht80bWarnings.BATT_LOW.getValue(), Fht80bWarnings.BATT_LOW.getLabel(), Color.RED);
                        break;
                    default:
                }
                dp.setTextRenderer(mr);
                dp.setChartRenderer(new ImageChartRenderer(TimePeriods.DAYS, 1));
                break;
            default:
        }

        DataPointRT dataPointRT = Common.ctx.getRuntimeManager().saveDataPoint(dp);
        Common.ctx.getRuntimeManager().addPointToHierarchy(dp, vo.getName(), "FHT", fhtMessage.getHousecodeStr());

        if (dataPointRT != null) {
            updateValue(dataPointRT, fhtMessage, fhtMessage.getCommand());
        }
        LOG.log(Level.INFO, "FHT point added: {0}", dp.getXid());
    }

    public void addFoundFs20Message(FS20Message fs20Message) {

        // this value was not found, so create one
        final Fhz4JPointLocatorVO<FS20DeviceType> fhzLocator = (Fhz4JPointLocatorVO<FS20DeviceType>) vo.createPointLocator(FhzProtocol.FS20);
        final FS20PointLocator fs20Locator = (FS20PointLocator) fhzLocator.getProtocolLocator();
        fs20Locator.setHousecode(fs20Message.getHousecode());
        fs20Locator.setProperty(fs20Message.getDeviceType());
        fs20Locator.setOffset(fs20Message.getOffset());
        fs20Locator.setDeviceType(fs20Message.getDeviceType());
        fs20Locator.setSettable(false);

        DataPointVO dp = new DataPointVO();
        dp.setName(fs20Locator.defaultName());
        dp.setDataSourceId(vo.getId());
        dp.setXid(String.format("%04x-%d-%s", fs20Message.getHousecode(), fs20Message.getOffset(), fs20Message.getDeviceType().getName()));
        dp.setPointLocator(fhzLocator);
        dp.setEnabled(true);
        dp.setLoggingType(LoggingTypes.ALL);
        dp.setEventDetectors(new ArrayList<PointEventDetectorVO>());

        if (dp.getPointLocator().getDataType() == DataType.MULTISTATE) {
            MultistateRenderer mr = new MultistateRenderer();
            mr.addMultistateValue(FS20CommandValues.OFF.getValue(), FS20CommandValues.OFF.getLabel(), Color.RED);
            mr.addMultistateValue(FS20CommandValues.DIM_DOWN.getValue(), FS20CommandValues.DIM_DOWN.getLabel(), Color.RED.brighter().brighter());
            mr.addMultistateValue(FS20CommandValues.DIM_UP.getValue(), FS20CommandValues.DIM_UP.getLabel(), Color.GREEN.brighter().brighter());
            mr.addMultistateValue(FS20CommandValues.ON.getValue(), FS20CommandValues.ON.getLabel(), Color.GREEN);

            dp.setTextRenderer(mr);
            dp.setChartRenderer(new ImageChartRenderer(TimePeriods.DAYS, 1));
        }

        DataPointRT dataPointRT = Common.ctx.getRuntimeManager().saveDataPoint(dp);
        Common.ctx.getRuntimeManager().addPointToHierarchy(dp, vo.getName(), "FS20", String.format("%04x", fs20Message.getHousecode()));

        if (dataPointRT != null) {
            updateValue(dataPointRT, fs20Message, fs20Message.getDeviceType());
        }
        LOG.log(Level.INFO, "FS20 point added: {0}", dp.getXid());
    }

    private void addFoundEmDataPoint(EmMessage emMessage, EmProperty prop) {

        // this value was not found, so create one
        Fhz4JPointLocatorVO<EmProperty> fhzLocator = (Fhz4JPointLocatorVO<EmProperty>) vo.createPointLocator(FhzProtocol.EM);

        EmPointLocator emLocator = (EmPointLocator) fhzLocator.getProtocolLocator();
        emLocator.setAddress(emMessage.getAddress());
        emLocator.setProperty(prop);
        emLocator.setSettable(false);

        DataPointVO dp = new DataPointVO();
        dp.setName(emLocator.defaultName());
        dp.setXid(String.format("%04x-%s", emMessage.getAddress(), prop.getName()));
        dp.setDataSourceId(vo.getId());
        dp.setEnabled(true);
        dp.setLoggingType(LoggingTypes.ALL);
        dp.setEventDetectors(new ArrayList<PointEventDetectorVO>());

        dp.setPointLocator(fhzLocator);
        if (dp.getPointLocator().getDataType() == DataType.NUMERIC) {
            dp.setTextRenderer(new AnalogRenderer("#,##0.0", prop.getUnitOfMeasurement()));
            dp.setChartRenderer(new ImageChartRenderer(TimePeriods.DAYS, 1));
        }

        DataPointRT dataPointRT = Common.ctx.getRuntimeManager().saveDataPoint(dp);
        Common.ctx.getRuntimeManager().addPointToHierarchy(dp, vo.getName(), "EM", String.format("%04d", emLocator.getAddress()));

        if (dataPointRT != null) {
            updateValue(dataPointRT, emMessage, prop);
        }
        LOG.log(Level.SEVERE, "Em point added: {0}", dp.getXid());
    }

    private Iterable<Short> getFhtDeviceHousecodes() {
        return fhtPoints.keySet();
    }

    @Override
    public void emDataParsed(EmMessage emMsg) {
        LOG.log(Level.INFO, "EM SignalStrength : {0} {1}", new Object[]{String.format("%04X", emMsg.getAddress()), emMsg.getSignalStrength()});
        try {
            Map<EmProperty, DataPointRT> emDataPoints = getEmPoints(emMsg);
            for (EmProperty prop : emMsg.getSupportedProperties()) {
                DataPointRT dataPoint = emDataPoints == null ? null : emDataPoints.get(prop);
                if (dataPoint == null) {
                    if (getEmDisabledPoint(emMsg, prop) == null) {
                        addFoundEmDataPoint(emMsg, prop);
                    }
                } else {
                    updateValue(dataPoint, emMsg, prop);
                }
            }
            returnToNormal(POINT_READ_EXCEPTION_EVENT, System.currentTimeMillis());
        } catch (Throwable t) {
            raiseEvent(POINT_READ_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl("event.exception2", vo.getName(), t.getMessage()));
            final LogRecord lr = new LogRecord(Level.SEVERE, "FHZ em parsed: {0}");
            lr.setParameters(new Object[]{emMsg});
            lr.setThrown(t);
            LOG.log(lr);
        }
    }

    @Override
    public void fs20DataParsed(FS20Message fs20Msg) {
        LOG.log(Level.INFO, "FHT SignalStrength : {0} {1}", new Object[]{fs20Msg.getHousecodeStr(), fs20Msg.getSignalStrength()});

        try {
            DataPointRT dataPoint = getFs20Point(fs20Msg);
            if (dataPoint != null) {
                updateValue(dataPoint, fs20Msg, fs20Msg.getDeviceType());
            } else {
                if (getFs20DisabledPoint(fs20Msg) == null) {
                    LOG.log(Level.INFO, "NEW FS20 property detected, wil add: {0}", fs20Msg);
                    addFoundFs20Message(fs20Msg);
                }
            }
            returnToNormal(POINT_READ_EXCEPTION_EVENT, System.currentTimeMillis());
        } catch (Throwable t) {
            raiseEvent(POINT_READ_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl("event.exception2", vo.getName(), t.getMessage()));
            final LogRecord lr = new LogRecord(Level.SEVERE, "FHZ fs20 parsed: {0}");
            lr.setParameters(new Object[]{fs20Msg});
            lr.setThrown(t);
            LOG.log(lr);
        }
    }

    @Override
    public void fhtDataParsed(FhtMessage fhtMessage) {
        LOG.log(Level.INFO, "FHT SignalStrength : {0} {1}", new Object[]{fhtMessage.getHousecodeStr(), fhtMessage.getSignalStrength()});
        if (fhtMessage.getCommand() == FhtProperty.MEASURED_LOW || fhtMessage.getCommand() == FhtProperty.MEASURED_HIGH) {
            // Just ignore this...
            return;
        }
        try {
            DataPointRT dataPoint = getFhtPoint(fhtMessage);
            if (dataPoint != null) {
                updateValue(dataPoint, fhtMessage, fhtMessage.getCommand());
            } else {
                if (getFhtDisabledPoint(fhtMessage) == null) {
                    LOG.log(Level.INFO, "NEW FHT property detected, wil add: {0}", fhtMessage);
                    addFoundFhtMessage(fhtMessage);
                }
            }
            returnToNormal(POINT_READ_EXCEPTION_EVENT, System.currentTimeMillis());
        } catch (Throwable t) {
            raiseEvent(POINT_READ_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl("event.exception2", vo.getName(), t.getMessage()));
            final LogRecord lr = new LogRecord(Level.SEVERE, "FHZ fht parsed: {0}");
            lr.setParameters(new Object[]{fhtMessage});
            lr.setThrown(t);
            LOG.log(lr);
        }
    }

    @Override
    public void fhtMultiMsgParsed(FhtMultiMsgMessage fhtMultiMsgMessage) {
        try {
            DataPointRT dataPoint = getFhtMultiMsgPoint(fhtMultiMsgMessage);
            if (dataPoint != null) {
                updateValue(dataPoint, fhtMultiMsgMessage, fhtMultiMsgMessage.getProperty());
            } else {
                if (getFhtMultiMsgDisabledPoint(fhtMultiMsgMessage) == null) {
                    LOG.log(Level.SEVERE, "NEW Fht property detected, wil add: {0}", fhtMultiMsgMessage);
                    addFoundFhtMultiMsgMessage(fhtMultiMsgMessage);
                }
            }
            returnToNormal(POINT_READ_EXCEPTION_EVENT, System.currentTimeMillis());
        } catch (Throwable t) {
            raiseEvent(POINT_READ_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl("event.exception2", vo.getName(), t.getMessage()));
            final LogRecord lr = new LogRecord(Level.SEVERE, "FHZ fht multi message parsed: {0}");
            lr.setParameters(new Object[]{fhtMultiMsgMessage});
            lr.setThrown(t);
            LOG.log(lr);
        }
    }

    @Override
    public void hmsDataParsed(HmsMessage hmsMessage) {
        LOG.log(Level.INFO, "HMS SignalStrength : {0} {1}", new Object[]{String.format("%04X", hmsMessage.getHousecode()), hmsMessage.getSignalStrength()});
        try {
            Map<HmsProperty, DataPointRT> hmsDataPoints = getHmsPoints(hmsMessage);
            for (HmsProperty prop : hmsMessage.getSupportedProperties()) {
                DataPointRT dataPoint = hmsDataPoints == null ? null : hmsDataPoints.get(prop);
                if (dataPoint == null) {
                    if (getHmsDisabledPoint(hmsMessage, prop) == null) {
                        addFoundHmsDataPoint(hmsMessage, prop);
                    }
                } else {
                    updateValue(dataPoint, hmsMessage, prop);
                }
            }
            returnToNormal(POINT_READ_EXCEPTION_EVENT, System.currentTimeMillis());
        } catch (Throwable t) {
            raiseEvent(POINT_READ_EXCEPTION_EVENT, System.currentTimeMillis(), true, new LocalizableMessageImpl("event.exception2", vo.getName(), t.getMessage()));
            final LogRecord lr = new LogRecord(Level.SEVERE, "FHZ hms parsed: {0}");
            lr.setParameters(new Object[]{hmsMessage});
            lr.setThrown(t);
            LOG.log(lr);
        }
    }

    private void updateValue(DataPointRT point, ScadaValueAccessor propertyProvider, ScadaProperty prop) {
        final long time = System.currentTimeMillis();
        switch (prop.getDataType()) {
            case BOOLEAN:
                point.updatePointValue(new PointValueTime(propertyProvider.getBoolean(prop), time));
                break;
            case BYTE:
                point.updatePointValue(new PointValueTime(propertyProvider.getByte(prop), time));
                break;
            case CHAR:
                point.updatePointValue(new PointValueTime(propertyProvider.getChar(prop), time));
                break;
            case DATE:
                if (true) {
                    throw new ImplementMeException();
                }
//                point.updatePointValue(new PointValueTime(propertyProvider.asString(prop), time));
                break;
            case DOUBLE:
                point.updatePointValue(new PointValueTime(propertyProvider.getDouble(prop), time));
                break;
            case FLOAT:
                point.updatePointValue(new PointValueTime(propertyProvider.getFloat(prop), time));
                break;
            case INT:
                point.updatePointValue(new PointValueTime(propertyProvider.getInt(prop), time));
                break;
            case LONG:
                point.updatePointValue(new PointValueTime(propertyProvider.getLong(prop), time));
                break;
            case SHORT:
                point.updatePointValue(new PointValueTime(propertyProvider.getShort(prop), time));
                break;
            case STRING:
                point.updatePointValue(new PointValueTime(propertyProvider.getString(prop), time));
                break;
            case TIME:
                point.updatePointValue(new PointValueTime(propertyProvider.getTime(prop).toString(), time));
                break;
            case TIME_STAMP:
                point.updatePointValue(new PointValueTime(propertyProvider.getTimestamp(prop).toString(), time));
                break;
            default:
                throw new RuntimeException();

        }
    }

    private void addFoundHmsDataPoint(HmsMessage hmsMessage, HmsProperty prop) {

        // this value was not found, so create one
        Fhz4JPointLocatorVO<HmsProperty> fhzLocator = (Fhz4JPointLocatorVO<HmsProperty>) vo.createPointLocator(FhzProtocol.HMS);

        HmsPointLocator hmsLocator = (HmsPointLocator) fhzLocator.getProtocolLocator();
        hmsLocator.setHousecode(hmsMessage.getHousecode());
        hmsLocator.setHmsDeviceType(hmsMessage.getDeviceType());
        hmsLocator.setProperty(prop);
        hmsLocator.setSettable(false);

        DataPointVO dp = new DataPointVO();
        dp.setName(hmsLocator.defaultName());
        dp.setXid(String.format("%04x-%s-%s", hmsMessage.getHousecode(), hmsMessage.getDeviceType().getName(), prop.getName()));
        dp.setDataSourceId(vo.getId());
        dp.setEnabled(true);
        dp.setLoggingType(LoggingTypes.ALL);
        dp.setEventDetectors(new ArrayList<PointEventDetectorVO>());

        dp.setPointLocator(fhzLocator);
        switch (dp.getDataType()) {
            case NUMERIC:
                dp.setTextRenderer(new AnalogRenderer("#,##0.0", prop.getUnitOfMeasurement()));
                dp.setChartRenderer(new ImageChartRenderer(TimePeriods.DAYS, 1));
                break;
            case BINARY:
                dp.setTextRenderer(new BinaryTextRenderer("0", null, "1", null));
            default:;
        }

        DataPointRT dataPointRT = Common.ctx.getRuntimeManager().saveDataPoint(dp);
        Common.ctx.getRuntimeManager().addPointToHierarchy(dp, vo.getName(), "HMS", String.format("%s %s", hmsLocator.getHousecodeStr(), hmsMessage.getDeviceType().getLabel()));

        if (dataPointRT != null) {
            updateValue(dataPointRT, hmsMessage, prop);
        }
        LOG.log(Level.SEVERE, "HMS point added: {0}", dp.getXid());
    }

    private Map<EmProperty, DataPointRT> getEmPoints(EmMessage emMessage) {
        return emPoints.get(emMessage.getAddress());
    }

    private DataPointVO getEmDisabledPoint(EmMessage emMessage, EmProperty emProperty) {
        Map<EmProperty, DataPointVO> emPropertyMap = emDisabledPoints.get(emMessage.getAddress());
        if (emPropertyMap == null) {
            return null;
        }
        final DataPointVO dp = emPropertyMap.get(emProperty);
        return dp;
    }

    private DataPointRT getFs20Point(FS20Message fs20Message) {
        Map<Byte, DataPointRT> fs20PropertyMap = fs20Points.get(fs20Message.getHousecode());
        if (fs20PropertyMap == null) {
            return null;
        }
        final DataPointRT dp = fs20PropertyMap.get(fs20Message.getOffset());
        return dp;
    }

    private DataPointVO getFs20DisabledPoint(FS20Message fs20Message) {
        Map<Byte, DataPointVO> fs20PropertyMap = fs20DisabledPoints.get(fs20Message.getHousecode());
        if (fs20PropertyMap == null) {
            return null;
        }
        final DataPointVO dp = fs20PropertyMap.get(fs20Message.getOffset());
        return dp;
    }

    private DataPointRT getFhtPoint(FhtMessage fhtMessage) {
        Map<FhtProperty, DataPointRT> fhtPropertyMap = fhtPoints.get(fhtMessage.getHousecode());
        if (fhtPropertyMap == null) {
            return null;
        }
        final DataPointRT dp = fhtPropertyMap.get(fhtMessage.getCommand());
        return dp;
    }

    private DataPointVO getFhtDisabledPoint(FhtMessage fhtMessage) {
        Map<FhtProperty, DataPointVO> fhtPropertyMap = fhtDisabledPoints.get(fhtMessage.getHousecode());
        if (fhtPropertyMap == null) {
            return null;
        }
        final DataPointVO dp = fhtPropertyMap.get(fhtMessage.getCommand());
        return dp;
    }

    private Map<HmsProperty, DataPointRT> getHmsPoints(HmsMessage hmsMessage) {
        return hmsPoints.get(hmsMessage.getHousecode());
    }

    private DataPointVO getHmsDisabledPoint(HmsMessage hmsMessage, HmsProperty hmsProperty) {
        Map<HmsProperty, DataPointVO> hmsPropertyMap = hmsDisabledPoints.get(hmsMessage.getHousecode());
        if (hmsPropertyMap == null) {
            return null;
        }
        final DataPointVO dp = hmsPropertyMap.get(hmsProperty);
        return dp;
    }

    private DataPointRT getFhtMultiMsgPoint(FhtMultiMsgMessage fhtMultiMsgMessage) {
        Map<FhtMultiMsgProperty, DataPointRT> fhtMultiMsgPropertyMap = fhtMultiMsgPoints.get(fhtMultiMsgMessage.getHousecode());
        if (fhtMultiMsgPropertyMap == null) {
            return null;
        }
        final DataPointRT dp = fhtMultiMsgPropertyMap.get(fhtMultiMsgMessage.getProperty());
        return dp;
    }

    private DataPointVO getFhtMultiMsgDisabledPoint(FhtMultiMsgMessage fhtMultiMsgMessage) {
        Map<FhtMultiMsgProperty, DataPointVO> fhtMultiMsgPropertyMap = fhtMultiMsgDisabledPoints.get(fhtMultiMsgMessage.getHousecode());
        if (fhtMultiMsgPropertyMap == null) {
            return null;
        }
        final DataPointVO dp = fhtMultiMsgPropertyMap.get(fhtMultiMsgMessage.getProperty());
        return dp;
    }

    private void addFoundFhtMultiMsgMessage(FhtMultiMsgMessage fhtMultiMsgMessage) {

        // this value was not found, so create one
        final Fhz4JPointLocatorVO<FhtMultiMsgProperty> fhzLocator = (Fhz4JPointLocatorVO<FhtMultiMsgProperty>) vo.createPointLocator(FhzProtocol.FHT_MULTI_MSG);
        final FhtMultiMsgPointLocator fhtLocator = (FhtMultiMsgPointLocator) fhzLocator.getProtocolLocator();
        fhtLocator.setHousecode(fhtMultiMsgMessage.getHousecode());
        fhtLocator.setProperty(fhtMultiMsgMessage.getProperty());
        fhtLocator.setSettable(false);

        DataPointVO dp = new DataPointVO();
        dp.setName(fhtLocator.defaultName());
        dp.setEnabled(true);
        dp.setXid(String.format("%04x-%s", fhtMultiMsgMessage.getHousecode(), fhtMultiMsgMessage.getProperty()));
        dp.setDataSourceId(vo.getId());
        dp.setLoggingType(LoggingTypes.ALL);
        dp.setEventDetectors(new ArrayList<PointEventDetectorVO>());

        dp.setPointLocator(fhzLocator);
        if (dp.getDataType() == DataType.NUMERIC) {
            dp.setTextRenderer(new AnalogRenderer("#,##0.0", fhtMultiMsgMessage.getProperty().getUnitOfMeasurement()));
            dp.setChartRenderer(new ImageChartRenderer(TimePeriods.DAYS, 1));
        }

        DataPointRT dataPointRT = Common.ctx.getRuntimeManager().saveDataPoint(dp);
        Common.ctx.getRuntimeManager().addPointToHierarchy(dp, vo.getName(), "FHT", fhtMultiMsgMessage.getHousecodeStr());

        if (dataPointRT != null) {
            updateValue(dataPointRT, fhtMultiMsgMessage, fhtMultiMsgMessage.getProperty());
        }
        LOG.log(Level.INFO, "FHT point added: {0}", dp.getName());
    }

    private void setFhtValue(PointValueTime valueTime, FhtPointLocator fhtPointLocator) throws IOException {
        FhtMessage fhtMessage = new FhtMessage();
        switch (fhtPointLocator.getProperty().getDataType()) {
            case FLOAT:
                fhtMessage.setHousecode(fhtPointLocator.getHousecode());
                fhtMessage.setFloat(fhtPointLocator.getProperty(), valueTime.getFloatValue());
                writer.writeFhtMsg(fhtMessage);
                break;
            default:
                throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.

        }

    }

}
