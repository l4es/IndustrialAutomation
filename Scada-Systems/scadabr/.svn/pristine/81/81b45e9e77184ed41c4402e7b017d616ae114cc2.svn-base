package com.serotonin.mango.util.timeout;

import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.timer.cron.CronExpression;
import br.org.scadabr.timer.cron.EventCronTask;
import java.text.ParseException;
import java.util.TimeZone;

public class EventRunTask extends EventCronTask {

    private final RunClient client;

    public EventRunTask(CronExpression cronExpression, RunClient client) {
        super(cronExpression);
        this.client = client;
    }

    public EventRunTask(String cronPattern, TimeZone tz, RunClient client) throws ParseException {
        super(cronPattern, tz);
        this.client = client;
    }

    @Override
    protected void run(long scheduledExecutionTime) {
        client.run(scheduledExecutionTime);
    }

    @Override
    protected boolean overrunDetected(long lastExecutionTime, long thisExecutionTime) {
        throw new ImplementMeException();
    }
}
