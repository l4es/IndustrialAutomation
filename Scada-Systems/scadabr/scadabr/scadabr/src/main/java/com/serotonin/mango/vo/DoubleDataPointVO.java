/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.serotonin.mango.vo;

import br.org.scadabr.DataType;
import br.org.scadabr.vo.datapoints.NumberDataPointVO;
import br.org.scadabr.vo.datasource.PointLocatorVO;
import com.fasterxml.jackson.annotation.JsonTypeName;
import com.serotonin.mango.rt.dataImage.DoubleDataPointRT;
import com.serotonin.mango.rt.dataImage.DoubleValueTime;

/**
 *
 * @author aploese
 */
@JsonTypeName("DATA_POINT.DOUBLE")
public class DoubleDataPointVO extends NumberDataPointVO<DoubleDataPointVO, DoubleValueTime>{

    
    public DoubleDataPointVO() {
        super("#,##0.00", "{0,number,#,##0.00} {1}");
    }
    
   @Override
    public DoubleDataPointRT createRT(PointLocatorVO<DoubleValueTime> pointLocatorVO) {
        return new DoubleDataPointRT(this, pointLocatorVO.createRuntime());
    }

    @Override
    public DataType getDataType() {
        return DataType.DOUBLE;
    }

    @Override
    public DoubleDataPointRT createRT() {
        //TODO do it right
        return new DoubleDataPointRT(this, null);
    }

}
