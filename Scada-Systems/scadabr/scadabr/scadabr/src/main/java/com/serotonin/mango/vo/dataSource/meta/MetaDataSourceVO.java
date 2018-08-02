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
package com.serotonin.mango.vo.dataSource.meta;

import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import br.org.scadabr.vo.datasource.meta.MetaDataSourceEventKey;
import br.org.scadabr.vo.datasource.meta.MetaDataSourceType;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonTypeName;
import com.serotonin.mango.rt.dataSource.meta.MetaDataSourceRT;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import java.util.EnumMap;
import java.util.EnumSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * @author Matthew Lohbihler
 */
@JsonTypeName("DATA_SOURCE.MetaDataSource")
public class MetaDataSourceVO extends DataSourceVO<MetaDataSourceVO> {

    @Override
    public MetaDataSourceRT createRT() {
        return new MetaDataSourceRT(this);
    }

    @Override
    public String getDataSourceTypeKey() {
        return MetaDataSourceType.KEY;
    }

    @Override
    public int getDataSourceTypeId() {
        return MetaDataSourceType.DB_ID;
    }

    @Override
    protected Object clone() throws CloneNotSupportedException {
        return super.clone();
    }

    @JsonIgnore
    @Override
    public LocalizableMessage getConnectionDescription() {
        return new LocalizableMessageImpl("common.noMessage");
    }

    @Override
    protected void addPropertiesImpl(List<LocalizableMessage> list) {
        // no op
    }

    @Override
    protected void addPropertyChangesImpl(List<LocalizableMessage> list, MetaDataSourceVO from) {
        // no op
    }

    @Override
    public Set<MetaDataSourceEventKey> createEventKeySet() {
        return EnumSet.allOf(MetaDataSourceEventKey.class);
    }

    @Override
    public Map<MetaDataSourceEventKey, ?> createEventKeyMap() {
        return new EnumMap(MetaDataSourceEventKey.class);
    }

}
