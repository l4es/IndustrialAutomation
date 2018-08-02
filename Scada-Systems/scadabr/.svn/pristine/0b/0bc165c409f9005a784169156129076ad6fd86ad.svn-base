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
package br.org.scadabr.vo.datasource;

import br.org.scadabr.DataType;
import java.io.Serializable;

import com.serotonin.mango.rt.dataSource.PointLocatorRT;
import com.serotonin.mango.util.ChangeComparableObject;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.vo.dataSource.DataPointSaveHandler;
import javax.validation.constraints.NotNull;
import javax.validation.constraints.Size;

public interface PointLocatorVO<T extends PointValueTime> extends Serializable, ChangeComparableObject<PointLocatorVO<T>> {

    public static final String XID_PREFIX = "PL_";

    boolean isNew();
    
    /**
     * One of the com.serotonin.mango.DataType
     *
     * @return
     */
    DataType getDataType();

    /**
     * An arbitrary description of the point location configuration for human
     * consumption.
     *
     * @return
     */
    LocalizableMessage getConfigurationDescription();

    /**
     * Can the value be set in the data source?
     *
     * @return
     */
    boolean isSettable();

    /**
     * Supplemental to being settable, can the set value be relinquished?
     *
     * @return
     */
    boolean isRelinquishable();

    /**
     * Create a runtime version of the locator
     *
     * @return
     */
    PointLocatorRT createRuntime();

    DataPointSaveHandler getDataPointSaveHandler();

    @NotNull(message = "{validate.nameRequired}")
    @Size(min = 1, max = 40)
    String getName();

    void setName(String name);

    Integer getId();
    
    void setId(Integer id);
    
    boolean isEnabled();

    void setEnabled(boolean value);

    Integer getDataSourceId();
    
    void setDataSourceId(Integer dataSourceId);

    Integer getPointLocatorFolderId();
    
    void setPointLocatorFolderId(Integer pointLocatorFolderId);
    
}
