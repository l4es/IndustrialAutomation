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

import br.org.scadabr.utils.ImplementMeException;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.serotonin.mango.Common;
import br.org.scadabr.timer.cron.CronExpression;
import br.org.scadabr.timer.cron.SystemCronTask;

/**
 * @author Matthew Lohbihler
 */
@Deprecated//Whats this for?
public class MemoryCheck extends SystemCronTask {

    private static final Log log = LogFactory.getLog(MemoryCheck.class);
    private static final int PERIOD_IN_S = 5; // Run every five seconds.

    /**
     * This method will set up the memory checking job. It assumes that the
     * corresponding system setting for running this job is true.
     */
    public static void start() {
       throw new ImplementMeException();
//        Common.systemCronPool.schedule(new MemoryCheck());
    }

    public MemoryCheck() {
        super(CronExpression.createPeriodBySecond(PERIOD_IN_S, 0));
    }

    @Override
    protected void run(long scheduledExecutionTime) {
        memoryCheck();
    }

    public static void memoryCheck() {
        Runtime rt = Runtime.getRuntime();
        log.info("Free=" + rt.freeMemory() + ", total=" + rt.totalMemory() + ", max=" + rt.maxMemory());
    }

        @Override
        protected boolean overrunDetected(long lastExecutionTime, long thisExecutionTime) {
            throw new ImplementMeException();
        }
}
