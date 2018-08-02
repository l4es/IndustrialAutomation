/*
 Mango - Open Source M2M - http://mango.serotoninsoftware.com
 Copyright (C) 2006-2011 Serotonin Software Technologies Inc.
 @author Matthew Lohbihler
    
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
package com.serotonin.mango.rt.dataSource.meta;

import br.org.scadabr.rt.RT;
import br.org.scadabr.rt.WrongEdgeTypeException;
import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataSource.DataSourceRT;
import com.serotonin.mango.vo.dataSource.meta.MetaDataSourceVO;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.EdgeType;
import br.org.scadabr.vo.VO;
import br.org.scadabr.vo.datasource.meta.MetaDataSourceEventKey;
import com.serotonin.mango.vo.DataPointVO;
import java.util.HashSet;
import java.util.Set;
import br.org.scadabr.vo.EdgeConsumer;

/**
 * @author Matthew Lohbihler
 */
public class MetaDataSourceRT extends DataSourceRT<MetaDataSourceVO> {

    private boolean contextPointDisabledEventActive;

    public MetaDataSourceRT(MetaDataSourceVO vo) {
        super(vo, true);
    }

    @Override
    public void setPointValue(DataPointRT dataPoint, PointValueTime valueTime, VO<?> source) {
        dataPoint.setPointValueSync(valueTime, source);
    }

    @Override
    public void dataPointEnabled(DataPointRT dataPoint) {
        super.dataPointEnabled(dataPoint);
        MetaPointLocatorRT locator = (MetaPointLocatorRT)dataPoint.getPointLocator();
        locator.start(this, dataPoint);
        checkForDisabledPoints();
    }

    @Override
    public void dataPointDisabled(DataPointVO dataPoint) {
        DataPointRT dp = enabledDataPointsCache.get(dataPoint.getId());
        if (dp != null) {
            MetaPointLocatorRT locator = (MetaPointLocatorRT)dp.getPointLocator();
            locator.terminate();
        }
        super.dataPointDisabled(dataPoint);
    }

    synchronized void checkForDisabledPoints() {
        DataPointRT problemPoint = null;

        for (DataPointRT dp : enabledDataPoints.values()) {
            MetaPointLocatorRT locator = (MetaPointLocatorRT)dp.getPointLocator();
            if (!locator.isContextCreated()) {
                problemPoint = dp;
                break;
            }
        }

        if (contextPointDisabledEventActive != (problemPoint != null)) {
            if (contextPointDisabledEventActive) // A context point has been terminated, was never enabled, or not longer exists.
            {
                raiseAlarm(MetaDataSourceEventKey.CONTEXT_POINT_DISABLED, "event.meta.pointUnavailable", problemPoint.getName());
            } else // Everything is good
            {
                clearAlarm(MetaDataSourceEventKey.CONTEXT_POINT_DISABLED);
            }
        }
    }

    //TODO eventhandling should not be delegated to DataSource but rather handled here ... to filter out errors ...
    public void fireScriptErrorEvent(long runtime, DataPointRT dataPoint, LocalizableMessage msg) {
        Set<Integer> pointsWithErrors = (Set<Integer>) activeEvents.get(MetaDataSourceEventKey.SCRIPT_ERROR);
        if (pointsWithErrors == null) {
            pointsWithErrors = new HashSet<>();
            activeEvents.put(MetaDataSourceEventKey.SCRIPT_ERROR, pointsWithErrors);
        }
        if (!pointsWithErrors.contains(dataPoint.getId())) {
            pointsWithErrors.add(dataPoint.getId());
            fireEvent(MetaDataSourceEventKey.SCRIPT_ERROR, runtime, "event.meta.scriptError", dataPoint.getName(), msg);
        }
    }

    public void fireScriptErrorEvent(long runtime, DataPointRT dataPoint, String i18nKey) {
        fireScriptErrorEvent(runtime, dataPoint, new LocalizableMessageImpl(i18nKey));
    }

    public void fireScriptErrorEvent(long runtime, DataPointRT dataPoint, String i18nKey, Object... args) {
        fireScriptErrorEvent(runtime, dataPoint, new LocalizableMessageImpl(i18nKey, args));
    }

    public void clearScriptErrorAlarm(long runtime, DataPointRT dataPoint) {
        final Set<Integer> pointsWithErrors = (Set<Integer>) activeEvents.get(MetaDataSourceEventKey.SCRIPT_ERROR);
        if (pointsWithErrors == null) {
            // no key no error
            //TODO startup information is lost ... so we wont clear any errors before pot metaInfo in EventInstances Table instead of refId2 ???
            return;
        } else {
            if (pointsWithErrors.remove(dataPoint.getId())) {
                clearAlarm(MetaDataSourceEventKey.SCRIPT_ERROR, runtime);
            }
        }
    }

    public void fireResultTypeErrorEvent(long runtime, DataPointRT dataPoint, LocalizableMessage message) {
        Set<Integer> pointsWithErrors = (Set<Integer>) activeEvents.get(MetaDataSourceEventKey.RESULT_TYPE_ERROR);
        if (pointsWithErrors == null) {
            pointsWithErrors = new HashSet<>();
            activeEvents.put(MetaDataSourceEventKey.RESULT_TYPE_ERROR, pointsWithErrors);
        }
        if (!pointsWithErrors.contains(dataPoint.getId())) {
            pointsWithErrors.add(dataPoint.getId());
        fireEvent(MetaDataSourceEventKey.RESULT_TYPE_ERROR, runtime, "event.meta.typeError", dataPoint.getName(), message);
        }
    }

    @Override
    public void wireEdgeAsSrc(RT<?> dest, EdgeType edgeType) throws WrongEdgeTypeException {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void wireEdgeAsDest(RT<?> src, EdgeType edgeType) throws WrongEdgeTypeException {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void iterateEdgesAsSrc(EdgeConsumer edgeIterator, EdgeType... edgeTypes) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void iterateEdgesAsDest(EdgeConsumer edgeIterator, EdgeType... edgeTypes) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void patch(MetaDataSourceVO vo) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public MetaDataSourceVO getVO() {
        final MetaDataSourceVO result = new MetaDataSourceVO();
        fillVO(result);
        return result;
    }

}
