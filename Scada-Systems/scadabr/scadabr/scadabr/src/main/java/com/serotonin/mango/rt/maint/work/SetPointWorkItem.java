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
package com.serotonin.mango.rt.maint.work;

import br.org.scadabr.timer.cron.SystemRunnable;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.vo.VO;
import com.serotonin.mango.rt.RuntimeManager;
import java.util.ArrayList;
import java.util.List;

import com.serotonin.mango.rt.dataImage.PointValueTime;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

/**
 * @author Matthew Lohbihler
 */
@Configurable
public class SetPointWorkItem implements SystemRunnable {

    private static final ThreadLocal<List<String>> threadLocal = new ThreadLocal<List<String>>();
    private static final int MAX_RECURSION = 10;

    private final int targetPointId;
    private final PointValueTime pvt;
    private final VO<?> source;
    private final List<String> sourceIds;
    @Autowired
    private RuntimeManager runtimeManager;

    public SetPointWorkItem(int targetPointId, PointValueTime pvt, VO<?> source) {
        this.targetPointId = targetPointId;
        this.pvt = pvt;
        this.source = source;

        if (threadLocal.get() == null) {
            sourceIds = new ArrayList<String>();
        } else {
            sourceIds = threadLocal.get();
        }
    }

    @Override
    public void run() {
        String sourceId = source.getNodeType().name() + "-"
                + Integer.toString(source.getId());

        // Check if we've reached the maximum number of hits for this point
        int count = 0;
        for (String id : sourceIds) {
            if (id.equals(sourceId)) {
                count++;
            }
        }

        if (true) throw new ImplementMeException();
        if (count > MAX_RECURSION) {
            //TODO was ((SetpointSource)source).raiseRecursionFailureEvent();
            return;
        }

        sourceIds.add(sourceId);
        threadLocal.set(sourceIds);
        try {
            runtimeManager.setDataPointValue(pvt, source);
        } finally {
            threadLocal.remove();
        }
    }

    /*
    @Override
    public int getPriority() {
        return WorkItem.PRIORITY_HIGH;
    }
    */
}
