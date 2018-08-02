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
package com.serotonin.mango.rt.dataSource;

import br.org.scadabr.utils.ImplementMeException;

import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.rt.SchedulerPool;
import br.org.scadabr.timer.cron.CronExpression;
import br.org.scadabr.timer.cron.DataSourceCronTask;
import br.org.scadabr.timer.cron.PollingDataSourceCronTask;
import br.org.scadabr.utils.TimePeriods;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import java.text.ParseException;
import java.util.Date;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

@Configurable
abstract public class PollingDataSource<T extends DataSourceVO<T>> extends DataSourceRT<T> {

    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCARABR_DS_RT);

    @Autowired
    private SchedulerPool schedulerPool;
    
    private DataSourceCronTask timerTask;

    public PollingDataSource(T vo, boolean doCache) {
        super(vo, doCache);
    }

    abstract public void doPoll(long time);

    //
    //
    // Data source interface
    //
    public void beginPolling() {
        try {
            timerTask = new PollingDataSourceCronTask(this, getCronExpression());
            schedulerPool.schedule(timerTask);
        } catch (ParseException e) {
            throw new RuntimeException(e);
        }
    }

    @Override
    public void terminate() {
        if (timerTask != null) {
            timerTask.cancel();
            if (!timerTask.isExecuting()) {
                timerTask = null;
            }
        }
        super.terminate();
    }

    @Override
    public void joinTermination() {
        super.joinTermination();

        final DataSourceCronTask local = timerTask;
        if (local != null) {
//TODO ???
            /*            try {
                local.join(30000); // 30 seconds
            } catch (InterruptedException e) { /* no op 

            }
  */
          if (timerTask != null) {
//                throw new ShouldNeverHappenException("Timeout waiting for data source to stop: id=" + getId() + ", type=" + getClass() + ", stackTrace=" + Arrays.toString(localThread.getStackTrace()));
            }
        }
    }
    
    protected abstract CronExpression getCronExpression() throws ParseException;
    
    @Deprecated
    protected void setPollingPeriod(TimePeriods updatePeriodType, int updatePeriods, boolean quantize) {
        // Set cronpattern from this
        throw new ImplementMeException();
    }

    public boolean overrunDetected(long lastExecutionTime, long thisExecutionTime) {
            LOG.log(Level.WARNING, "{0}: poll at {1} aborted because a previous poll started at {2} is still running", new Object[]{name, new Date(thisExecutionTime), new Date(lastExecutionTime)});
            return false;
    }
    
}
