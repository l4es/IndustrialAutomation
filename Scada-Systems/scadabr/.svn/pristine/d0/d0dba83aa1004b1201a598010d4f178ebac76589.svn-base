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
package com.serotonin.mango.view.text;

import br.org.scadabr.view.FormatPatternHolder;
import java.io.Serializable;

import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.view.ImplDefinition;

public interface TextRenderer extends Serializable, FormatPatternHolder {

    public static final int TYPE_ANALOG = 1;
    public static final int TYPE_BINARY = 2;
    public static final int TYPE_MULTISTATE = 3;
    public static final int TYPE_PLAIN = 4;
    public static final int TYPE_RANGE = 5;

    /**
     * Do not render the value. Just return the java-formatted version of the
     * value.
     */
    public static final int HINT_RAW = 1;
    /**
     * Render the value according to the full functionality of the renderer.
     */
    public static final int HINT_FULL = 2;
    /**
     * Render the value in a way that does not generalize. Currently only used
     * to prevent analog range renderers from obfuscating a numeric into a
     * descriptor.
     */
    public static final int HINT_SPECIFIC = 3;

    public static final String UNKNOWN_VALUE = "(n/a)";

    @Deprecated //TODO localized value is passed 
    String getText(int hint);

    @Deprecated //TODO localized value is passed 
    String getText(PointValueTime valueTime, int hint);

    @Deprecated //TODO localized value is passed 
    String getText(MangoValue value, int hint);

    @Deprecated //TODO localized value is passed 
    String getText(double value, int hint);

    @Deprecated //TODO localized value is passed 
    String getText(int value, int hint);

    @Deprecated //TODO localized value is passed 
    String getText(boolean value, int hint);

    @Deprecated //TODO localized value is passed 
    String getText(String value, int hint);

    String getMetaText();

    String getColour();

    String getColour(PointValueTime valueTime);

    String getColour(MangoValue value);

    String getColour(double value);

    String getColour(int value);

    String getColour(boolean value);

    String getColour(String value);

    String getTypeName();

    ImplDefinition getDef();
}
