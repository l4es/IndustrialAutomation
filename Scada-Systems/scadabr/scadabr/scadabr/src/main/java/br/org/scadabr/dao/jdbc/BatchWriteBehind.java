/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.dao.jdbc;

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.jdbc.DatabaseAccessFactory;
import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.rt.SchedulerPool;
import br.org.scadabr.timer.cron.SystemCallable;
import com.serotonin.mango.rt.dataImage.DoubleValueTime;
import java.util.LinkedList;
import java.util.Queue;
import java.util.concurrent.Future;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.inject.Inject;
import javax.inject.Named;
import org.springframework.dao.ConcurrencyFailureException;
import org.springframework.dao.DataAccessException;
import org.springframework.jdbc.core.JdbcTemplate;

/**
 *
 * @author aploese
 */
@Named
class BatchWriteBehind {

    class BatchWriteBehindCallable implements SystemCallable<Integer> {

        private JdbcTemplate ejt;
        private Future<Integer> future;
        private boolean sheduledOrRunning;
        private final Object sheduledOrRunningLock = new Object();

        public BatchWriteBehindCallable() {
        }

        private void writeInto(DoubleValueTime pvt, Object[] params, int index) {
            index *= BatchWriteBehind.POINT_VALUE_INSERT_VALUES_COUNT;
            params[index++] = pvt.getDataPointId();
            params[index++] = pvt.getDoubleValue();
            params[index++] = pvt.getTimestamp();
        }

        @Override
        public Integer call() {
            LOG.log(Level.FINE, "Start Write Batch entries: {0}", doubleValuesToWrite.size());
            int entriesWritten = 0;
            try {
                DoubleValueTime[] inserts;
                while (!doubleValuesToWrite.isEmpty()) {
                    synchronized (doubleValuesToWrite) {
                        inserts = new DoubleValueTime[doubleValuesToWrite.size() < maxRows ? doubleValuesToWrite.size() : maxRows];
                        for (int i = 0; i < inserts.length; i++) {
                            inserts[i] = doubleValuesToWrite.remove();
                        }
                    }
                    // Create the sql and parameters
                    Object[] params = new Object[inserts.length * POINT_VALUE_INSERT_VALUES_COUNT];
                    StringBuilder sb = new StringBuilder();
                    sb.append(PointValueDaoImpl.POINT_VALUE_INSERT_START);
                    for (int i = 0; i < inserts.length; i++) {
                        if (i > 0) {
                            sb.append(',');
                        }
                        sb.append(PointValueDaoImpl.POINT_VALUE_INSERT_VALUES);
                        writeInto(inserts[i], params, i);
                    }
                    // Insert the data
                    int retries = 10;
                    while (true) {
                        try {
                            final long start = System.currentTimeMillis();
                            LOG.log(Level.FINER, "Concurrency saving call ejt");
                            final int count = ejt.update(sb.toString(), params);
                            entriesWritten += count;
                            if (LOG.isLoggable(Level.FINER)) {
                                LOG.log(Level.FINER, "Concurrency saving SUCCESS {0} rows in {1}ms", new Object[]{count, System.currentTimeMillis() - start});
                            }
                            break;
                        } catch (ConcurrencyFailureException e) {
                            if (retries <= 0) {
                                LOG.log(Level.SEVERE, "Concurrency failure saving {0} batch inserts after 10 tries. Data lost.", inserts.length);
                                break;
                            }
                            int wait = (10 - retries) * 100;
                            try {
                                if (wait > 0) {
                                    synchronized (this) {
                                        wait(wait);
                                    }
                                }
                            } catch (InterruptedException ie) {
                                // no op
                            }
                            retries--;
                        } catch (DataAccessException e) {
                            LOG.log(Level.SEVERE, "Error saving batch inserts. Data lost.", e);
                            break;
                        } catch (Throwable t) {
                            LOG.log(Level.SEVERE, "Unknown Error saving batch inserts. Data lost.", t);
                            break;
                        }
                    }
                }
                return entriesWritten;
            } finally {
                LOG.finer("Will finish Write Batch");
                synchronized (sheduledOrRunningLock) {
                    future = null;
                    sheduledOrRunning = false;
                }
                LOG.finer("Write Batch finished");
            }
        }

    }

    static final int POINT_VALUE_INSERT_VALUES_COUNT = 3;
    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_DAO);
    private final BatchWriteBehindCallable batchWriteBehindCallable;

    @Inject
    private SchedulerPool schedulerPool;
    /**
     *
     * BatchWriteBehindEntry are collected until a minimum size s reached.
     */
    private final Queue<DoubleValueTime> doubleValuesToWrite = new LinkedList<>();

    private int maxRows;

    public void init(DatabaseAccessFactory daf) {
        switch (daf.getDatabaseType()) {
            case DERBY:
                maxRows = 1000;
                break;
            case MSSQL:
                maxRows = 524;
                break;
            case MYSQL:
                maxRows = 2000;
                break;
            default:
                throw new ShouldNeverHappenException("Unknown database type: " + daf.getDatabaseType());
        }
    }

    void add(DoubleValueTime e, JdbcTemplate ejt) {
        boolean needsFlush;
        synchronized (doubleValuesToWrite) {
            doubleValuesToWrite.add(e);
            needsFlush = (doubleValuesToWrite.size() > maxRows) && !batchWriteBehindCallable.sheduledOrRunning;
        }
        if (needsFlush) {
            flush(ejt);
        }
    }

    public BatchWriteBehind() {
        batchWriteBehindCallable = new BatchWriteBehindCallable();
    }

    /**
     *
     * @param ejt
     * @return a Future if any data to write otherwise null.
     */
    public Future<Integer> flush(JdbcTemplate ejt) {
        LOG.log(Level.FINE, "flush called, entries: {0}", doubleValuesToWrite.size());
        synchronized (batchWriteBehindCallable) {
            Future<Integer> f = batchWriteBehindCallable.future;
            if (f != null) {
                LOG.fine("flush already scheduled");
                return f;
            }
            try {
                if (doubleValuesToWrite.isEmpty()) {
                    LOG.fine("nothing to flush");
                    return null;
                }
                batchWriteBehindCallable.ejt = ejt;
                batchWriteBehindCallable.sheduledOrRunning = true;
                LOG.finest("will schedule Batch");
                f = schedulerPool.submit(batchWriteBehindCallable);
                LOG.finest("Batch scheduled");
                synchronized (batchWriteBehindCallable.sheduledOrRunningLock) {
                    if (batchWriteBehindCallable.sheduledOrRunning) {
                        LOG.fine("Batch runnung");
                        batchWriteBehindCallable.future = f;
                    } else {
                        LOG.fine("Batch finished before");
                    }

                }

            } catch (InterruptedException ex) {
                LOG.log(Level.SEVERE, null, ex);
                throw new RuntimeException("Cant flush", ex);
            }
            return batchWriteBehindCallable.future;
        }
    }

}
