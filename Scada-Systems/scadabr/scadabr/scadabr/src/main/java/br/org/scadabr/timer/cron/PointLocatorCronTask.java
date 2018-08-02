/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.timer.cron;

import br.org.scadabr.rt.datasource.PollingPointLocatorRT;
import com.serotonin.mango.rt.dataSource.DataSourceRT;
import java.text.ParseException;
import java.util.TimeZone;

/**
 *
 * @author aploese
 */
public class PointLocatorCronTask extends DataSourceCronTask<DataSourceRT> {

    private PollingPointLocatorRT pollingPointLocator;

    public PointLocatorCronTask(DataSourceRT dataSource, PollingPointLocatorRT pollingPointLocatorRT, String cronPattern, TimeZone tz) throws ParseException {
        super(dataSource, cronPattern, tz);
        this.pollingPointLocator = pollingPointLocatorRT;
    }

    public PointLocatorCronTask(DataSourceRT dataSource, PollingPointLocatorRT pollingPointLocatorRT, CronExpression cronExpression) {
        super(dataSource, cronExpression);
        this.pollingPointLocator = pollingPointLocatorRT;
    }

    @Override
    protected void run(long scheduledExecutionTime) {
        pollingPointLocator.doPoll(scheduledExecutionTime);
    }

    @Override
    protected boolean overrunDetected(long lastExecutionTime, long thisExecutionTime) {
        return pollingPointLocator.overrunDetected(lastExecutionTime, thisExecutionTime);
    }

}
