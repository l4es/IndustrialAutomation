package com.serotonin.mango.rt.maint;

import br.org.scadabr.utils.ImplementMeException;
import java.util.Collection;

import com.serotonin.mango.Common;
import br.org.scadabr.monitor.IntegerMonitor;
import br.org.scadabr.rt.SchedulerPool;
import br.org.scadabr.timer.cron.CronExpression;
import br.org.scadabr.timer.cron.SystemCronTask;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

@Deprecated// "Whats this for?)"
@Configurable
public class WorkItemMonitor extends SystemCronTask {

    
    @Autowired
    private SchedulerPool schedulerPool;
    /**
     * This method will set up the memory checking job. It assumes that the
     * corresponding system setting for running this job is true.
     */
    public void start() {
        schedulerPool.schedule(this);
    }

    private final IntegerMonitor mediumPriorityServiceQueueSize = new IntegerMonitor(
            "WorkItemMonitor.mediumPriorityServiceQueueSize", null);
    private final IntegerMonitor scheduledTimerTaskCount = new IntegerMonitor(
            "WorkItemMonitor.scheduledTimerTaskCount", null);
    private final IntegerMonitor highPriorityServiceQueueSize = new IntegerMonitor(
            "WorkItemMonitor.highPriorityServiceQueueSize", null);
    private final IntegerMonitor maxStackHeight = new IntegerMonitor("WorkItemMonitor.maxStackHeight", null);
    private final IntegerMonitor threadCount = new IntegerMonitor("WorkItemMonitor.threadCount", null);

    public WorkItemMonitor() {
        super(CronExpression.createPeriodBySecond(10, 0));

        Common.MONITORED_VALUES.addIfMissingStatMonitor(mediumPriorityServiceQueueSize);
        Common.MONITORED_VALUES.addIfMissingStatMonitor(scheduledTimerTaskCount);
        Common.MONITORED_VALUES.addIfMissingStatMonitor(highPriorityServiceQueueSize);
        Common.MONITORED_VALUES.addIfMissingStatMonitor(maxStackHeight);
        Common.MONITORED_VALUES.addIfMissingStatMonitor(threadCount);
    }

    @Override
    protected void run(long scheduledExecutionTime) {
        //BackgroundProcessing bp = Common.ctx.getBackgroundProcessing();

        mediumPriorityServiceQueueSize.setValue(0); //bp.getMediumPriorityServiceQueueSize());
        scheduledTimerTaskCount.setValue(schedulerPool.getSystemPoolSize());
        highPriorityServiceQueueSize.setValue(schedulerPool.getSystemActiveCount());

        // Check the stack heights
        int max = 0;
        Collection<StackTraceElement[]> stacks = Thread.getAllStackTraces().values();
        threadCount.setValue(stacks.size());
        for (StackTraceElement[] stack : stacks) {
            if (max < stack.length) {
                max = stack.length;
            }
        }
        maxStackHeight.setValue(max);
    }
        @Override
        protected boolean overrunDetected(long lastExecutionTime, long thisExecutionTime) {
            throw new ImplementMeException();
        }
}
