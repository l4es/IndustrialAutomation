package com.serotonin.mango.util.timeout;

import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.timer.cron.CronExpression;
import br.org.scadabr.timer.cron.EventCronTask;
import java.text.ParseException;
import java.util.TimeZone;

public class EventRunWithArgTask<T> extends EventCronTask {

    private final RunWithArgClient<T> client;
    private final T model;

    public EventRunWithArgTask(CronExpression cronExpression, RunWithArgClient<T> client, T model) {
        super(cronExpression);
        this.client = client;
        this.model = model;
    }

    public EventRunWithArgTask(String cronPattern, TimeZone tz, RunWithArgClient<T> client, T model) throws ParseException {
        super(cronPattern, tz);
        this.client = client;
        this.model = model;
    }

    @Override
    protected void run(long scheduledExecutionTime) {
        client.run(model, scheduledExecutionTime);
    }

    @Override
    protected boolean overrunDetected(long lastExecutionTime, long thisExecutionTime) {
        throw new ImplementMeException();
    }
}
