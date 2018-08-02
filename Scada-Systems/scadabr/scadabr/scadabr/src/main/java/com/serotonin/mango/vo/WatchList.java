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
package com.serotonin.mango.vo;

import br.org.scadabr.dao.DataPointDao;
import br.org.scadabr.dao.WatchListDao;
import br.org.scadabr.rt.UserRT;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.view.SharedUserAcess;
import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;

import com.fasterxml.jackson.annotation.JsonIgnore;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;
import org.springframework.validation.Errors;
import org.springframework.validation.Validator;

/**
 * @author Matthew Lohbihler
 */
@Deprecated //TODO use WatchListRT|VO
public class WatchList implements Iterable<DataPointVO> {

    @Configurable
    public static class WatchListValidator implements Validator {

        @Autowired
        private DataPointDao dataPointDao;
        @Autowired
        private WatchListDao watchListDao;

        @Override
        public boolean supports(Class<?> clazz) {
            return WatchList.class.isAssignableFrom(clazz);
        }

        @Override
        public void validate(Object target, Errors errors) {
            final WatchList vo = (WatchList) target;
            if (vo.name.isEmpty()) {
                errors.rejectValue("name", "validate.required");
            } else if (vo.name.length() > 50) {
                errors.rejectValue("name", "validate.notLongerThan", new Object[]{50}, "validate.notLongerThan");
            }

            if (vo.xid.isEmpty()) {
                errors.rejectValue("xid", "validate.required");
            } else if (vo.xid.length() > 50) {
                errors.rejectValue("xid", "validate.notLongerThan", new Object[]{50}, "validate.notLongerThan");
            } else if (!watchListDao.isXidUnique(vo.xid, vo.id)) {
                errors.rejectValue("xid", "validate.xidUsed");
            }

            throw new ImplementMeException();
            /*
             for (DataPointVO dpVO : vo.pointList) {
             dpVO.validate(response);
             }
             */
        }

    }

    public static final String XID_PREFIX = "WL_";

    private Integer id;
    private String xid;
    private Integer userId;

    private String name;
    private final List<DataPointVO> pointList = new CopyOnWriteArrayList<>();
    private Map<Integer, SharedUserAcess> watchListUsers = new HashMap<>();

    public SharedUserAcess getUserAccess(UserRT user) {
        if (user.getId() == userId) {
            return SharedUserAcess.OWNER;
        }

        
        SharedUserAcess wlu = watchListUsers.get(user.getId());
        if (wlu != null) {
            return wlu;
        } else {
            return SharedUserAcess.NONE;
        }
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
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
        if (name == null) {
            this.name = "";
        } else {
            this.name = name;
        }
    }

    public List<DataPointVO> getPointList() {
        return pointList;
    }

    public Integer getUserId() {
        return userId;
    }

    public void setUserId(Integer userId) {
        this.userId = userId;
    }

    @Override
    public Iterator<DataPointVO> iterator() {
        return pointList.iterator();
    }

    @JsonIgnore
    public boolean isNew() {
        return id == null;
    }
}
