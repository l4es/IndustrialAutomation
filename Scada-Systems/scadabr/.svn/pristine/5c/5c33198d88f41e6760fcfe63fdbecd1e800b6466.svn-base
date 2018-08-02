package br.org.scadabr.rt;

import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.vo.DataPointVO;

public interface IDataPointLiveCycleListener {

    void dataPointEnabled(DataPointRT dataPoint);

    void dataPointDisabled(DataPointVO dataPoint);

    /**
     * the dataPoint is already disabled...
     * 
     * @param dataPoint 
     */
    void dataPointDeleted(DataPointVO dataPoint);
}
