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



import com.serotonin.mango.rt.dataImage.types.BooleanValue;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.view.ImplDefinition;
import br.org.scadabr.util.SerializationHelper;
import java.util.EnumSet;


public class PlainRenderer extends BaseTextRenderer {

    private static ImplDefinition definition = new ImplDefinition("textRendererPlain", "PLAIN", "textRenderer.plain",
            EnumSet.of(DataType.BOOLEAN, DataType.ALPHANUMERIC, DataType.MULTISTATE, DataType.DOUBLE));

    public static ImplDefinition getDefinition() {
        return definition;
    }

    @Override
    public String getTypeName() {
        return definition.getName();
    }

    @Override
    public ImplDefinition getDef() {
        return definition;
    }

    
    private String suffix;

    public PlainRenderer() {
        // no op
    }

    public PlainRenderer(String suffix) {
        this.suffix = suffix;
    }

    @Override
    public String getMetaText() {
        return suffix;
    }

    @Override
    protected String getTextImpl(MangoValue value, int hint) {
        if (hint == HINT_RAW || suffix == null) {
            return getStringValue(value);
        }
        return getStringValue(value) + suffix;
    }

    private String getStringValue(MangoValue value) {
        if (value instanceof BooleanValue) {
            if (((BooleanValue)value).getBooleanValue()) {
                return "1";
            }
            return "0";
        }
        return value.toString();
    }

    @Override
    public String getSuffix() {
        return suffix;
    }

    public void setSuffix(String suffix) {
        this.suffix = suffix;
    }

    @Override
    protected String getColourImpl(MangoValue value) {
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
        SerializationHelper.writeSafeUTF(out, suffix);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            suffix = SerializationHelper.readSafeUTF(in);
        }
    }

    @Override
    public String getValueMessagePattern() {
        return "{0}";
    }

    @Override
    public String getMessagePattern() {
        return "{0} " + suffix;
    }

}
