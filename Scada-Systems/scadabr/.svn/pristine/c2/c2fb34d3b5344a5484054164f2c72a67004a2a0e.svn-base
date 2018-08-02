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
package com.serotonin.mango.rt.dataImage;

import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.NodeType;

/**
 * This class provides a way of arbitrarily annotating a PointValue. Point value
 * annotations should not be confused with Java annotations. A point value
 * annotation will typically explain the source of the value when it did not
 * simply come from data source.
 *
 * @see SetPointSource
 * @author Matthew Lohbihler
 */
public class PointValueAnnotation {

    private static final long serialVersionUID = -1;

    /**
     * The type of source that created the annotation.
     *
     * @see SetPointSource
     */
    private final NodeType sourceType;

    /**
     * The id of the source that created the annotation.
     *
     * @see SetPointSource
     */
    private final int sourceId;

    /**
     * An arbitrary description of the source, human readable. This depends on
     * the source type, but will typically be the source's name. For example,
     * for a user source it would be the username.
     *
     * @see SetPointSource
     */
    private String sourceDescriptionArgument;

    public PointValueAnnotation(NodeType sourceType, int sourceId) {
        this.sourceType = sourceType;
        this.sourceId = sourceId;
    }

    public int getSourceId() {
        return sourceId;
    }

    public NodeType getSourceType() {
        return sourceType;
    }

    public String getSourceDescriptionKey() {
        throw new ImplementMeException();
        /*
        switch (sourceType) {
            case ANONYMOUS:
                return "annotation.anonymous";
            case EVENT_HANDLER:
                return "annotation.eventHandler";
            case USER:
                return "annotation.user";
            case POINT_LINK:
                return "annotation.pointLink";
        }
        return null;
*/
    }

    public String getSourceDescriptionArgument() {
        return sourceDescriptionArgument;
    }

    public void setSourceDescriptionArgument(String sourceDescriptionArgument) {
        this.sourceDescriptionArgument = sourceDescriptionArgument;
    }

    public LocalizableMessage getAnnotation() {
        if (sourceDescriptionArgument == null) {
            return new LocalizableMessageImpl(getSourceDescriptionKey(), new LocalizableMessageImpl("common.deleted"));
        }
        return new LocalizableMessageImpl(getSourceDescriptionKey(), sourceDescriptionArgument);
    }
}
