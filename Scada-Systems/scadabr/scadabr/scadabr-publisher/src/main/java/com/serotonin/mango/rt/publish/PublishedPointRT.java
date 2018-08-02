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
package com.serotonin.mango.rt.publish;

import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.rt.dataImage.DataPointListener;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.vo.publish.PublishedPointVO;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

/**
 * @author Matthew Lohbihler
 */
@Configurable
public class PublishedPointRT<T extends PublishedPointVO> implements DataPointListener {

    private final T vo;
    private final PublisherRT<T> parent;
    @Autowired
    private RuntimeManager runtimeManager;
    private boolean pointEnabled;

    public PublishedPointRT(T vo, PublisherRT<T> parent) {
        this.vo = vo;
        this.parent = parent;
        runtimeManager.addDataPointListener(vo.getDataPointId(), this);
        pointEnabled = runtimeManager.isDataPointRunning(vo.getDataPointId());
    }

    public void terminate() {
        runtimeManager.removeDataPointListener(vo.getDataPointId(), this);
    }

    @Override
    public void pointChanged(PointValueTime oldValue, PointValueTime newValue) {
        if (parent.getVo().isChangesOnly()) {
            parent.publish(vo, newValue);
        }
    }

    @Override
    public void pointSet(PointValueTime oldValue, PointValueTime newValue) {
        // no op. Everything gets handled in the other methods.
    }

    @Override
    public void pointUpdated(PointValueTime newValue) {
        if (!parent.getVo().isChangesOnly()) {
            parent.publish(vo, newValue);
        }
    }

    @Override
    public void pointBackdated(PointValueTime value) {
        // no op
    }

    public boolean isPointEnabled() {
        return pointEnabled;
    }

    @Override
    public void pointInitialized() {
        pointEnabled = true;
        parent.pointInitialized(this);
    }

    @Override
    public void pointTerminated() {
        pointEnabled = false;
        parent.pointTerminated(this);
    }

    public T getVo() {
        return vo;
    }
}
