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

import br.org.scadabr.DataType;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.ArrayList;
import java.util.List;
import br.org.scadabr.view.FormatPatternHolder;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.view.ImplDefinition;

abstract public class BaseTextRenderer implements TextRenderer, FormatPatternHolder {

    static List<ImplDefinition> definitions;

    static void ensureDefinitions() {
        if (definitions == null) {
            List<ImplDefinition> d = new ArrayList<>();
            d.add(AnalogRenderer.getDefinition());
            d.add(BinaryTextRenderer.getDefinition());
            d.add(MultistateRenderer.getDefinition());
            d.add(NoneRenderer.getDefinition());
            d.add(PlainRenderer.getDefinition());
            d.add(RangeRenderer.getDefinition());
            d.add(TimeRenderer.getDefinition());
            definitions = d;
        }
    }

    public static List<ImplDefinition> getImplementation(DataType dataType) {
        ensureDefinitions();
        List<ImplDefinition> impls = new ArrayList<>(definitions.size());
        for (ImplDefinition def : definitions) {
            if (def.supports(dataType)) {
                impls.add(def);
            }
        }
        return impls;
    }

    public static List<String> getExportTypes() {
        ensureDefinitions();
        List<String> result = new ArrayList<>(definitions.size());
        for (ImplDefinition def : definitions) {
            result.add(def.getExportName());
        }
        return result;
    }

    @Override
    public String getText(int hint) {
        if (hint == HINT_RAW) {
            return "";
        }
        return UNKNOWN_VALUE;
    }

    @Override
    public String getText(PointValueTime valueTime, int hint) {
        if (valueTime == null) {
            return getText(hint);
        }
        return getText(valueTime.toMangoValue(), hint);
    }

    @Override
    public String getText(MangoValue value, int hint) {
        if (value == null) {
            return getText(hint);
        }
        return getTextImpl(value, hint);
    }

    abstract protected String getTextImpl(MangoValue value, int hint);

    @Override
    public String getText(double value, int hint) {
        return Double.toString(value);
    }

    @Override
    public String getText(int value, int hint) {
        return Integer.toString(value);
    }

    @Override
    public String getText(boolean value, int hint) {
        return value ? "1" : "0";
    }

    @Override
    public String getText(String value, int hint) {
        return value;
    }

    @Override
    public String getMetaText() {
        return null;
    }

    //
    // / Colours
    //
    @Override
    public String getColour() {
        return null;
    }

    @Override
    public String getColour(PointValueTime valueTime) {
        if (valueTime == null) {
            return getColour();
        }
        return getColour(valueTime.toMangoValue());
    }

    @Override
    public String getColour(MangoValue value) {
        if (value == null) {
            return getColour();
        }
        return getColourImpl(value);
    }

    abstract protected String getColourImpl(MangoValue value);

    @Override
    public String getColour(double value) {
        return null;
    }

    @Override
    public String getColour(int value) {
        return null;
    }

    @Override
    public String getColour(boolean value) {
        return null;
    }

    @Override
    public String getColour(String value) {
        return null;
    }

    //
    // /
    // / Serialization
    // /
    //
    private static final long serialVersionUID = -1;
    private static final int version = 1;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(version);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        in.readInt(); // Read the version. Value is currently not used.
    }

}
