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
package com.serotonin.mango.web.mvc.controller;

import br.org.scadabr.DataType;
import java.util.LinkedList;
import java.util.List;

import javax.servlet.http.HttpServletRequest;

import br.org.scadabr.ShouldNeverHappenException;
import com.serotonin.mango.Common;
import com.serotonin.mango.db.dao.DataPointDao;
import com.serotonin.mango.db.dao.DataSourceDao;
import com.serotonin.mango.util.CommPortConfigException;
import com.serotonin.mango.vo.DataPointExtendedNameComparator;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.User;
import com.serotonin.mango.vo.dataSource.DataSourceVO;
import com.serotonin.mango.vo.permission.Permissions;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

@Controller
@RequestMapping("/data_source_edit.shtm")
public class DataSourceEditController {

    //TODO split the methods by param ...
    //   @Override
    @RequestMapping(method = RequestMethod.GET)
    public String showForm(ModelMap modelMap, HttpServletRequest request) {
        DataSourceVO<?> dataSourceVO = null;
        User user = Common.getUser(request);

        // Get the id.
        int id = Common.NEW_ID;
        String idStr = request.getParameter("dsid");
        if (idStr == null) {
            // Check for a data point id
            String pidStr = request.getParameter("pid");
            if (pidStr == null) {
                // Adding a new data source? Get the type id.
                int typeId = Integer.parseInt(request.getParameter("typeId"));

                Permissions.ensureAdmin(user);

                // A new data source
                dataSourceVO = DataSourceVO.createDataSourceVO(typeId);
                dataSourceVO.setId(Common.NEW_ID);
                dataSourceVO.setXid(DataSourceDao.getInstance().generateUniqueXid());
            } else {
                int pid = Integer.parseInt(pidStr);
                DataPointVO dp = DataPointDao.getInstance().getDataPoint(pid);
                if (dp == null) {
                    throw new ShouldNeverHappenException("DataPoint not found with id " + pid);
                }
                id = dp.getDataSourceId();
            }
        } else // An existing configuration.
        {
            id = Integer.parseInt(idStr);
        }

        if (id != Common.NEW_ID) {
            dataSourceVO = Common.ctx.getRuntimeManager().getDataSource(id);
            if (dataSourceVO == null) {
                throw new ShouldNeverHappenException("DataSource not found with id " + id);
            }
            Permissions.ensureDataSourcePermission(user, id);
        }

        // Set the id of the data source in the user object for the DWR.
        user.setEditDataSource(dataSourceVO);

        // The data source
        modelMap.addAttribute("dataSource", dataSourceVO);

        // Reference data
        try {
            modelMap.addAttribute("commPorts", Common.getCommPorts());
        } catch (CommPortConfigException e) {
            modelMap.addAttribute("commPortError", e.getMessage());
        }

        List<DataPointVO> allPoints = DataPointDao.getInstance().getDataPoints(DataPointExtendedNameComparator.instance, false);
        List<DataPointVO> userPoints = new LinkedList<>();
        List<DataPointVO> analogPoints = new LinkedList<>();
        for (DataPointVO dp : allPoints) {
            if (Permissions.hasDataPointReadPermission(user, dp)) {
                userPoints.add(dp);
                if (dp.getDataType() == DataType.NUMERIC) {
                    analogPoints.add(dp);
                }
            }
        }
        modelMap.addAttribute("userPoints", userPoints);
        modelMap.addAttribute("analogPoints", analogPoints);

        return "dataSourceEdit";
    }
}
