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
package com.serotonin.mango.view.component;

import br.org.scadabr.DataType;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.Map;

import com.serotonin.mango.Common;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.types.NumericValue;
import com.serotonin.mango.view.DynamicImage;
import com.serotonin.mango.view.ImplDefinition;
import br.org.scadabr.util.SerializationHelper;
import com.serotonin.mango.rt.dataImage.DoubleValueTime;
import com.serotonin.mango.rt.dataImage.types.DoubleValue;
import java.util.EnumSet;

/**
 * @author Matthew Lohbihler
 */

public class DynamicGraphicComponent extends PointComponent<DoubleValueTime> {

    public static ImplDefinition DEFINITION = new ImplDefinition("dynamicGraphic", "DYNAMIC_GRAPHIC",
            "graphic.dynamicGraphic", EnumSet.of(DataType.DOUBLE));

    private DynamicImage dynamicImage;
    
    private boolean displayText;
    
    private double min;
    
    private double max;

    public DynamicImage tgetDynamicImage() {
        return dynamicImage;
    }

    public void tsetDynamicImage(DynamicImage dynamicImage) {
        this.dynamicImage = dynamicImage;
    }

    public boolean isDisplayText() {
        return displayText;
    }

    public void setDisplayText(boolean displayText) {
        this.displayText = displayText;
    }

    public double getMax() {
        return max;
    }

    public void setMin(double min) {
        this.min = min;
    }

    public double getMin() {
        return min;
    }

    public void setMax(double max) {
        this.max = max;
    }

    @Override
    public ImplDefinition definition() {
        return DEFINITION;
    }

    @Override
    public String snippetName() {
        return "dynamicImageContent";
    }

    public String getImage() {
        if (dynamicImage == null) {
            return null;
        }
        return dynamicImage.getImageFilename();
    }

    public double getProportion(DoubleValueTime pointValue) {
        if (pointValue == null) {
            return 0;
        }

        double dvalue = pointValue.getDoubleValue();
        double proportion = (dvalue - min) / (max - min);
        if (proportion > 1) {
            return 1;
        }
        if (proportion < 0) {
            return 0;
        }
        return proportion;
    }

    public int getHeight() {
        if (dynamicImage == null) {
            return 0;
        }
        return dynamicImage.getHeight();
    }

    public int getWidth() {
        if (dynamicImage == null) {
            return 0;
        }
        return dynamicImage.getWidth();
    }

    public int getTextX() {
        if (dynamicImage == null) {
            return 0;
        }
        return dynamicImage.getTextX();
    }

    public int getTextY() {
        if (dynamicImage == null) {
            return 0;
        }
        return dynamicImage.getTextY();
    }

    public String getDynamicImageId() {
        if (dynamicImage == null) {
            return null;
        }
        return dynamicImage.getId();
    }

    @Override
    public void addDataToModel(Map<String, Object> model, DoubleValueTime pointValue) {
        model.put("proportion", getProportion(pointValue));
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
        if (dynamicImage == null) {
            SerializationHelper.writeSafeUTF(out, null);
        } else {
            SerializationHelper.writeSafeUTF(out, dynamicImage.getId());
        }
        out.writeDouble(min);
        out.writeDouble(max);
        out.writeBoolean(displayText);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            dynamicImage = Common.ctx.getDynamicImage(SerializationHelper.readSafeUTF(in));
            min = in.readDouble();
            max = in.readDouble();
            displayText = in.readBoolean();
        }
    }

}
