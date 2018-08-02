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

import br.org.scadabr.DataType;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.script.ScriptException;

import br.org.scadabr.db.IntValuePair;
import com.serotonin.mango.Common;
import com.serotonin.mango.db.dao.DataPointDao;
import com.serotonin.mango.db.dao.PointLinkDao;
import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.IDataPoint;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.rt.dataSource.meta.ResultTypeException;
import com.serotonin.mango.rt.dataSource.meta.ScriptExecutor;
import com.serotonin.mango.rt.link.PointLinkRT;
import com.serotonin.mango.vo.DataPointExtendedNameComparator;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.User;
import com.serotonin.mango.vo.link.PointLinkVO;
import com.serotonin.mango.vo.permission.Permissions;
import br.org.scadabr.web.dwr.DwrResponseI18n;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import com.serotonin.mango.db.dao.EventDao;
import com.serotonin.mango.db.dao.UserDao;
import com.serotonin.mango.rt.RuntimeManager;
import java.util.Date;
import javax.inject.Inject;
import org.springframework.beans.factory.annotation.Autowired;

/**
 * @author Matthew Lohbihler
 */
public class PointLinksDwr extends BaseDwr {

    @Inject
    private PointLinkDao pointLinkDao;
    @Autowired
    private RuntimeManager runtimeManager;

    public Map<String, Object> init() {
        User user = Common.getUser();
        Map<String, Object> data = new HashMap<>();

        // Get the points that this user can access.
        List<DataPointVO> allPoints = dataPointDao.getDataPoints(DataPointExtendedNameComparator.instance, false);
        List<IntValuePair> sourcePoints = new ArrayList<>();
        List<IntValuePair> targetPoints = new ArrayList<>();
        for (DataPointVO point : allPoints) {
            if (Permissions.hasDataPointReadPermission(user, point)) {
                sourcePoints.add(new IntValuePair(point.getId(), point.getExtendedName()));
            }
            if (point.getPointLocator().isSettable() && Permissions.hasDataPointSetPermission(user, point)) {
                targetPoints.add(new IntValuePair(point.getId(), point.getExtendedName()));
            }
        }

        data.put("sourcePoints", sourcePoints);
        data.put("targetPoints", targetPoints);

        // Get the existing point links.
        List<PointLinkVO> pointLinks = new ArrayList<>();
        for (PointLinkVO pointLink : pointLinkDao.getPointLinks()) {
            if (containsPoint(sourcePoints, pointLink.getSourcePointId())
                    && containsPoint(targetPoints, pointLink.getTargetPointId())) {
                pointLinks.add(pointLink);
            }
        }

        data.put("pointLinks", pointLinks);

        return data;
    }

    private boolean containsPoint(List<IntValuePair> pointList, int pointId) {
        for (IntValuePair ivp : pointList) {
            if (ivp.getKey() == pointId) {
                return true;
            }
        }
        return false;
    }

    public PointLinkVO getPointLink(int id) {
        PointLinkVO vo;
        if (id == Common.NEW_ID) {
            vo = new PointLinkVO();
            vo.setXid(pointLinkDao.generateUniqueXid());
        } else {
            vo = pointLinkDao.getPointLink(id);
        }
        return vo;
    }

    public DwrResponseI18n savePointLink(int id, String xid, int sourcePointId, int targetPointId, String script,
            int event, boolean disabled) {
        // Validate the given information. If there is a problem, return an appropriate error message.
        PointLinkVO vo = new PointLinkVO();
        vo.setId(id);
        vo.setXid(xid);
        vo.setSourcePointId(sourcePointId);
        vo.setTargetPointId(targetPointId);
        vo.setScript(script);
        vo.setEvent(event);
        vo.setDisabled(disabled);

        DwrResponseI18n response = new DwrResponseI18n();

        if (xid.isEmpty()) {
            response.addContextual("xid", "validate.required");
        } else if (!pointLinkDao.isXidUnique(xid, id)) {
            response.addContextual("xid", "validate.xidUsed");
        }

        vo.validate(response);

        // Save it
        if (response.isEmpty()) {
            runtimeManager.savePointLink(vo);
        }

        response.addData("plId", vo.getId());

        return response;
    }

    public void deletePointLink(int id) {
        runtimeManager.deletePointLink(id);
    }

    public DwrResponseI18n validateScript(String script, int sourcePointId, int targetPointId) {
        DwrResponseI18n response = new DwrResponseI18n();
        LocalizableMessage message;
        ScriptExecutor scriptExecutor = new ScriptExecutor();

        DataPointRT point = runtimeManager.getDataPoint(sourcePointId);
        if (point == null) {
            message = new LocalizableMessageImpl("event.pointLink.sourceUnavailable");
        } else {
            Map<String, IDataPoint> context = new HashMap<>();
            context.put(PointLinkRT.CONTEXT_VAR_NAME, point);
            final DataType targetDataType = dataPointDao.getDataPoint(targetPointId).getDataType();

            try {
                PointValueTime pvt = scriptExecutor.execute(script, context, System.currentTimeMillis(),
                        targetDataType, -1);
                if (pvt.getValue() == null) {
                    message = new LocalizableMessageImpl("event.pointLink.nullResult");
                } else if (pvt.getTime() == -1) {
                    message = new LocalizableMessageImpl("pointLinks.validate.success", pvt.getValue());
                } else {
                    message = new LocalizableMessageImpl("pointLinks.validate.successTs", pvt.getValue(), new Date(pvt.getTime()));
                }
            } catch (ScriptException e) {
                message = new LocalizableMessageImpl("common.default", e.getMessage());
            } catch (ResultTypeException e) {
                message = e;
            }
        }

        response.addContextual("script", message.getI18nKey(), message.getArgs());
        return response;
    }

    /**
     * @return the pointLinkDao
     */
    public PointLinkDao getPointLinkDao() {
        return pointLinkDao;
    }

    /**
     * @param pointLinkDao the pointLinkDao to set
     */
    public void setPointLinkDao(PointLinkDao pointLinkDao) {
        this.pointLinkDao = pointLinkDao;
    }

}
