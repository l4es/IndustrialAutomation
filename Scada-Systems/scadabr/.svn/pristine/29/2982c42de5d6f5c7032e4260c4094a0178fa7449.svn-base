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
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import br.org.scadabr.db.IntValuePair;
import com.serotonin.mango.rt.dataImage.MultistateValueTime;

import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataImage.types.MultistateValue;
import com.serotonin.mango.view.ImplDefinition;
import java.util.EnumSet;

/**
 * @author Matthew Lohbihler
 */

public class MultistateGraphicComponent extends ImageSetComponent<MultistateValueTime> {

    public static ImplDefinition DEFINITION = new ImplDefinition("multistateGraphic", "MULTISTATE_GRAPHIC",
            "graphic.multistateGraphic", EnumSet.of(DataType.MULTISTATE));

    private Map<Integer, Integer> stateImageMap = new HashMap<>();
    
    private int defaultImage;

    public int getDefaultImage() {
        return defaultImage;
    }

    public void setDefaultImage(int defaultImage) {
        this.defaultImage = defaultImage;
    }

    @Override
    public ImplDefinition definition() {
        return DEFINITION;
    }

    @Override
    public String getImage(MultistateValueTime pointValue) {
        Byte state = null;
        if (pointValue != null) {
            state = pointValue.getByteValue();
        }

        Integer imageId = null;
        if (state != null) {
            imageId = stateImageMap.get(state);
        }

        if (imageId == null) {
            imageId = defaultImage;
        }

        int id = imageId;

        if (id >= 0 && id < imageSet.getImageCount()) {
            return imageSet.getImageFilename(id);
        }

        return null;
    }

    public List<IntValuePair> getImageStateList() {
        List<IntValuePair> result = new ArrayList<>();
        for (Integer state : stateImageMap.keySet()) {
            Integer imageId = stateImageMap.get(state);

            IntValuePair stateList = null;
            for (IntValuePair ivp : result) {
                if (ivp.getKey() == imageId) {
                    stateList = ivp;
                    break;
                }
            }

            if (stateList == null) {
                stateList = new IntValuePair(imageId, state.toString());
                result.add(stateList);
            } else {
                stateList.setValue(stateList.getValue() + ',' + state.toString());
            }
        }
        return result;
    }

    public void setImageStateList(List<IntValuePair> imageStateList) {
        stateImageMap.clear();
        for (IntValuePair ivp : imageStateList) {
            String[] states = ivp.getValue().split(",");
            for (String stateStr : states) {
                try {
                    int state = Integer.parseInt(stateStr.trim());
                    stateImageMap.put(state, ivp.getKey());
                } catch (NumberFormatException e) {
                    // Ignore
                }
            }
        }
    }
/*
    @Override
    public void validate(DwrResponseI18n response) {
        super.validate(response);

        for (Integer index : stateImageMap.values()) {
            if (index < 0) {
                response.addContextual("stateImageMappings", "validate.cannotBeNegative");
            }
        }
        if (defaultImage < 0) {
            response.addContextual("defaultImageIndex", "validate.cannotBeNegative");
        }

        if (imageSet != null) {
            for (Integer index : stateImageMap.values()) {
                if (index >= imageSet.getImageCount()) {
                    response.addContextual("stateImageMappings", "emport.error.component.imageIndex", index, imageSet.getId(), imageSet.getImageCount() - 1);
                }
            }
            if (defaultImage >= imageSet.getImageCount()) {
                response.addContextual("defaultImageIndex", "emport.error.component.imageIndex", defaultImage, imageSet.getId(), imageSet.getImageCount() - 1);
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

        out.writeObject(stateImageMap);
        out.writeInt(defaultImage);
    }

    @SuppressWarnings("unchecked")
    private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            stateImageMap = (Map<Integer, Integer>) in.readObject();
            defaultImage = in.readInt();
        }
    }

}
