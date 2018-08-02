/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.timer.cron;

import br.org.scadabr.timer.CronTask;
import com.serotonin.mango.rt.dataSource.DataSourceRT;
import java.text.ParseException;
import java.util.TimeZone;

/**
 *
 * @author aploese
 */
public abstract class DataSourceCronTask<T extends DataSourceRT> extends CronTask {

    protected final T dataSource;
    
    public DataSourceCronTask(T dataSource, String cronPattern, TimeZone tz) throws ParseException {
        super(cronPattern, tz);
        this.dataSource = dataSource;
    }

    public DataSourceCronTask(T dataSource, CronExpression cronExpression) {
        super(cronExpression);
        this.dataSource = dataSource;
    }

}
