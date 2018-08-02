package com.serotonin.mango.rt.dataImage;

import br.org.scadabr.DataType;
import br.org.scadabr.dao.PointValueDao;

import br.org.scadabr.utils.NotImplementedException;
import br.org.scadabr.timer.SimulationTimer;

public class HistoricalDataPoint implements IDataPoint {

    private final int id;
    private final DataType dataType;
    private final PointValueDao pointValueDao;
    private final SimulationTimer timer;

    public HistoricalDataPoint(int id, DataType dataType, SimulationTimer timer, PointValueDao pointValueDao) {
        this.id = id;
        this.dataType = dataType;
        this.pointValueDao = pointValueDao;
        this.timer = timer;
    }

    public int getId() {
        return id;
    }

    @Override
    public void updatePointValue(PointValueTime newValue) {
        throw new NotImplementedException();
    }

    @Override
    public void updatePointValue(PointValueTime newValue, boolean async) {
        throw new NotImplementedException();
    }

    @Override
    public void setPointValue(PointValueTime newValue, SetPointSource source) {
        throw new NotImplementedException();
    }

    @Override
    public PointValueTime getPointValue() {
        return pointValueDao.getPointValueBefore(id, dataType, timer.currentTimeMillis() + 1);
    }

    @Override
    public DataType getDataType() {
        return dataType;
    }
}
