package br.org.scadabr.rt.dataSource.dnp3;

import br.org.scadabr.utils.ImplementMeException;
import java.net.ConnectException;
import java.util.Calendar;
import java.util.Date;
import java.util.List;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import br.org.scadabr.dnp34j.master.session.database.DataElement;
import br.org.scadabr.timer.cron.CronExpression;
import br.org.scadabr.vo.dataSource.dnp3.Dnp3DataSourceVO;
import br.org.scadabr.vo.dataSource.dnp3.Dnp3PointLocatorVO;
import br.org.scadabr.utils.i18n.LocalizableException;

import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.SetPointSource;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.rt.dataSource.DataSourceRT;
import com.serotonin.mango.rt.dataSource.PollingDataSource;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import java.text.ParseException;

public class Dnp3DataSource<T extends Dnp3DataSourceVO<T>> extends PollingDataSource<T> {

    private final Log LOG = LogFactory.getLog(Dnp3DataSource.class);

    public static final int POINT_READ_EXCEPTION_EVENT = 1;
    public static final int DATA_SOURCE_EXCEPTION_EVENT = 2;

    private DNP3Master dnp3Master;

    public Dnp3DataSource(T vo, boolean doCache) {
        super(vo, doCache);
        setPollingPeriod(vo.getRbePeriodType(), vo.getRbePollPeriods(), vo
                .isQuantize());
    }

    @Override
    public void doPoll(long time) {
        updateChangedPoints();

        try {
            dnp3Master.doPoll();
            returnToNormal(DATA_SOURCE_EXCEPTION_EVENT, time);
        } catch (Exception e) {
            e.printStackTrace();
            raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, time, true,
                    new LocalizableMessageImpl("event.exception2", vo.getName(), e
                            .getMessage()));
        }

        for (DataPointRT dataPoint : enabledDataPoints.values()) {
            Dnp3PointLocatorVO pointLocator = ((Dnp3PointLocatorVO) dataPoint
                    .getVo().getPointLocator());
            List<DataElement> pointValues = dnp3Master.read(pointLocator
                    .getDnp3DataType(), pointLocator.getIndex());

            if (pointValues.size() > 0) {
                for (DataElement dataElement : pointValues) {
                    MangoValue value = MangoValue.stringToValue(dataElement
                            .getValue(), pointLocator.getDataType());
                    Calendar ts = Calendar.getInstance();
                    ts.setTimeInMillis(dataElement.getTimestamp());

                    dataPoint.updatePointValue(new PointValueTime(value, ts
                            .getTimeInMillis()));
                }
            }
        }
    }

    protected void initialize(DNP3Master dnp3Master) {
        this.dnp3Master = dnp3Master;
        super.initialize();
    }

    @Override
    public void terminate() {
        super.terminate();
        try {
            dnp3Master.terminate();
        } catch (Exception e) {
            raiseEvent(DATA_SOURCE_EXCEPTION_EVENT, new Date().getTime(), true,
                    new LocalizableMessageImpl("event.exception2", vo.getName(), e
                            .getMessage()));
            e.printStackTrace();
        }
    }

    @Override
    public void setPointValue(DataPointRT dataPoint, PointValueTime valueTime,
            SetPointSource source) {
        Dnp3PointLocatorVO pointLocator = (Dnp3PointLocatorVO) dataPoint
                .getVo().getPointLocator();

        int dataType = pointLocator.getDnp3DataType();
        int index = pointLocator.getIndex();

        try {
            if (dataType == 0x10) {
                dnp3Master.controlCommand(valueTime.getValue().toString(),
                        index, pointLocator.getControlCommand(), pointLocator
                        .getTimeOn(), pointLocator.getTimeOff());
            } else {
                dnp3Master
                        .sendAnalogCommand(index, valueTime.getIntegerValue());
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static LocalizableException wrapException(Exception e) {
        if (e instanceof Exception) {
            Throwable cause = e.getCause();
            if (cause instanceof ConnectException) {
                return new LocalizableException("common.default", e.getMessage());
            }
        }

        return DataSourceRT.wrapException(e);
    }

    @Override
    protected CronExpression getCronExpression() throws ParseException {
        throw new ImplementMeException();
    }
}
