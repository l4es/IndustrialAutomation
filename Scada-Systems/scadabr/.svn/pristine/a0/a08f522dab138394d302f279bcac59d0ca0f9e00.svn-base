package br.org.scadabr.rt.dataSource.asciiFile;

import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.timer.cron.CronExpression;
import java.io.File;
import java.io.FileReader;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import br.org.scadabr.vo.dataSource.asciiFile.ASCIIFileDataSourceVO;
import br.org.scadabr.vo.dataSource.asciiFile.ASCIIFilePointLocatorVO;

import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.SetPointSource;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.rt.dataSource.PollingDataSource;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import java.text.ParseException;

public class ASCIIFileDataSource extends PollingDataSource<ASCIIFileDataSourceVO> {

    private final Log LOG = LogFactory.getLog(ASCIIFileDataSource.class);
    public static final int POINT_READ_EXCEPTION_EVENT = 1;
    public static final int DATA_SOURCE_EXCEPTION_EVENT = 2;

    public ASCIIFileDataSource(ASCIIFileDataSourceVO vo) {
        super(vo, true);
        setPollingPeriod(vo.getUpdatePeriodType(), vo.getUpdatePeriods(), vo
                .isQuantize());
    }

    @Override
    public void doPoll(long time) {
        updateChangedPoints();

        File file = new File(vo.getFilePath());

        if (!file.exists()) {
            raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, time, true,
                    new LocalizableMessageImpl("event.exception2", vo.getName(),
                            "Arquivo não encontrado!"));
        } else {
            String arquivo = readFile(file);

            for (DataPointRT dataPoint : enabledDataPoints.values()) {
                try {
                    ASCIIFilePointLocatorVO dataPointVO = dataPoint.getVo()
                            .getPointLocator();
                    MangoValue value;
                    value = getValue(dataPointVO, arquivo);
                    long timestamp = time;
                    if (dataPointVO.isCustomTimestamp()) {
                        try {
                            timestamp = getTimestamp(dataPointVO, arquivo);
                        } catch (Exception e) {
                            raiseEvent(POINT_READ_EXCEPTION_EVENT, time, true,
                                    new LocalizableMessageImpl("event.exception2",
                                            vo.getName(), e.getMessage()));
                            timestamp = time;
                        }

                    }

                    dataPoint.updatePointValue(new PointValueTime(value,
                            timestamp));
                } catch (Exception e) {
                    raiseEvent(POINT_READ_EXCEPTION_EVENT, time, true,
                            new LocalizableMessageImpl("event.exception2", vo
                                    .getName(), e.getMessage()));
                    e.printStackTrace();
                }

            }
        }
    }

    private MangoValue getValue(ASCIIFilePointLocatorVO point, String arquivo)
            throws Exception {
        String valueRegex = point.getValueRegex();
        Pattern valuePattern = Pattern.compile(valueRegex);
        Matcher matcher = valuePattern.matcher(arquivo);
        MangoValue value = null;
        String strValue = null;
        boolean found = false;
        while (matcher.find()) {
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

    private long getTimestamp(ASCIIFilePointLocatorVO point, String arquivo)
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
    public void setPointValue(DataPointRT dataPoint, PointValueTime valueTime,
            SetPointSource source) {

    }

    private String readFile(File file) {
        StringBuilder sb = new StringBuilder();
        FileReader reader;
        try {
            reader = new FileReader(file);
            int c;
            do {
                c = reader.read();
                if (c != -1) {
                    sb.append((char) c);
                }
            } while (c != -1);
            reader.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return sb.toString();
    }

    @Override
    protected CronExpression getCronExpression() throws ParseException {
        throw new ImplementMeException();
    }
}
