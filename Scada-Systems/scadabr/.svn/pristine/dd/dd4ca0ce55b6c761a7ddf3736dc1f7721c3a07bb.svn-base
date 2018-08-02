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
package com.serotonin.mango.vo.dataSource;

import br.org.scadabr.DataType;
import br.org.scadabr.ShouldNeverHappenException;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.vo.datasource.PointLocatorVO;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.event.type.AuditEventType;
import java.util.List;
import javax.validation.constraints.NotNull;
import javax.validation.constraints.Size;

abstract public class AbstractPointLocatorVO<T extends PointValueTime> implements PointLocatorVO<T> {

    private Integer id;
    private String xid;
    private Integer pointLocatorFolderId;
    private boolean enabled;
    @NotNull
    @Size(min = 1, max = 40)
    private String name;
    private DataType dataType;
    private Integer dataSourceId;
    
    public AbstractPointLocatorVO() {
    }
    
    
    public AbstractPointLocatorVO(DataType dataType) {
        this.name = getClass().getSimpleName();
        this.dataType = dataType;
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.name", name);
        AuditEventType.addPropertyMessage(list, "dsEdit.pointDataType", getDataType());
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, PointLocatorVO<T> o) {
        if (id != o.getId()) {
            throw new ShouldNeverHappenException("Id mismatch! Not the same point locator???");
        }
        final AbstractPointLocatorVO<T> from = (AbstractPointLocatorVO<T>) o;
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.name", from.name, name);
        // This below should never happen ...
        if (from.dataType != dataType) {
            throw new ShouldNeverHappenException("DataTypes mismatch");
        }
    }

    /**
     * Defaults to returning null. Override to return something else.
     *
     * @return
     */
    @Override
    public DataPointSaveHandler getDataPointSaveHandler() {
        return null;
    }

    /**
     * Defaults to returning false. Override to return something else.
     */
    @Override
    public boolean isRelinquishable() {
        return false;
    }

    @Override
    public Integer getId() {
        return id;
    }
    
    @Override
    public void setId(Integer id) {
        this.id = id;
    }
    
    @Override
    public boolean isNew() {
        return id == null;
    }

    @Override
    public boolean isEnabled() {
        return enabled;
    }

    @Override
    public void setEnabled(boolean enabled) {
        this.enabled = enabled;
    }

    /**
     * @return the name
     */
    @Override
    public String getName() {
        return name;
    }

    /**
     * @param name the name to set
     */
    public void setName(String name) {
        this.name = name;
    }
    
    @Override
    public final DataType getDataType() {
        return dataType;
    }

    /**
     * @return the dataSourceId
     */
    @Override
    public Integer getDataSourceId() {
        return dataSourceId;
    }

    /**
     * @param dataSourceId the dataSourceId to set
     */
    @Override
    public void setDataSourceId(Integer dataSourceId) {
        this.dataSourceId = dataSourceId;
    }

    /**
     * @return the pointLocatorFolderId
     */
    @Override
    public Integer getPointLocatorFolderId() {
        return pointLocatorFolderId;
    }

    /**
     * @param pointLocatorFolderId the pointLocatorFolderId to set
     */
    @Override
    public void setPointLocatorFolderId(Integer pointLocatorFolderId) {
        this.pointLocatorFolderId = pointLocatorFolderId;
    }

    /**
     * @return the xid
     */
    public String getXid() {
        return xid;
    }

    /**
     * @param xid the xid to set
     */
    public void setXid(String xid) {
        this.xid = xid;
    }

}
