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
package com.serotonin.mango.rt.event.detectors;

import br.org.scadabr.utils.ImplementMeException;
import com.serotonin.mango.util.timeout.RunClient;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.util.timeout.SystemRunTask;

/**
 * This class is a base class for detectors that need to schedule timeouts for
 * their operation. Subclasses may use schedules for timeouts that make them
 * active, or that make them inactive.
 *
 * @author Matthew Lohbihler
 */
abstract public class TimeoutDetectorRT<T extends PointValueTime> extends PointEventDetectorRT<T> implements RunClient {

    /**
     * Internal configuration field. The millisecond version of the duration
     * fields.
     */
    private long durationMS;

    /**
     * Internal configuration field. The human-readable description of the
     * duration fields.
     */
    private LocalizableMessage durationDescription;

    /**
     * Internal configuration field. The unique name for this event producer to
     * be used in the scheduler (if required).
     */
    // TODO is this right???
    private SystemRunTask task;

    @Override
    public void initialize() {
        durationMS = vo.getDurationType().getMillis(vo.getDuration());
        durationDescription = vo.getDurationDescription();

        super.initialize();
    }

    protected boolean isJobScheduled() {
        return task != null;
    }

    @Override
    public void terminate() {
        super.terminate();
        cancelTask();
    }

    protected LocalizableMessage getDurationDescription() {
        return durationDescription;
    }

    protected long getDurationMS() {
        return durationMS;
    }

    protected void scheduleJob(long timeout) {
        if (task != null) {
            cancelTask();
        }
        throw new ImplementMeException(); //WAS task = new TimeoutTask(new Date(timeout), this);
    }

    protected void unscheduleJob() {
        cancelTask();
    }

    @Override
    synchronized public final void run(long fireTime) {
        scheduleTimeoutImpl(fireTime);
        task = null;
    }

    abstract protected void scheduleTimeoutImpl(long fireTime);

    synchronized private void cancelTask() {
        if (task != null) {
            task.cancel();
            task = null;
        }
    }
}
