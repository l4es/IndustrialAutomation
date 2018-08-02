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

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

import br.org.scadabr.view.component.AlarmListComponent;
import br.org.scadabr.view.component.ButtonComponent;
import br.org.scadabr.view.component.ChartComparatorComponent;
import br.org.scadabr.view.component.FlexBuilderComponent;
import br.org.scadabr.view.component.LinkComponent;
import br.org.scadabr.view.component.ScriptButtonComponent;

import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.dao.DataPointDao;


import com.serotonin.mango.view.ImplDefinition;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.User;
import br.org.scadabr.util.SerializationHelper;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;

/**
 * @author Matthew Lohbihler
 */
@Configurable
abstract public class ViewComponent implements Serializable {
    /*
public class ViewComponenetValidator {
        public void validate(DwrResponseI18n response) {
        if (x < 0) {
            response.addContextual("x", "validate.cannotBeNegative");
        }
        if (y < 0) {
            response.addContextual("y", "validate.cannotBeNegative");
        }
    }


}
*/
    @Autowired
    private DataPointDao dataPointDao;

    private static List<ImplDefinition> DEFINITIONS;

    public static List<ImplDefinition> getImplementations() {
        if (DEFINITIONS == null) {
            List<ImplDefinition> d = new ArrayList<>();
            d.add(AnalogGraphicComponent.DEFINITION);
            d.add(BinaryGraphicComponent.DEFINITION);
            d.add(DynamicGraphicComponent.DEFINITION);
            d.add(HtmlComponent.DEFINITION);
            d.add(MultistateGraphicComponent.DEFINITION);
            d.add(ScriptComponent.DEFINITION);
            d.add(SimpleImageComponent.DEFINITION);
            d.add(SimplePointComponent.DEFINITION);
            d.add(ThumbnailComponent.DEFINITION);
            d.add(SimpleCompoundComponent.DEFINITION);
            d.add(ImageChartComponent.DEFINITION);
            d.add(WirelessTempHumSensor.DEFINITION);
            d.add(ButtonComponent.DEFINITION);
            d.add(LinkComponent.DEFINITION);
            d.add(AlarmListComponent.DEFINITION);
            d.add(ScriptButtonComponent.DEFINITION);
            // d.add(FlexBuilderComponent.DEFINITION);
            d.add(ChartComparatorComponent.DEFINITION);
            DEFINITIONS = d;
        }
        return DEFINITIONS;
    }

    public static ViewComponent newInstance(String name) {
        ImplDefinition def = ImplDefinition.findByName(getImplementations(),
                name);
        try {
            return resolveClass(def).newInstance();
        } catch (Exception e) {
            throw new ShouldNeverHappenException(
                    "Error finding component with name '" + name + "': "
                    + e.getMessage());
        }
    }

    static Class<? extends ViewComponent> resolveClass(ImplDefinition def) {
        if (def == AnalogGraphicComponent.DEFINITION) {
            return AnalogGraphicComponent.class;
        }
        if (def == BinaryGraphicComponent.DEFINITION) {
            return BinaryGraphicComponent.class;
        }
        if (def == DynamicGraphicComponent.DEFINITION) {
            return DynamicGraphicComponent.class;
        }
        if (def == HtmlComponent.DEFINITION) {
            return HtmlComponent.class;
        }
        if (def == MultistateGraphicComponent.DEFINITION) {
            return MultistateGraphicComponent.class;
        }
        if (def == ScriptComponent.DEFINITION) {
            return ScriptComponent.class;
        }
        if (def == SimpleImageComponent.DEFINITION) {
            return SimpleImageComponent.class;
        }
        if (def == SimplePointComponent.DEFINITION) {
            return SimplePointComponent.class;
        }
        if (def == ThumbnailComponent.DEFINITION) {
            return ThumbnailComponent.class;
        }
        if (def == SimpleCompoundComponent.DEFINITION) {
            return SimpleCompoundComponent.class;
        }
        if (def == ImageChartComponent.DEFINITION) {
            return ImageChartComponent.class;
        }
        if (def == WirelessTempHumSensor.DEFINITION) {
            return WirelessTempHumSensor.class;
        }
        if (def == ButtonComponent.DEFINITION) {
            return ButtonComponent.class;
        }
        if (def == LinkComponent.DEFINITION) {
            return LinkComponent.class;
        }
        if (def == AlarmListComponent.DEFINITION) {
            return AlarmListComponent.class;
        }
        if (def == ScriptButtonComponent.DEFINITION) {
            return ScriptButtonComponent.class;
        }
        if (def == FlexBuilderComponent.DEFINITION) {
            return FlexBuilderComponent.class;
        }
        if (def == ChartComparatorComponent.DEFINITION) {
            return ChartComparatorComponent.class;
        }
        return null;
    }

    public static List<String> getExportTypes() {
        List<ImplDefinition> definitions = getImplementations();
        List<String> result = new ArrayList<>(definitions.size());
        for (ImplDefinition def : definitions) {
            result.add(def.getExportName());
        }
        return result;
    }

    private int index;
    private String idSuffix;
    private String style;
    
    private int x;
    
    private int y;

    public void setLocation(int x, int y) {
        this.x = x;
        this.y = y;
    }

    abstract public ImplDefinition definition();

    abstract public void validateDataPoint(User user, boolean makeReadOnly);

    abstract public boolean isVisible();

    abstract public boolean isValid();

    abstract public boolean containsValidVisibleDataPoint(int dataPointId);

    public boolean isPointComponent() {
        return false;
    }

    public boolean isCompoundComponent() {
        return false;
    }

    public boolean isCustomComponent() {
        return false;
    }

    public String getDefName() {
        return definition().getName();
    }

    public String getId() {
        if (idSuffix == null) {
            return Integer.toString(index);
        }
        return Integer.toString(index) + idSuffix;
    }

    public int getIndex() {
        return index;
    }

    public void setIndex(int index) {
        this.index = index;
    }

    public String getIdSuffix() {
        return idSuffix;
    }

    public void setIdSuffix(String idSuffix) {
        this.idSuffix = idSuffix;
    }

    public int getX() {
        return x;
    }

    public void setX(int x) {
        this.x = x;
    }

    public int getY() {
        return y;
    }

    public void setY(int y) {
        this.y = y;
    }

    public String getStyle() {
        if (style != null) {
            return style;
        }

        StringBuilder sb = new StringBuilder();
        sb.append("position:absolute;");
        sb.append("left:").append(x).append("px;");
        sb.append("top:").append(y).append("px;");
        return sb.toString();
    }

    public void setStyle(String style) {
        this.style = style;
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
        out.writeInt(index);
        SerializationHelper.writeSafeUTF(out, idSuffix);
        out.writeInt(x);
        out.writeInt(y);
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be
        // elegantly handled.
        if (ver == 1) {
            index = in.readInt();
            idSuffix = SerializationHelper.readSafeUTF(in);
            x = in.readInt();
            y = in.readInt();
        }
    }

    protected void writeDataPoint(ObjectOutputStream out, DataPointVO dataPoint)
            throws IOException {
        if (dataPoint == null) {
            out.writeInt(0);
        } else {
            out.writeInt(dataPoint.getId());
        }
    }

    protected DataPointVO readDataPoint(ObjectInputStream in)
            throws IOException {
        return dataPointDao.getDataPoint(in.readInt());
    }

}
