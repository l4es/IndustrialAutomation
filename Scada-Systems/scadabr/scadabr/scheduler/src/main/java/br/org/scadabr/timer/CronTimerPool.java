/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.timer;

import java.util.Arrays;
import java.util.Date;
import java.util.concurrent.Future;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author aploese
 * @param <T>
 * @param <V>
 */
public class CronTimerPool<T extends CronTask, V extends Runnable> {
    
    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_SCHEDULER);

    class TimerThread extends Thread {

        boolean newTasksMayBeScheduled = true;

        private final TaskQueue queue;

        TimerThread(TaskQueue queue) {
            this.queue = queue;
        }

        @Override
        public void run() {
            try {
                mainLoop();
            } catch (Throwable t) {
                LOG.log(Level.SEVERE, "CronTimePOOL EX", t);
            } finally {
                // Someone killed this Thread, behave as if Timer cancelled
                synchronized (queue) {
                    newTasksMayBeScheduled = false;
                    queue.clear();  // Eliminate obsolete references
                }
            } 
        }

        /**
         * The main timer loop. (See class comment.)
         */
        private void mainLoop() {
            while (true) {
                try {
                    CronTask task;
                    boolean taskFired;
                    long executionTime;
                    synchronized (queue) {
                        // Wait for queue to become non-empty
                        while (queue.isEmpty() && newTasksMayBeScheduled) {
                            queue.wait();
                        }
                        if (queue.isEmpty()) {
                            break; // Queue is empty and will forever remain; die
                        }
                        // Queue nonempty; look at first evt and do the right thing
                        long currentTime;
                        task = queue.getMin();
                        synchronized (task.lock) {
                            if (task.state == CronTask.CANCELLED) {
                                queue.removeMin();
                                continue;  // No action required, poll queue again
                            }
                            currentTime = System.currentTimeMillis();
                            executionTime = task.nextExecutionTime;
                            if (taskFired = (executionTime <= currentTime)) {
                                queue.rescheduleMin(
                                        task.calcNextExecutionTimeAfter());
                            }
                        }
                        if (!taskFired) {
                            // Task hasn't yet fired; wait
                            queue.wait(executionTime - currentTime);
                        }
                    }
                    if (taskFired) {
                        // Task fired; run it, holding no locks
                        try {
                            tpe.execute(task.createRunner(executionTime));
                        } catch (CronTask.OverrideDieException e) {
                            
                        }
                    }
                } catch (InterruptedException e) {
                }
            }
        }
    }

    ThreadPoolExecutor tpe;

    public <O> Future<O> submit(CronTaskCallable<O> task) throws InterruptedException {
        return tpe.submit(task); 
    }

    
    public void execute(V r) {
        tpe.execute(r);
    }

    public Future<?> submit(V r) {
        return tpe.submit(r);
    }

    public int getPoolSize() {
        return tpe.getPoolSize();
    }

    public int getActiveCount() {
        return tpe.getActiveCount();
    }

    public int getQueueSize() {
        return tpe.getQueue().size();
    }

    private final TaskQueue queue = new TaskQueue();

    private final TimerThread thread = new TimerThread(queue);

    private final Object threadReaper = new Object() {
        protected void finalize() throws Throwable {
            synchronized (queue) {
                thread.newTasksMayBeScheduled = false;
                queue.notify(); // In case queue is empty.
            }
        }
    };

    private final static AtomicInteger nextSerialNumber = new AtomicInteger(0);

    private static int serialNumber() {
        return nextSerialNumber.getAndIncrement();
    }

    public CronTimerPool(int corePoolSize, int maximumPoolSize, long keepAliveTime, TimeUnit unit) {
        thread.setName("CronTimerPool-" + serialNumber());
        thread.setDaemon(true);
        thread.start();
        tpe = new ThreadPoolExecutor(corePoolSize, maximumPoolSize, keepAliveTime, unit, new LinkedBlockingQueue<Runnable>());
    }

    public void schedule(T task) {

        synchronized (queue) {
            if (!thread.newTasksMayBeScheduled) {
                throw new IllegalStateException("Timer already cancelled.");
            }

            synchronized (task.lock) {
                if (task.state != CronTask.VIRGIN) {
                    throw new IllegalStateException(
                            "Task already scheduled or cancelled");
                }
                final long time = System.currentTimeMillis();
                task.calcNextExecutionTimeIncludingNow(time);
                System.err.println("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX "  + time + " " + new Date(time));
                System.err.println("NEXT EXECUTION TIME: " + task.nextExecutionTime + " " + new Date(task.nextExecutionTime));
                task.state = CronTask.SCHEDULED;
                task.tpe = this.tpe;
            }

            queue.add(task);
            if (queue.getMin() == task) {
                queue.notify();
            }
        }
    }
    
    public boolean awaitTermination(long timeout, TimeUnit unit) throws InterruptedException {
        return tpe.awaitTermination(timeout, unit);
    }

    public void shutdown() {
        synchronized (queue) {
            thread.newTasksMayBeScheduled = false;
            queue.clear();
            queue.notify();  // In case queue was already empty.
        }
        tpe.shutdown();
    }

    public int purge() {
        int result = 0;

        synchronized (queue) {
            for (int i = queue.size(); i > 0; i--) {
                if (queue.get(i).state == CronTask.CANCELLED) {
                    queue.quickRemove(i);
                    result++;
                }
            }

            if (result != 0) {
                queue.heapify();
            }
        }

        return result;
    }
}

class TaskQueue {

    private CronTask[] queue = new CronTask[128];

    private int size = 0;

    int size() {
        return size;
    }

    void add(CronTask task) {
        // Grow backing store if necessary
        if (size + 1 == queue.length) {
            queue = Arrays.copyOf(queue, 2 * queue.length);
        }

        queue[++size] = task;
        fixUp(size);
    }

    CronTask getMin() {
        return queue[1];
    }

    CronTask get(int i) {
        return queue[i];
    }

    void removeMin() {
        queue[1] = queue[size];
        queue[size--] = null;  // Drop extra reference to prevent memory leak
        fixDown(1);
    }

    void quickRemove(int i) {
        assert i <= size;

        queue[i] = queue[size];
        queue[size--] = null;  // Drop extra ref to prevent memory leak
    }

    void rescheduleMin(long newTime) {
        queue[1].nextExecutionTime = newTime;
        fixDown(1);
    }

    boolean isEmpty() {
        return size == 0;
    }

    void clear() {
        // Null out task references to prevent memory leak
        for (int i = 1; i <= size; i++) {
            queue[i] = null;
        }

        size = 0;
    }

    private void fixUp(int k) {
        while (k > 1) {
            int j = k >> 1;
            if (queue[j].nextExecutionTime <= queue[k].nextExecutionTime) {
                break;
            }
            CronTask tmp = queue[j];
            queue[j] = queue[k];
            queue[k] = tmp;
            k = j;
        }
    }

    private void fixDown(int k) {
        int j;
        while ((j = k << 1) <= size && j > 0) {
            if (j < size
                    && queue[j].nextExecutionTime > queue[j + 1].nextExecutionTime) {
                j++; // j indexes smallest kid
            }
            if (queue[k].nextExecutionTime <= queue[j].nextExecutionTime) {
                break;
            }
            CronTask tmp = queue[j];
            queue[j] = queue[k];
            queue[k] = tmp;
            k = j;
        }
    }

    void heapify() {
        for (int i = size / 2; i >= 1; i--) {
            fixDown(i);
        }
    }
}
