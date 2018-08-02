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

import br.org.scadabr.rt.scripting.DataPointStateException;
import br.org.scadabr.rt.scripting.ResultTypeException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import javax.script.ScriptException;

import br.org.scadabr.db.IntValuePair;
import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.rt.SchedulerPool;
import br.org.scadabr.rt.datasource.PollingPointLocatorRT;
import br.org.scadabr.rt.scripting.ScriptExecutor;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.rt.dataImage.DataPointListener;
import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.IDataPoint;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.vo.dataSource.meta.MetaPointLocatorVO;
import br.org.scadabr.timer.cron.PointLocatorCronTask;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.vo.datasource.meta.UpdateEvent;
import java.util.Date;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

/**
 * @author Matthew Lohbihler
 */
@Configurable
public class MetaPointLocatorRT<T extends PointValueTime> extends PollingPointLocatorRT<T, MetaPointLocatorVO<T>, MetaDataSourceRT> implements DataPointListener {

    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCARABR_DS_META);

    private static final ThreadLocal<List<Integer>> threadLocal = new ThreadLocal<>();
    private static final int MAX_RECURSION = 10;
    @Autowired
    private RuntimeManager runtimeManager;
    @Autowired
    private SchedulerPool schedulerPool;
    
    final Object LOCK = new Object();

    protected Map<String, IDataPoint> context;
    boolean initialized;
    private PointLocatorCronTask pollingCronTask;

    public MetaPointLocatorRT(MetaPointLocatorVO<T> vo) {
        super(vo);
    }

    boolean isContextCreated() {
        return context != null;
    }

    //
    // Lifecycle stuff. Note, this class does not implement the lifecycle interface because it is not controlled by
    // the RuntimeManager. The owning data source calls these methods then the point is added and removed from the
    // data source.
    //
    @Override
    public void start(MetaDataSourceRT dsRT, DataPointRT dataPoint) {
        super.start(dsRT, dataPoint);

        createContext();

        // Add listener registrations
        for (IntValuePair contextKey : vo.getContext()) {
            // Points shouldn't listen for their own updates.
            if (dataPoint.getId() != contextKey.getKey()) {
                runtimeManager.addDataPointListener(contextKey.getKey(), this);
            }
        }

        initialized = true;

        final UpdateEvent updateEvent = vo.getUpdateEvent();
        if (updateEvent == UpdateEvent.CONTEXT_UPDATE) {
        throw new ImplementMeException();
                } else {
            // Scheduled update. Create the timeout that will update this point.
            pollingCronTask = new PointLocatorCronTask(dsRT, this, vo.getCronExpression());
            schedulerPool.schedule(pollingCronTask);

        }
    }

    public void terminate() {
        synchronized (LOCK) {
            // Remove listener registrations
            RuntimeManager rm = runtimeManager;
            for (IntValuePair contextKey : vo.getContext()) {
                rm.removeDataPointListener(contextKey.getKey(), this);
            }

            // Cancel scheduled job
            if (pollingCronTask != null) {
                pollingCronTask.cancel();
            }

            initialized = false;
        }
    }

    //
    // / DataPointListener
    //
    @Override
    public void pointChanged(PointValueTime oldValue, PointValueTime newValue) {
        // No op. Events are covered in pointUpdated.
    }

    @Override
    public void pointSet(PointValueTime oldValue, PointValueTime newValue) {
        // No op. Events are covered in pointUpdated.
    }

    @Override
    public void pointUpdated(PointValueTime newValue) {
        // Ignore if this is not a context update.
        if (vo.getUpdateEvent() == UpdateEvent.CONTEXT_UPDATE) {
            // Check for infinite loops
            List<Integer> sourceIds;
            if (threadLocal.get() == null) {
                sourceIds = new ArrayList<>();
            } else {
                sourceIds = threadLocal.get();
            }

            long time = newValue.getTimestamp();
            if (vo.getExecutionDelaySeconds() == 0) {
                execute(time, sourceIds);
            } else {
                synchronized (LOCK) {
                    if (initialized) {
                        if (pollingCronTask != null) {
                            pollingCronTask.cancel();
                        }
                        throw new ImplementMeException();
                        // timerTask = new ExecutionDelayTimeout(time, sourceIds);
                    }
                }
            }
        }
    }

    @Override
    public void pointBackdated(PointValueTime value) {
        // No op.
    }

    @Override
    public void pointInitialized() {
        createContext();
        dsRT.checkForDisabledPoints();
    }

    @Override
    public void pointTerminated() {
        createContext();
        dsRT.checkForDisabledPoints();
    }

    void execute(long runtime, List<Integer> sourceIds) {
        if (context == null) {
            return;
        }

        // Check if we've reached the maximum number of recursions for this point
        int count = 0;
        for (Integer id : sourceIds) {
            if (id == dpRT.getId()) {
                count++;
            }
        }

        if (count > MAX_RECURSION) {
            fireScriptErrorEvent(runtime, "event.meta.recursionFailure");
            return;
        }

        sourceIds.add(dpRT.getId());
        threadLocal.set(sourceIds);
        try {
            ScriptExecutor executor = new ScriptExecutor();
            try {
                T pvt = (T)executor.execute(vo.getScript(), context, System.currentTimeMillis(), vo.getId(), vo.getDataType(), runtime);
                if (pvt == null || pvt.getValue() == null) {
                    fireScriptErrorEvent(runtime, "event.meta.nullResult");
                } else {
                    clearScriptErrorEvent(runtime);
                    updatePoint(pvt);
                }
            } catch (ScriptException e) {
                fireScriptErrorEvent(runtime, "common.default", e.getMessage());
            } catch (ResultTypeException e) {
                fireScriptErrorEvent(runtime, e);
            }
        } finally {
            threadLocal.remove();
        }
    }

    private void createContext() {
        context = null;
        try {
            ScriptExecutor scriptExecutor = new ScriptExecutor();
            context = scriptExecutor.convertContext(vo.getContext());
        } catch (DataPointStateException e) {
            // no op
        }
    }

    protected void updatePoint(T pvt) {
        dpRT.updatePointValueAsync(pvt);
    }

    protected void fireScriptErrorEvent(long runtime, LocalizableMessage msg) {
        ((MetaDataSourceRT)dsRT).fireScriptErrorEvent(runtime, dpRT, msg);
    }
   
    protected void fireScriptErrorEvent(long runtime, String i18nKey) {
        ((MetaDataSourceRT)dsRT).fireScriptErrorEvent(runtime, dpRT, i18nKey);
    }
   
    protected void fireScriptErrorEvent(long runtime, String i18nKey, Object... args) {
        ((MetaDataSourceRT)dsRT).fireScriptErrorEvent(runtime, dpRT, i18nKey, args);
    }
   
    protected void clearScriptErrorEvent(long runtime) {
        ((MetaDataSourceRT)dsRT).clearScriptErrorAlarm(runtime, dpRT);
    }
    
    protected List<Integer> getSourceIds() {
         final List<Integer> result = threadLocal.get();
         return result != null ? result : new ArrayList<Integer>();
    }
    
    @Override
    public void doPoll(long scheduledExecutionTime) {
        execute(scheduledExecutionTime, getSourceIds());
    }

    @Override
    public boolean overrunDetected(long lastExecutionTime, long thisExecutionTime) {
        //TODO dataSource.raiseOverrunError(runtime, dataPoint, message);
            LOG.log(Level.WARNING, "{0}: poll at {1} aborted because a previous poll started at {2} is still running", new Object[]{vo.getName(), new Date(thisExecutionTime), new Date(lastExecutionTime)});
            return false;
    }


}
