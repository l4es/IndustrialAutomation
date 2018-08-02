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
import java.util.List;
import java.util.ResourceBundle;

import br.org.scadabr.l10n.AbstractLocalizer;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.User;
import br.org.scadabr.util.SerializationHelper;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import java.util.Set;

/**
 * @author Matthew Lohbihler
 */
abstract public class CompoundComponent extends ViewComponent {

    
    private String name;
    private List<CompoundChild> children = new ArrayList<CompoundChild>();

    // Runtime attributes
    private boolean visible;

    abstract protected void initialize();

    abstract public boolean isDisplayImageChart();

    abstract public String getImageChartData(ResourceBundle bundle);

    abstract public String getStaticContent();

    abstract public boolean hasInfo();

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public List<CompoundChild> getChildComponents() {
        return children;
    }

    protected void addChild(String id, String descriptionKey, HtmlComponent htmlComponent) {
        addChildImpl(id, descriptionKey, htmlComponent, null);
    }

    protected void addChild(String id, String descriptionKey, PointComponent pointComponent, Set<DataType> dataTypesOverride) {
        addChildImpl(id, descriptionKey, pointComponent, dataTypesOverride);
    }

    private void addChildImpl(String id, String descriptionKey, ViewComponent viewComponent, Set<DataType> dataTypesOverride) {
        viewComponent.setIndex(getIndex());
        viewComponent.setIdSuffix("-" + id);
        children.add(new CompoundChild(id, new LocalizableMessageImpl(descriptionKey), viewComponent, dataTypesOverride));
    }

    @Override
    public boolean isCompoundComponent() {
        return true;
    }

    @Override
    public boolean isValid() {
        return true;
    }

    @Override
    public boolean isVisible() {
        return visible;
    }

    @Override
    public boolean containsValidVisibleDataPoint(int dataPointId) {
        if (!visible) {
            return false;
        }

        for (CompoundChild child : children) {
            if (child.getViewComponent().containsValidVisibleDataPoint(dataPointId)) {
                return true;
            }
        }

        return false;
    }

    public PointComponent findPointComponent(String viewComponentId) {
        for (CompoundChild child : children) {
            ViewComponent vc = child.getViewComponent();
            if (vc.isPointComponent() && vc.getId().equals(viewComponentId)) {
                return (PointComponent) vc;
            }
        }
        return null;
    }

    @Override
    public void validateDataPoint(User user, boolean makeReadOnly) {
        visible = false;

        // Validate child components
        for (CompoundChild child : children) {
            ViewComponent vc = child.getViewComponent();
            vc.validateDataPoint(user, makeReadOnly);

            // If any child component is visible, this is visible.
            if (vc.isVisible()) {
                visible = true;
            }
        }
    }

    @Override
    public void setIndex(int index) {
        super.setIndex(index);
        // Make sure the child components have the same id.
        for (CompoundChild child : children) {
            child.getViewComponent().setIndex(index);
        }
    }

    public void setDataPoint(String childId, DataPointVO dataPoint) {
        CompoundChild child = getChild(childId);
        if (child != null && child.getViewComponent().isPointComponent()) {
            ((PointComponent) child.getViewComponent()).tsetDataPoint(dataPoint);
        }
    }

    public ViewComponent getChildComponent(String childId) {
        CompoundChild child = getChild(childId);
        if (child == null) {
            return null;
        }
        return child.getViewComponent();
    }

    private CompoundChild getChild(String childId) {
        for (CompoundChild child : children) {
            if (child.getId().equals(childId)) {
                return child;
            }
        }
        return null;
    }

    protected String generateImageChartData(ResourceBundle bundle, long duration, String... childIds) {
        return generateImageChartData(bundle, duration, 500, 250, childIds);
    }

    protected String generateImageChartData(ResourceBundle bundle, long duration, int width, int height,
            String... childIds) {
        long ts = 0;
        for (String childId : childIds) {
            PointComponent comp = (PointComponent) getChild(childId).getViewComponent();
            if (comp.isValid() && comp.isVisible() && comp.tgetDataPoint().lastValue() != null) {
                long cts = comp.tgetDataPoint().lastValue().getTimestamp();
                if (ts < cts) {
                    ts = cts;
                }
            }
        }

        StringBuilder htmlData = new StringBuilder();
        htmlData.append("<img src=\"chart/");
        htmlData.append(ts);
        htmlData.append('_');
        htmlData.append(duration);

        for (String childId : childIds) {
            PointComponent comp = (PointComponent) getChild(childId).getViewComponent();
            if (comp.isValid() && comp.isVisible()) {
                htmlData.append('_');
                htmlData.append(comp.tgetDataPoint().getId());
            }
        }

        htmlData.append("_w");
        htmlData.append(width);
        htmlData.append("_h");
        htmlData.append(height);

        htmlData.append(".png");
        htmlData.append("\" alt=\"").append(AbstractLocalizer.localizeI18nKey("common.imageChart", bundle)).append("\"/>");

        return htmlData.toString();
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
        SerializationHelper.writeSafeUTF(out, name);

        int len = 0;
        for (CompoundChild child : children) {
            if (child.getViewComponent().isPointComponent()) {
                len++;
            }
        }
        out.writeInt(len);

        for (CompoundChild child : children) {
            if (child.getViewComponent().isPointComponent()) {
                out.writeUTF(child.getId());
                writeDataPoint(out, ((PointComponent) child.getViewComponent()).tgetDataPoint());
            }
        }
    }

    private void readObject(ObjectInputStream in) throws IOException {
        int ver = in.readInt();

        children = new ArrayList<>();
        initialize();

        // Switch on the version of the class so that version changes can be elegantly handled.
        if (ver == 1) {
            name = SerializationHelper.readSafeUTF(in);

            int len = in.readInt();
            for (int i = 0; i < len; i++) {
                String childId = in.readUTF();
                DataPointVO dataPoint = readDataPoint(in);
                setDataPoint(childId, dataPoint);
            }
        }
    }

}
