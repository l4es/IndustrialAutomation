/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.timer.cron;

import com.serotonin.mango.rt.dataSource.PollingDataSource;
import java.text.ParseException;
import java.util.TimeZone;

/**
 *
 * @author aploese
 */
public class PollingDataSourceCronTask extends DataSourceCronTask<PollingDataSource> {

    public PollingDataSourceCronTask(PollingDataSource dataSource, String cronPattern, TimeZone tz) throws ParseException {
        super(dataSource, cronPattern, tz);
    }

    public PollingDataSourceCronTask(PollingDataSource dataSource, CronExpression cronExpression) {
        super(dataSource, cronExpression);
    }
    
    @Override
    protected void run(long scheduledExecutionTime) {
        dataSource.doPoll(scheduledExecutionTime);
    }

    @Override
    protected boolean overrunDetected(long lastExecutionTime, long thisExecutionTime) {
        return dataSource.overrunDetected(lastExecutionTime, thisExecutionTime);
    }

}
