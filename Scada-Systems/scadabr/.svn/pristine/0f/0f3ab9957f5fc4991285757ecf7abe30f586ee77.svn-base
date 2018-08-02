/*
 Mango - Open Source M2M - http://mango.serotoninsoftware.com
 Copyright (C) 2006-2011 Serotonin Software Technologies Inc.
 @author Matthew Lohbihler
    
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
package com.serotonin.mango.vo.report;

import br.org.scadabr.utils.ImplementMeException;
import java.text.ParseException;
import java.util.HashMap;
import java.util.Map;

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.rt.SchedulerPool;
import br.org.scadabr.timer.cron.SystemCronTask;
import com.serotonin.mango.rt.maint.work.ReportWorkItem;
import java.util.TimeZone;
import java.util.logging.Logger;
import javax.inject.Inject;
import javax.inject.Named;

/**
 * @author Matthew Lohbihler
 */
@Named
public class ReportTaskManager {

    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_REPORTS);

    class ReportTask extends SystemCronTask {

        private final ReportVO report;

        private ReportTask(String pattern, TimeZone tz, ReportVO report) throws ParseException {
            super(pattern, tz);
            this.report = report;
        }

        @Override
        protected void run(long scheduledExecutionTime) {
            new ReportWorkItem().queueReport(report);
        }

        @Override
        protected boolean overrunDetected(long lastExecutionTime, long thisExecutionTime) {
            LOG.severe("Report Overrun detected");
            return true;
        }

    }

    @Inject
    private SchedulerPool schedulerPool;

    private final Map<Integer, ReportTask> jobRegistry = new HashMap<>();

    public void scheduleReportJob(ReportVO report) {
        synchronized (jobRegistry) {
            // Ensure that there is no existing job.
            unscheduleReportJob(report);

            if (report.isSchedule()) {
                ReportTask reportJob;
                if (report.isCronScheduled()) {
                    try {
                        reportJob = new ReportTask(report.getScheduleCron(), report.getTimeZone(), report);
                    } catch (ParseException e) {
                        throw new ShouldNeverHappenException(e);
                    }
                } else {
                    throw new ImplementMeException(); //WAS: reportJob = new ReportTaskManager(report.getSchedulePeriod(), report.getRunDelayMinutes() * 60, report);
                }

                jobRegistry.put(report.getId(), reportJob);
                schedulerPool.schedule(reportJob);
            }
        }
    }

    public void unscheduleReportJob(ReportVO report) {
        synchronized (jobRegistry) {
            final ReportTask reportJob = jobRegistry.remove(report.getId());
            if (reportJob != null) {
                reportJob.cancel();
            }
        }
    }

}
