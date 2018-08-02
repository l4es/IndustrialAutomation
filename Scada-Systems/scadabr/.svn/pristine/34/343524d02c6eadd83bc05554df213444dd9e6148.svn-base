/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.rt.datasource;

import br.org.scadabr.vo.datasource.PointLocatorVO;
import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataSource.DataSourceRT;
import com.serotonin.mango.rt.dataSource.PointLocatorRT;

/**
 *
 * @author aploese
 * @param <T>
 * @param <U>
 */
public abstract class PollingPointLocatorRT<T extends PointValueTime, VO extends PointLocatorVO<T>, U extends DataSourceRT> extends PointLocatorRT<T, VO> {

    protected DataPointRT<?, T> dpRT;
    protected U dsRT;
    
    public PollingPointLocatorRT(VO vo) {
        super(vo);
    }

    public final U getDsRT() {
        return dsRT;
    }

    public final DataPointRT<?, T> getDpRT() {
        return dpRT;
    }


    public abstract void doPoll(long scheduledExecutionTime);

    public abstract boolean overrunDetected(long lastExecutionTime, long thisExecutionTime);

    public void start(U dsRT, DataPointRT<?, T> dpRT) {
        this.dsRT = dsRT;
        this.dpRT = dpRT;
    }
    
    public void stop() {
        this.dsRT = null;
        this.dpRT = null;
    }
    

}
