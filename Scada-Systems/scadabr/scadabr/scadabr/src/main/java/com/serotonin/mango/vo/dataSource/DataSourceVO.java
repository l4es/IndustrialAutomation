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

import br.org.scadabr.ScadaBrConstants;
import java.io.Serializable;
import java.util.List;
import java.util.Map;

import br.org.scadabr.ShouldNeverHappenException;

import com.serotonin.mango.rt.dataSource.DataSourceRT;
import com.serotonin.mango.rt.event.type.AuditEventType;
import br.org.scadabr.util.ChangeComparable;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.vo.NodeType;
import br.org.scadabr.vo.VO;
import br.org.scadabr.vo.datasource.UniqueDsXid;
import br.org.scadabr.vo.event.type.DataSourceEventKey;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.serotonin.mango.rt.event.type.DataSourceEventType;
import java.util.Set;
import javax.validation.ValidationException;
import javax.validation.constraints.NotNull;
import javax.validation.constraints.Size;

@UniqueDsXid
abstract public class DataSourceVO<T extends DataSourceVO<T>> implements VO<T>,
        Serializable, Cloneable, ChangeComparable<T> {

    private void fillEventTypeMap() {
        this.eventTypeMap = (Map<DataSourceEventKey, DataSourceEventType>) createEventKeyMap();
        for (DataSourceEventKey key : createEventKeySet()) {
//TODO            eventTypeMap.put(key, new DataSourceEventType(id, key, key.getDefaultAlarmLevel()));
        }
    }

    public static final String XID_PREFIX = "DS_";

    private Map<DataSourceEventKey, DataSourceEventType> eventTypeMap;

    abstract public String getDataSourceTypeKey();
    
    public void setDataSourceTypeKey(String dataSourceTypeKey) {
        if (!getDataSourceTypeKey().equals(dataSourceTypeKey)) {
            throw new ValidationException("dataSourceTypeKey mismatch");
        }
    }

    @JsonIgnore
    public abstract int getDataSourceTypeId();

    abstract public LocalizableMessage getConnectionDescription();

    @Override
    abstract public DataSourceRT<T> createRT();

    public DataSourceVO() {
        fillEventTypeMap();
    }

    @JsonIgnore
    public boolean isNew() {
        return id == null;
    }

    private Integer id;
    @NotNull
    @Size(min = 1, max = 50)
    private String xid;

    @NotNull(message = "validate.nameRequired")
    @Size(min = 1, max = 40)
    private String name = this.getClass().getSimpleName();

    private boolean enabled;

    public boolean isEnabled() {
        return enabled;
    }

    public void setEnabled(boolean enabled) {
        this.enabled = enabled;
    }

    @Override
    public Integer getId() {
        return id;
    }

    @Deprecated
    public void setId(Integer id) {
        this.id = id;
        // replace id with righth id...
        for (DataSourceEventKey key : eventTypeMap.keySet()) {
            DataSourceEventType dsEvt = eventTypeMap.get(key);
            eventTypeMap.put(key, new DataSourceEventType(id, key, dsEvt.getAlarmLevel()));
        }
    }

    public String getXid() {
        return xid;
    }

    public void setXid(String xid) {
        this.xid = xid;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public DataSourceVO<?> copy() {
        try {
            return (DataSourceVO<?>) super.clone();
        } catch (CloneNotSupportedException e) {
            throw new ShouldNeverHappenException(e);
        }
    }

    @JsonIgnore
    @Override
    //TODO is tis everytime an audit event ???
    public String getTypeKey() {
        return "event.audit.dataSource";
    }

    @Override
    public NodeType getNodeType() {
        return NodeType.DATA_SOURCE;
    }


    @Override
    public final void addProperties(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.head.name", name);
        AuditEventType.addPropertyMessage(list, "common.xid", xid);
        AuditEventType.addPropertyMessage(list, "common.enabled", enabled);
//TODO events??
        addPropertiesImpl(list);
    }

    @Override
    public final void addPropertyChanges(List<LocalizableMessage> list, T from) {
        final DataSourceVO fromVO = (DataSourceVO) from;
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.head.name",
                fromVO.name, name);
        AuditEventType.maybeAddPropertyChangeMessage(list, "common.xid",
                fromVO.xid, xid);
        AuditEventType.maybeAddPropertyChangeMessage(list, "common.enabled",
                fromVO.enabled, enabled);

//TODO events??
        addPropertyChangesImpl(list, from);
    }

    abstract protected void addPropertiesImpl(List<LocalizableMessage> list);

    abstract protected void addPropertyChangesImpl(
            List<LocalizableMessage> list, T from);

    /**
     * get all Types for configuration
     *
     * @param <K>
     * @return
     */
    public abstract <K extends DataSourceEventKey> Set<K> createEventKeySet();

    /**
     * Create a optimized map i.e. EnumMap ...
     *
     * @param <K>
     * @return
     */
    public abstract <K extends DataSourceEventKey> Map<K, ?> createEventKeyMap();

    /**
     * Get the type of a Key
     *
     * @param key
     * @return
     */
    public DataSourceEventType getEventType(DataSourceEventKey key) {
        return eventTypeMap.get(key);
    }

}
