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
import com.serotonin.mango.rt.dataImage.BooleanValueTime;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;



import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.types.BooleanValue;
import com.serotonin.mango.view.ImplDefinition;
import java.util.EnumSet;

/**
 * @author Matthew Lohbihler
 */

public class BinaryGraphicComponent extends ImageSetComponent<BooleanValueTime> {

    public static ImplDefinition DEFINITION = new ImplDefinition("binaryGraphic", "BINARY_GRAPHIC",
            "graphic.binaryGraphic", EnumSet.of(DataType.BOOLEAN));

    private int zeroImage;
    private int oneImage;

    public int getZeroImage() {
        return zeroImage;
    }

    public void setZeroImage(int zeroImage) {
        this.zeroImage = zeroImage;
    }

    public int getOneImage() {
        return oneImage;
    }

    public void setOneImage(int oneImage) {
        this.oneImage = oneImage;
    }

    @Override
    public ImplDefinition definition() {
        return DEFINITION;
    }

    @Override
    public String getImage(BooleanValueTime pointValue) {
        boolean bvalue = false;
        if (pointValue != null) {
            bvalue = pointValue.getBooleanValue();
        }
        return imageSet.getImageFilename(bvalue ? oneImage : zeroImage);
    }

    /*
    @Override
    public void validate(DwrResponseI18n response) {
        super.validate(response);

        if (zeroImage < 0) {
            response.addContextual("zeroImageIndex", "validate.cannotBeNegative");
        }
        if (oneImage < 0) {
            response.addContextual("oneImageIndex", "validate.cannotBeNegative");
        }

        if (imageSet != null) {
            if (zeroImage >= imageSet.getImageCount()) {
                response.addContextual("zeroImageIndex", "emport.error.component.imageIndex", zeroImage, imageSet.getId(), imageSet.getImageCount() - 1);
            }
            if (oneImage >= imageSet.getImageCount()) {
                response.addContextual("oneImageIndex", "emport.error.component.imageIndex", oneImage, imageSet.getId(), imageSet.getImageCount() - 1);
            }
        }
    }
*/
    //
    // /
    // / Serialization
    // /
    //
    private static final long serialVersionUID = -1;
    private static final int version = 1;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(version);
        out.writeInt(zeroImage);
        out.writeInt(oneImage);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            zeroImage = in.readInt();
            oneImage = in.readInt();
        }
    }
}
