/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.rt;

import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.timer.CronTimerPool;
import br.org.scadabr.timer.cron.DataSourceCallable;
import br.org.scadabr.timer.cron.DataSourceCronTask;
import br.org.scadabr.timer.cron.DataSourceRunnable;
import br.org.scadabr.timer.cron.EventCallable;
import br.org.scadabr.timer.cron.EventCronTask;
import br.org.scadabr.timer.cron.EventRunnable;
import br.org.scadabr.timer.cron.SystemCallable;
import br.org.scadabr.timer.cron.SystemCronTask;
import br.org.scadabr.timer.cron.SystemRunnable;
import java.util.concurrent.Callable;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.annotation.PostConstruct;
import javax.annotation.PreDestroy;
import javax.inject.Named;

/**
 *
 * @author aploese
 */
@Named
public class SchedulerPool {
    
    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_CORE);
    
    private CronTimerPool<DataSourceCronTask, DataSourceRunnable> dataSourcePool;
    private CronTimerPool<SystemCronTask, SystemRunnable> systemPool;
    private CronTimerPool<EventCronTask, EventRunnable> eventPool;
    
    @PreDestroy
    public void shutdown() {
                //TODO move to Common
        LOG.log(Level.INFO, "Shutdown dataSourcePool active: {0} queue size: {1}", new Object[]{dataSourcePool.getActiveCount(), dataSourcePool.getQueueSize()});
        dataSourcePool.shutdown();
        LOG.log(Level.INFO, "Shutdown eventCronPool active: {0} queue size: {1}", new Object[]{eventPool.getActiveCount(), eventPool.getQueueSize()});
        eventPool.shutdown();
        LOG.log(Level.INFO, "Shutdown systemCronPool active: {0} queue size: {1}", new Object[]{systemPool.getActiveCount(), systemPool.getQueueSize()});
        systemPool.shutdown();

        try {
            if (dataSourcePool.awaitTermination(10, TimeUnit.SECONDS)) {
                LOG.info("dataSourcePool terminated");
            } else {
                LOG.log(Level.SEVERE, "dataSourcePool termination timeout active: {0} queue size: {1}", new Object[]{dataSourcePool.getActiveCount(), dataSourcePool.getQueueSize()});
            }
        } catch (InterruptedException e) {
            LOG.log(Level.SEVERE, "dataSourcePool termination interrupted exception active: {0} queue size: {1}", new Object[]{dataSourcePool.getActiveCount(), dataSourcePool.getQueueSize()});
        }

        try {
            if (eventPool.awaitTermination(10, TimeUnit.SECONDS)) {
                LOG.info("eventCronPool terminated");
            } else {
                LOG.log(Level.SEVERE, "eventCronPool termination timeout active: {0} queue size: {1}", new Object[]{eventPool.getActiveCount(), eventPool.getQueueSize()});
            }
        } catch (InterruptedException e) {
            LOG.log(Level.SEVERE, "eventCronPool termination interrupted exception active: {0} queue size: {1}", new Object[]{eventPool.getActiveCount(), eventPool.getQueueSize()});
        }

        try {
            if (systemPool.awaitTermination(10, TimeUnit.SECONDS)) {
                LOG.info("systemCronPool terminated");
            } else {
                LOG.log(Level.SEVERE, "systemCronPool termination timeout active: {0} queue size: {1}", new Object[]{systemPool.getActiveCount(), systemPool.getQueueSize()});
            }
        } catch (InterruptedException e) {
            LOG.log(Level.SEVERE, "systemCronPool termination interrupted exception active: {0} queue size: {1}", new Object[]{systemPool.getActiveCount(), systemPool.getQueueSize()});
        }

        dataSourcePool = null;
        eventPool = null;
        systemPool = null;

    }
    
    @PostConstruct
    public void startup() {
        dataSourcePool = new CronTimerPool(2, 5, 30, TimeUnit.SECONDS);
        systemPool = new CronTimerPool(2, 5, 30, TimeUnit.SECONDS);
        eventPool = new CronTimerPool(2, 5, 30, TimeUnit.SECONDS);
    }
    
    public SchedulerPool() {
    }

    public <T> Future<T> submit(DataSourceCallable<T> t) throws InterruptedException {
        return dataSourcePool.submit(t);
    }
    
    public <T> Future<T> submit(SystemCallable<T> t) throws InterruptedException {
        return systemPool.submit(t);
    }
    
    public <T> Future<T> submit(EventCallable<T> t) throws InterruptedException {
        return eventPool.submit(t);
    }
    
    public Future<?> submit(DataSourceRunnable runnable) {
        return dataSourcePool.submit(runnable);
    }

    public Future<?> submit(SystemRunnable runnable) {
        return systemPool.submit(runnable);
    }

    public Future<?> submit(EventRunnable runnable) {
        return eventPool.submit(runnable);
    }

    public void execute(DataSourceRunnable runnable) {
        dataSourcePool.execute(runnable);
    }

    public void execute(SystemRunnable runnable) {
        systemPool.execute(runnable);
    }

    public void execute(EventRunnable runnable) {
        eventPool.execute(runnable);
    }

    public void schedule(DataSourceCronTask task) {
        dataSourcePool.schedule(task);
    }

    public void schedule(SystemCronTask task) {
        systemPool.schedule(task);
    }

    public void schedule(EventCronTask task) {
        eventPool.schedule(task);
    }

    public int getDataSourcePoolSize() {
        return dataSourcePool.getPoolSize();
    }

    public int getDataSourceSystemActiveCount() {
        return dataSourcePool.getActiveCount();
    }

    public int getSystemPoolSize() {
        return systemPool.getPoolSize();
    }

    public int getSystemActiveCount() {
        return systemPool.getActiveCount();
    }

    public int getEventPoolSize() {
        return eventPool.getPoolSize();
    }

    public int getEventActiveCount() {
        return eventPool.getActiveCount();
    }

}
