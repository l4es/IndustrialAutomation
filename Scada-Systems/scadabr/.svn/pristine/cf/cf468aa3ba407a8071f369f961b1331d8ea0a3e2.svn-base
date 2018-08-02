package com.serotonin.mango.rt.dataSource.meta;

import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.vo.dataSource.meta.MetaPointLocatorVO;
import br.org.scadabr.timer.SimulationTimer;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.utils.i18n.LocalizableMessage;

public class HistoricalMetaPointLocatorRT extends MetaPointLocatorRT {

    private long updates;

    public HistoricalMetaPointLocatorRT(MetaPointLocatorVO vo) {
        super(vo);
    }

    public void initialize(SimulationTimer timer, DataPointRT dataPoint) {
        throw new ImplementMeException();
        /*
        this.timer = timer;
        this.dataPoint = dataPoint;
        initialized = true;
        initializeTimerTask();

        context = new HashMap<>();
        for (IntValuePair contextEntry : vo.getContext()) {
            DataPointVO cvo = dataPointDao.getDataPoint(contextEntry.getKey());
            HistoricalDataPoint point = new HistoricalDataPoint(cvo.getId(), cvo.getPointLocator().getDataType(),
                    timer, pointValueDao);
            context.put(contextEntry.getValue(), point);
        }
                */
    }

    @Override
    public void terminate() {
        throw new ImplementMeException();
/*
        synchronized (LOCK) {
            // Cancel scheduled job
            if (timerTask != null) {
                timerTask.cancel();
            }
        }
*/
    }

    public long getUpdates() {
        return updates;
    }

    @Override
    protected void updatePoint(PointValueTime pvt) {
        super.updatePoint(pvt);
        updates++;
    }

    @Override
    protected void fireScriptErrorEvent(long runtime, LocalizableMessage message) {
        throw new MetaPointExecutionException(message);
    }
}
