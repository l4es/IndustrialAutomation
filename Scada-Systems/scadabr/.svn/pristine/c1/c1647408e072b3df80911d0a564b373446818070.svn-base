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
import java.text.SimpleDateFormat;
import java.util.Date;



import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.rt.dataImage.types.NumericValue;
import com.serotonin.mango.view.ImplDefinition;
import br.org.scadabr.util.SerializationHelper;
import com.serotonin.mango.rt.dataImage.types.DoubleValue;
import java.util.EnumSet;


public class TimeRenderer extends BaseTextRenderer {

    private static ImplDefinition definition = new ImplDefinition("textRendererTime", "TIME", "textRenderer.time",
            EnumSet.of(DataType.DOUBLE));

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

    
    private String format;
    
    private int conversionExponent;

    private SimpleDateFormat formatInstance;

    public TimeRenderer() {
        // no op
    }

    public TimeRenderer(String format, int conversionExponent) {
        setFormat(format);
        this.conversionExponent = conversionExponent;
    }

    private void createFormatInstance() {
        formatInstance = new SimpleDateFormat(format);
    }

    @Override
    protected String getTextImpl(MangoValue value, int hint) {
        if (!(value instanceof NumericValue)) {
            return null;
        }
        return getText((long) ((DoubleValue)value).getDoubleValue(), hint);
    }

    @Override
    public String getText(double value, int hint) {
        long l = (long) value;

        if (hint == HINT_RAW || hint == HINT_SPECIFIC) {
            return new Long(l).toString();
        }

        l *= (long) Math.pow(10, conversionExponent);
        return formatInstance.format(new Date(l));
    }

    @Override
    protected String getColourImpl(MangoValue value) {
        return null;
    }

    public String getFormat() {
        return format;
    }

    public void setFormat(String format) {
        this.format = format;
        createFormatInstance();
    }

    public int getConversionExponent() {
        return conversionExponent;
    }

    public void setConversionExponent(int conversionExponent) {
        this.conversionExponent = conversionExponent;
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
        SerializationHelper.writeSafeUTF(out, format);
        out.writeInt(conversionExponent);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            format = SerializationHelper.readSafeUTF(in);
            createFormatInstance();
            conversionExponent = in.readInt();
        }
    }

    @Override
    public String getValueMessagePattern() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public String getMessagePattern() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public String getSuffix() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

}
