/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.serotonin.mango.view.chart;

import br.org.scadabr.DataType;
import java.util.Arrays;
import java.util.EnumSet;
import java.util.Set;

/**
 *
 * @author aploese
 */
public enum ChartType {
    NONE("chartRenderer.none", DataType.ALPHANUMERIC, DataType.BOOLEAN, DataType.MULTISTATE, DataType.DOUBLE, DataType.IMAGE), 
    TABLE("chartRenderer.table", DataType.ALPHANUMERIC, DataType.BOOLEAN, DataType.MULTISTATE, DataType.DOUBLE), 
    IMAGE("chartRenderer.image", DataType.BOOLEAN, DataType.MULTISTATE, DataType.DOUBLE), 
    STATS("chartRenderer.statistics", DataType.ALPHANUMERIC, DataType.BOOLEAN, DataType.MULTISTATE, DataType.DOUBLE),
    IMAGE_FLIPBOOK("chartRenderer.flipbook", DataType.IMAGE);
    
    final String i18nKey;
    final Set<DataType> dataTypes;
    
    private ChartType (String i18nKey, DataType ... dataTypes) {
        this.i18nKey = i18nKey;
        this.dataTypes = EnumSet.copyOf(Arrays.asList(dataTypes));
    }
    
    public boolean supports(DataType dataType) {
        return dataTypes.contains(dataType);
    }
    
}
