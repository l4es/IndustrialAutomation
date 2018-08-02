package br.org.scadabr.rt.dataSource.opc;

import br.org.scadabr.utils.ImplementMeException;
import java.util.ArrayList;
import java.util.logging.Level;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.jinterop.dcom.common.JISystem;

import br.org.scadabr.OPCMaster;
import br.org.scadabr.RealOPCMaster;
import br.org.scadabr.timer.cron.CronExpression;
import br.org.scadabr.vo.dataSource.opc.OPCDataSourceVO;
import br.org.scadabr.vo.dataSource.opc.OPCPointLocatorVO;

import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.SetPointSource;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.rt.dataSource.PollingDataSource;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import java.text.ParseException;

public class OPCDataSource extends PollingDataSource<OPCDataSourceVO> {

    private final Log LOG = LogFactory.getLog(OPCDataSource.class);
    public static final int POINT_READ_EXCEPTION_EVENT = 1;
    public static final int DATA_SOURCE_EXCEPTION_EVENT = 2;
    public static final int POINT_WRITE_EXCEPTION_EVENT = 3;
    private final OPCMaster opcMaster;
    private int timeoutCount = 0;
    private final int timeoutsToReconnect = 3;

    public OPCDataSource(OPCDataSourceVO vo) {
        super(vo, true);
        setPollingPeriod(vo.getUpdatePeriodType(), vo.getUpdatePeriods(),
                vo.isQuantize());

        this.opcMaster = new RealOPCMaster();
        JISystem.getLogger().setLevel(Level.OFF);
    }

    @Override
    public void doPoll(long time) {
        updateChangedPoints();
        ArrayList<String> enabledTags = new ArrayList<>();

        for (DataPointRT dataPoint : enabledDataPoints.values()) {
            OPCPointLocatorVO dataPointVO = dataPoint.getVo().getPointLocator();
            enabledTags.add(dataPointVO.getTag());
        }

        try {

            if (timeoutCount >= timeoutsToReconnect) {
                System.out.println("[OPC] Trying to reconnect !");
                timeoutCount = 0;
                initialize();
            } else {
                opcMaster.configureGroup(enabledTags);
                opcMaster.doPoll();
                returnToNormal(DATA_SOURCE_EXCEPTION_EVENT, time);
            }

        } catch (Exception e) {
            raiseEvent(
                    DATA_SOURCE_EXCEPTION_EVENT,
                    time,
                    true,
                    new LocalizableMessageImpl("event.exception2", vo.getName(), e
                            .getMessage()));
            timeoutCount++;
            System.out.println("[OPC] Poll Failed !");
        }

        for (DataPointRT dataPoint : enabledDataPoints.values()) {
            OPCPointLocatorVO dataPointVO = dataPoint.getVo().getPointLocator();
            MangoValue mangoValue = null;
            String value = "0";

            try {
                value = opcMaster.getValue(dataPointVO.getTag());

                mangoValue = MangoValue.stringToValue(value, dataPointVO.getDataType());
                dataPoint.updatePointValue(new PointValueTime(mangoValue, time));
            } catch (Exception e) {
                raiseEvent(POINT_READ_EXCEPTION_EVENT, time, true,
                        new LocalizableMessageImpl("event.exception2",
                                vo.getName(), e.getMessage()));
            }
        }
    }

    @Override
    public void setPointValue(DataPointRT dataPoint, PointValueTime valueTime,
            SetPointSource source) {
        String tag = ((OPCPointLocatorVO) dataPoint.getVo().getPointLocator())
                .getTag();
        Object value = null;
        switch (dataPoint.getDataType()) {
            case NUMERIC:
                value = valueTime.getDoubleValue();
                break;
            case BINARY:
                value = valueTime.getBooleanValue();
                break;
            case MULTISTATE:
                value = valueTime.getIntegerValue();
                break;
            default:
                value = valueTime.getStringValue();
        }

        try {
            opcMaster.write(tag, value);
        } catch (Exception e) {
            raiseEvent(
                    POINT_WRITE_EXCEPTION_EVENT,
                    System.currentTimeMillis(),
                    true,
                    new LocalizableMessageImpl("event.exception2", vo.getName(), e
                            .getMessage()));
            e.printStackTrace();
        }
    }

    @Override
    public void initialize() {

        opcMaster.setHost(vo.getHost());
        opcMaster.setDomain(vo.getDomain());
        opcMaster.setUser(vo.getUser());
        opcMaster.setPassword(vo.getPassword());
        opcMaster.setServer(vo.getServer());
        opcMaster.setDataSourceXid(vo.getXid());

        try {
            opcMaster.init();
            returnToNormal(DATA_SOURCE_EXCEPTION_EVENT,
                    System.currentTimeMillis());
        } catch (Exception e) {
            e.printStackTrace();
            raiseEvent(
                    DATA_SOURCE_EXCEPTION_EVENT,
                    System.currentTimeMillis(),
                    true,
                    new LocalizableMessageImpl("event.exception2", vo.getName(), e
                            .getMessage()));
            LOG.debug("Error while initializing data source", e);
            return;
        }
        super.initialize();
    }

    @Override
    public void terminate() {
        super.terminate();
        try {
            opcMaster.terminate();
        } catch (Exception e) {
            raiseEvent(
                    DATA_SOURCE_EXCEPTION_EVENT,
                    System.currentTimeMillis(),
                    true,
                    new LocalizableMessageImpl("event.exception2", vo.getName(), e
                            .getMessage()));
        }
    }

    @Override
    protected CronExpression getCronExpression() throws ParseException {
        throw new ImplementMeException();
    }
}
