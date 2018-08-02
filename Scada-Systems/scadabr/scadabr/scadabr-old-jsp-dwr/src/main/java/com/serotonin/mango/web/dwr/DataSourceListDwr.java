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
package com.serotonin.mango.web.dwr;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import br.org.scadabr.db.IntValuePair;
import com.serotonin.mango.Common;
import com.serotonin.mango.db.dao.DataSourceDao;
import com.serotonin.mango.db.dao.SystemSettingsDao;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.permission.Permissions;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import javax.inject.Inject;

/**
 * @author Matthew Lohbihler
 */
public class DataSourceListDwr extends BaseDwr {
    
    @Inject
    private DataSourceDao dataSourceDao;
@Inject
private SystemSettingsDao systemSettingsDao;

    public DwrResponseI18n init() {
        DwrResponseI18n response = new DwrResponseI18n();

        if (Common.getUser().isAdmin()) {
            List<IntValuePair> translatedTypes = new ArrayList<IntValuePair>();
            for (DataSourceVO.Type type : DataSourceVO.Type.values()) {
                // Allow customization settings to overwrite the default display value.
                boolean display = systemSettingsDao.getBooleanValue(type.name()
                        + SystemSettingsDao.DATASOURCE_DISPLAY_SUFFIX, type.isDisplay());
                if (display) {
                    translatedTypes.add(new IntValuePair(type.getId(), getMessage(type.getKey())));
                }
            }
            response.addData("types", translatedTypes);
        }

        return response;
    }

    public Map<String, Object> toggleDataSource(int dataSourceId) {
        Permissions.ensureDataSourcePermission(Common.getUser(), dataSourceId);

        DataSourceVO<?> dataSource = runtimeManager.getDataSource(dataSourceId);
        Map<String, Object> result = new HashMap<>();

        dataSource.setEnabled(!dataSource.isEnabled());
        runtimeManager.saveDataSource(dataSource);

        result.put("enabled", dataSource.isEnabled());
        result.put("id", dataSourceId);
        return result;
    }

    public int deleteDataSource(int dataSourceId) {
        Permissions.ensureDataSourcePermission(Common.getUser(), dataSourceId);
        runtimeManager.deleteDataSource(dataSourceId);
        return dataSourceId;
    }

    public DwrResponseI18n toggleDataPoint(int dataPointId) {
        DataPointVO dataPoint = dataPointDao.getDataPoint(dataPointId);
        Permissions.ensureDataSourcePermission(Common.getUser(), dataPoint.getDataSourceId());

        dataPoint.setEnabled(!dataPoint.isEnabled());
        runtimeManager.saveDataPoint(dataPoint);

        DwrResponseI18n response = new DwrResponseI18n();
        response.addData("id", dataPointId);
        response.addData("enabled", dataPoint.isEnabled());
        return response;
    }

    public int copyDataSource(int dataSourceId) {
        Permissions.ensureDataSourcePermission(Common.getUser(), dataSourceId);
        int dsId = dataSourceDao.copyDataSource(dataSourceId, getResourceBundle());
        userDao.populateUserPermissions(Common.getUser());
        return dsId;
    }

    /**
     * @return the dataSourceDao
     */
    public DataSourceDao getDataSourceDao() {
        return dataSourceDao;
    }

    /**
     * @param dataSourceDao the dataSourceDao to set
     */
    public void setDataSourceDao(DataSourceDao dataSourceDao) {
        this.dataSourceDao = dataSourceDao;
    }

}
