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
package com.serotonin.mango.rt.maint;

import br.org.scadabr.dao.DataPointDao;
import br.org.scadabr.dao.EventDao;
import br.org.scadabr.dao.PointValueDao;
import br.org.scadabr.dao.SystemSettingsDao;
import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.timer.cron.SystemCronTask;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.utils.TimePeriods;
import com.serotonin.mango.Common;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.rt.dataImage.types.ImageValue;
import com.serotonin.mango.vo.DataPointVO;
import java.io.File;
import java.text.ParseException;
import java.util.Collections;
import java.util.Date;
import java.util.List;
import java.util.TimeZone;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.joda.time.DateTime;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

@Configurable
public class DataPurge {

    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCADABR_CORE);
    @Autowired
    private RuntimeManager runtimeManager;
    @Autowired
    private DataPointDao dataPointDao;
    @Autowired
    private EventDao eventDao;
    @Autowired
    private SystemSettingsDao systemSettingsDao;
    @Autowired
    private PointValueDao pointValueDao;
    @Autowired
    private Common common;

    private long runtime;

    private final RuntimeManager rm = runtimeManager;

    synchronized public void execute(long runtime) {
        this.runtime = runtime;
        executeImpl();
    }

    private void executeImpl() {
        LOG.info("Data purge started");

        if (true) return; //TODO implement this
// Get the data point information.
        int deleteCount = 0;
        for (DataPointVO dataPoint : dataPointDao.getDataPoints(false)) {
            deleteCount += purgePoint(dataPoint);
        }
        // if (deleteCount > 0)
        // new PointValueDao().compressTables();

        LOG.log(Level.INFO, "Data purge ended, {0} point values deleted", deleteCount);

        // File data purge
        filedataPurge();

        // Event purge
        eventPurge();

        // Report instance purge
//TODO move to report package        reportPurge();
    }

    private long purgePoint(DataPointVO dataPoint) {

// No matter when this purge actually runs, we want it to act like it's midnight.
        DateTime cutoff = new DateTime(runtime);
        cutoff = TimePeriods.DAYS.truncateDateTime(cutoff);
        cutoff = dataPoint.getPurgeType().minus(cutoff, dataPoint.getPurgePeriod());

        throw new ImplementMeException();
//        return rm.purgeDataPointValues(dataPoint.getId(), cutoff.getMillis());
    }

    private void filedataPurge() {
        int deleteCount = 0;

        // Find all ids for which there should be a corresponding file
        List<Long> validIds = pointValueDao.getFiledataIds();

        // Get all of the existing filenames.
        File dir = new File(common.getFiledataPath());
        String[] files = dir.list();
        if (files != null) {
            for (String filename : files) {
                long pointId = ImageValue.parseIdFromFilename(filename);
                if (Collections.binarySearch(validIds, pointId) < 0) {
                    // Not found, so the point was deleted from the database. Delete the file.
                    new File(dir, filename).delete();
                    deleteCount++;
                }
            }
        }

        if (deleteCount > 0) {
            LOG.log(Level.INFO, "Filedata purge ended, {0} files deleted", deleteCount);
        }
    }

    private void eventPurge() {
        DateTime cutoff = TimePeriods.DAYS.truncateDateTime(new DateTime(runtime));
        cutoff = systemSettingsDao.getEvents().getCutoff(cutoff);

        int deleteCount = eventDao.purgeEventsBefore(cutoff.getMillis());
        if (deleteCount > 0) {
            LOG.log(Level.INFO, "Event purge ended, {0} events deleted", deleteCount);
        }
    }

    /*
    private void reportPurge() {
        DateTime cutoff = TimePeriods.DAYS.truncateDateTime(new DateTime(runtime));
        cutoff = systemSettingsDao.getReports().getCutoff(cutoff);

        int deleteCount = reportDao.purgeReportsBefore(cutoff.getMillis());
        if (deleteCount > 0) {
            LOG.log(Level.INFO, "Report purge ended, {0} report instances deleted", deleteCount);
        }
    }
*/
    public static class DataPurgeTask extends SystemCronTask {

        public DataPurgeTask(String pattern, TimeZone tz) throws ParseException {
            super(pattern, tz);
        }

        @Override
        protected void run(long scheduledExecutionTime) {
            new DataPurge().execute(scheduledExecutionTime);
        }

        @Override
        protected boolean overrunDetected(long lastExecutionTime, long thisExecutionTime) {
            LOG.log(Level.SEVERE, "DataPurge overrun last{0} current:{1}", new Object[]{new Date(lastExecutionTime), new Date(thisExecutionTime)});
            return false;
        }
    }
}
