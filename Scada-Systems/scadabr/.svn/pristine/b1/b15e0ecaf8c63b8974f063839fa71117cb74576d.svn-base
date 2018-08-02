/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.timer;

import br.org.scadabr.timer.cron.CronExpression;
import br.org.scadabr.timer.cron.CronParser;
import java.text.ParseException;
import java.util.TimeZone;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author aploese
 */
public abstract class CronTask {
    
    private final static Logger LOG = Logger.getLogger(CronTask.class.getName());

    ThreadPoolExecutor tpe;

    protected abstract void run(long executionTime);
    
    protected void handleException(Throwable t) {
       LOG.log(Level.SEVERE, "Ex: ", t);
    }

    /**
     * An Overrrun has occured, should this task run again???
     *
     * @param lastExecutionTime
     * @param thisExecutionTime
     * @return true, if Task should scheduled, false if it should not run.
     */
    protected abstract boolean overrunDetected(long lastExecutionTime, long thisExecutionTime);

    public boolean isExecuting() {
        synchronized (lock) {
            return currentExecutedTaskRunner != null;
        }

    }

    TaskRunner currentExecutedTaskRunner;

    TaskRunner createRunner(long scheduledExecutionTime) throws OverrideDieException {
        synchronized (lock) {
            if (currentExecutedTaskRunner != null) {
                if (!overrunDetected(currentExecutedTaskRunner.executionTime, scheduledExecutionTime)) {
                    currentExecutedTaskRunner = null;
                    throw new OverrideDieException();
                }
            }
            currentExecutedTaskRunner = new TaskRunner(scheduledExecutionTime);
        }
        return currentExecutedTaskRunner;
    }

    class OverrideDieException extends Exception {

        public OverrideDieException() {
        }
    }

    class TaskRunner implements Runnable {

        final long executionTime;

        private TaskRunner(final long executionTime) {
            this.executionTime = executionTime;
        }

        @Override
        public void run() {
            try {
                LOG.log(Level.FINEST, "Start Task {0}", CronTask.this);
                CronTask.this.run(executionTime);
                LOG.log(Level.FINEST, "Task finished {0}", CronTask.this);
            } catch (Throwable t) {
                CronTask.this.handleException(t);
            } finally {
                synchronized (lock) {
                    if (currentExecutedTaskRunner == this) {
                        currentExecutedTaskRunner = null;
                    }
                }
            }
        }
    }

    final Object lock = new Object();

    int state = VIRGIN;

    static final int VIRGIN = 0;

    static final int SCHEDULED = 1;

    static final int EXECUTED = 2;

    static final int CANCELLED = 3;

    CronExpression cronExpression;

    long nextExecutionTime;

    protected CronTask(CronExpression ce) {
        cronExpression = ce;
    }

    protected CronTask(String pattern, TimeZone tz) throws ParseException {
        CronParser cp = new CronParser();
        cronExpression = cp.parse(pattern, tz);
    }

    public boolean cancel() {
        synchronized (lock) {
            if (tpe != null && currentExecutedTaskRunner != null) {
                tpe.remove(currentExecutedTaskRunner);
            }
            currentExecutedTaskRunner = null;
            boolean result = (state == SCHEDULED);
            state = CANCELLED;
            return result;
        }
    }

    public long calcNextExecutionTimeIncludingNow(long timeInMillis) {
        synchronized (lock) {
            nextExecutionTime = cronExpression.calcNextValidTimeIncludingThis(timeInMillis);
            return nextExecutionTime;
        }
    }

    public long calcNextExecutionTimeAfter() {
        synchronized (lock) {
            nextExecutionTime = cronExpression.calcNextValidTimeAfter();
            return nextExecutionTime;
        }
    }

    public long getNextScheduledExecutionTime() {
        synchronized (lock) {
            return nextExecutionTime;
        }
    }

}
