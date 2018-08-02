package br.org.scadabr.rt.dataSource.asciiSerial;

import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.timer.cron.CronExpression;
import gnu.io.CommPortIdentifier;
import gnu.io.SerialPort;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Enumeration;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import br.org.scadabr.vo.dataSource.asciiSerial.ASCIISerialDataSourceVO;
import br.org.scadabr.vo.dataSource.asciiSerial.ASCIISerialPointLocatorVO;

import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.SetPointSource;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.rt.dataSource.PollingDataSource;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import java.text.ParseException;

public class ASCIISerialDataSource extends PollingDataSource<ASCIISerialDataSourceVO> {

    private final Log LOG = LogFactory.getLog(ASCIISerialDataSource.class);
    public static final int POINT_READ_EXCEPTION_EVENT = 1;
    public static final int DATA_SOURCE_EXCEPTION_EVENT = 2;
    private Enumeration portList;
    private InputStream inSerialStream;
    private OutputStream outSerialStream;
    private SerialPort sPort;

    public ASCIISerialDataSource(ASCIISerialDataSourceVO vo) {
        super(vo, true);
        setPollingPeriod(vo.getUpdatePeriodType(), vo.getUpdatePeriods(),
                vo.isQuantize());

        portList = CommPortIdentifier.getPortIdentifiers();
        getPort(vo.getCommPortId());
        configurePort(getsPort());

    }

    private boolean reconnect() {

        try {
            while (true) {
                Thread.sleep(5000);
                portList = CommPortIdentifier.getPortIdentifiers();
                SerialPort p = getPort(vo.getCommPortId());
                if (p != null) {
                    configurePort(getsPort());
                    beginPolling();
                    return true;
                }
            }
        } catch (Exception e) {
            return false;
        }

    }

    @Override
    public void doPoll(long time) {
        updateChangedPoints();
        try {

            // no data
            if (getInSerialStream().available() == 0) {

                for (DataPointRT dataPoint : enabledDataPoints.values()) {
                    ASCIISerialPointLocatorVO dataPointVO = dataPoint.getVo()
                            .getPointLocator();
                    if (dataPointVO.getCommand() != null) {
                        getOutSerialStream().write(
                                dataPointVO.getCommand().getBytes());
                    }

                }
                raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, time, true,
                        new LocalizableMessageImpl("event.exception2",
                                vo.getName(), "Sem dados disponíveis !"));
            } else if (getInSerialStream().available() > 0) {
                byte[] readBuffer = new byte[vo.getBufferSize()];
                try {

                    while (getInSerialStream().available() > 0) {
                        getInSerialStream().read(readBuffer);
                    }

                    String result = new String(readBuffer);
					// System.out.println("Result: " + result);

                    // Pos processing
                    String posResults[];

                    // Regex string
                    String posResultFinal = "";

                    // Debug posResults
                    posResults = result.split(vo.getCharX());

                    for (String posResult : posResults) {
                        // System.out.println("posResult: " + posResults[i]);
                    }

                    if (vo.getCharX() != null) {
                        // System.out.println("Caracterer :" + vo.getCharX());
                        posResults = result.split(vo.getCharX());

                        if (posResults.length != 0) {
                            posResultFinal = posResults[0];
                            for (String posResult : posResults) {
                                // System.out.println(posResults[i]);
                            }
                        }
                    }

                    for (DataPointRT dataPoint : enabledDataPoints.values()) {
                        try {
                            ASCIISerialPointLocatorVO dataPointVO = dataPoint
                                    .getVo().getPointLocator();
                            if (dataPointVO.getCommand() != null) {
                                getOutSerialStream().write(
                                        dataPointVO.getCommand().getBytes());
                            }

                            // troquei posResult por result
                            MangoValue value = getValue(dataPointVO, result);
                            long timestamp = time;
                            // write to port

                            if (dataPointVO.isCustomTimestamp()) {
                                try {
                                    timestamp = getTimestamp(dataPointVO,
                                            result);
                                } catch (Exception e) {
                                    raiseEvent(
                                            POINT_READ_EXCEPTION_EVENT,
                                            time,
                                            true,
                                            new LocalizableMessageImpl(
                                                    "event.exception2", vo
                                                    .getName(), e
                                                    .getMessage()));
                                    timestamp = time;
                                }

                            }

                            dataPoint.updatePointValue(new PointValueTime(
                                    value, timestamp));
                        } catch (Exception e) {
                            raiseEvent(POINT_READ_EXCEPTION_EVENT, time, true,
                                    new LocalizableMessageImpl("event.exception2",
                                            vo.getName(), e.getMessage()));
                            // e.printStackTrace();
                        }

                    }

                } catch (Exception e) {
                }
            }

        } catch (IOException io) {
            getsPort().close();
            reconnect();
        } catch (Exception e) {
        }
    }

    @Override
    public void initialize() {
        super.initialize();

    }

    private MangoValue getValue(ASCIISerialPointLocatorVO point, String arquivo)
            throws Exception {
        String valueRegex = point.getValueRegex();
        Pattern valuePattern = Pattern.compile(valueRegex);
        Matcher matcher = valuePattern.matcher(arquivo);
        // System.out.println("Utilizando string: " + arquivo
        // + "\n Utilizando regex: " + valueRegex);
        MangoValue value = null;
        String strValue = null;
        boolean found = false;
        if (matcher.find()) {
            found = true;
            strValue = matcher.group();
            value = MangoValue.stringToValue(strValue, point.getDataType());
        }
        if (!found) {
            throw new Exception("Value string not found (regex: " + valueRegex
                    + ")");
        }

        return value;
    }

    private long getTimestamp(ASCIISerialPointLocatorVO point, String arquivo)
            throws Exception {
        long timestamp = new Date().getTime();
        String dataFormat = point.getTimestampFormat();
        String tsRegex = point.getTimestampRegex();
        Pattern tsPattern = Pattern.compile(tsRegex);
        Matcher tsMatcher = tsPattern.matcher(arquivo);

        boolean found = false;
        while (tsMatcher.find()) {
            found = true;
            String tsValue = tsMatcher.group();
            timestamp = new SimpleDateFormat(dataFormat).parse(tsValue)
                    .getTime();
        }

        if (!found) {
            throw new Exception("Timestamp string not found (regex: " + tsRegex
                    + ")");
        }

        return timestamp;
    }

    @Override
    public void terminate() {
        super.terminate();
        getsPort().close();
    }

    @Override
    public void setPointValue(DataPointRT dataPoint, PointValueTime valueTime,
            SetPointSource source) {

    }

    public void configurePort(SerialPort port) {

        try {
            setInSerialStream(port.getInputStream());
            setOutSerialStream(port.getOutputStream());
        } catch (Exception e) {
        }

        port.notifyOnDataAvailable(true);

        try {
            port.setSerialPortParams(vo.getBaudRate(), vo.getDataBits(),
                    vo.getStopBits(), vo.getParity());
        } catch (Exception e) {
        }

    }

    public SerialPort getPort(String port) {
        SerialPort serialPort = null;
        while (portList.hasMoreElements()) {
            CommPortIdentifier portId = (CommPortIdentifier) portList
                    .nextElement();
            if (portId.getPortType() == CommPortIdentifier.PORT_SERIAL) {
                if (portId.getName().equals(port)) {
                    try {
                        serialPort = (SerialPort) portId.open(this.getName(),
                                10000);
                        setsPort(serialPort);
                    } catch (Exception e) {
                        System.out.println("Erro ao abrir a porta !");
                    }
                }
            }
        }

        return serialPort;
    }

    public OutputStream getOutSerialStream() {
        return outSerialStream;
    }

    public void setOutSerialStream(OutputStream outSerialStream) {
        this.outSerialStream = outSerialStream;
    }

    public InputStream getInSerialStream() {
        return inSerialStream;
    }

    public void setInSerialStream(InputStream inSerialStream) {
        this.inSerialStream = inSerialStream;
    }

    public SerialPort getsPort() {
        return sPort;
    }

    public void setsPort(SerialPort sPort) {
        this.sPort = sPort;
    }

    @Override
    protected CronExpression getCronExpression() throws ParseException {
        throw new ImplementMeException();
    }
}
