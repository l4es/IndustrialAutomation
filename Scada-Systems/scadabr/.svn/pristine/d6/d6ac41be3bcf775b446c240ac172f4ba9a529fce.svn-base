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
package com.serotonin.mango.rt.event.compound;

import java.util.List;

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.rt.event.schedule.ScheduledEventManager;
import com.serotonin.mango.rt.event.SimpleEventDetector;
import br.org.scadabr.utils.i18n.LocalizableException;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

/**
 * @author Matthew Lohbihler
 */
@Configurable
public class EventDetectorWrapper extends LogicalOperator {

    private final String detectorKey;
    @Autowired
    private ScheduledEventManager scheduledEventManager;
    private SimpleEventDetector source;

    public EventDetectorWrapper(String detectorKey) {
        this.detectorKey = detectorKey;
    }

    @Override
    public boolean evaluate() {
        if (source == null) {
            throw new ShouldNeverHappenException("No runtime object available");
        }
        return source.isEventActive();
    }

    @Override
    public String toString() {
        return detectorKey;
    }

    @Override
    public void initialize() throws LocalizableException {
        source = scheduledEventManager.getSimpleEventDetector(detectorKey);
        if (source == null) {
            throw new LocalizableException("compoundDetectors.initError.wrapper", detectorKey);
        }
    }

    @Override
    public void initSource(CompoundEventDetectorRT parent) {
        source.addListener(parent);
    }

    @Override
    public void terminate(CompoundEventDetectorRT parent) {
        if (source != null) {
            source.removeListener(parent);
        }
    }

    @Override
    protected void appendDetectorKeys(List<String> keys) {
        keys.add(detectorKey);
    }
}
