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
package com.serotonin.mango.view;

import br.org.scadabr.ScadaBrConstants;
import br.org.scadabr.dao.UserDao;
import br.org.scadabr.dao.ViewDao;
import br.org.scadabr.utils.ImplementMeException;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;

import com.serotonin.mango.view.component.CompoundComponent;
import com.serotonin.mango.view.component.PointComponent;
import com.serotonin.mango.view.component.ViewComponent;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.User;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;
import org.springframework.validation.Errors;
import org.springframework.validation.Validator;

@Configurable
public class View implements Serializable {

    
    @Configurable
public static class ViewValidator implements Validator {
    @Autowired
    private ViewDao viewDao;

    @Override
    public boolean supports(Class<?> clazz) {
        return View.class.isAssignableFrom(clazz);
    }

    @Override
    public void validate(Object target, Errors errors) {
        final View vo = (View) target;
        if (vo.name.isEmpty()) {
            errors.rejectValue("name",  "validate.required");
        } else if (vo.name.length() > 100) {
            errors.rejectValue("name", "validate.notLongerThan", new Object[]{100}, "validate.notLongerThan");
        }

        if (vo.xid.isEmpty()) {
            errors.rejectValue("xid", "validate.required");
        } else if (vo.xid.length() >  50) {
            errors.rejectValue("xid", "validate.notLongerThan", new Object[]{50}, "validate.notLongerThan");
        } else if (!viewDao.isXidUnique(vo.xid, vo.id)) {
            errors.rejectValue("xid", "validate.xidUsed");
        }

        throw new ImplementMeException();
        /*
         for (ViewComponent vc : vo.viewComponents) {
            vc.validate(response);
        }
        */
    }


}

    public static final String XID_PREFIX = "GV_";

    @Autowired
    private UserDao userDao;

    private int id = ScadaBrConstants.NEW_ID;

    private String xid;

    private String name;

    private String backgroundFilename;
    private int userId;
    private List<ViewComponent> viewComponents = new CopyOnWriteArrayList<>();
    private int anonymousAccess = ShareUser.ACCESS_NONE;
    private List<ShareUser> viewUsers = new CopyOnWriteArrayList<>();

    public void addViewComponent(ViewComponent viewComponent) {
        // Determine an index for the component.
        int min = 0;
        for (ViewComponent vc : viewComponents) {
            if (min < vc.getIndex()) {
                min = vc.getIndex();
            }
        }
        viewComponent.setIndex(min + 1);

        viewComponents.add(viewComponent);
    }

    public ViewComponent getViewComponent(int index) {
        for (ViewComponent vc : viewComponents) {
            if (vc.getIndex() == index) {
                return vc;
            }
        }
        return null;
    }

    public void removeViewComponent(ViewComponent vc) {
        if (vc != null) {
            viewComponents.remove(vc);
        }
    }

    public boolean isNew() {
        return id == ScadaBrConstants.NEW_ID;
    }

    public boolean containsValidVisibleDataPoint(int dataPointId) {
        for (ViewComponent vc : viewComponents) {
            if (vc.containsValidVisibleDataPoint(dataPointId)) {
                return true;
            }
        }
        return false;
    }

    public DataPointVO findDataPoint(String viewComponentId) {
        for (ViewComponent vc : viewComponents) {
            if (vc.isPointComponent()) {
                if (vc.getId().equals(viewComponentId)) {
                    return ((PointComponent) vc).tgetDataPoint();
                }
            } else if (vc.isCompoundComponent()) {
                PointComponent pc = ((CompoundComponent) vc)
                        .findPointComponent(viewComponentId);
                if (pc != null) {
                    return pc.tgetDataPoint();
                }
            }
        }
        return null;
    }

    public int getUserAccess(User user) {
        if (user == null) {
            return anonymousAccess;
        }

        if (userId == user.getId()) {
            return ShareUser.ACCESS_OWNER;
        }

        for (ShareUser vu : viewUsers) {
            if (vu.getUserId() == user.getId()) {
                return vu.getAccessType();
            }
        }
        return ShareUser.ACCESS_NONE;
    }

    /**
     * This method is used before the view is displayed in order to validate: -
     * that the given user is allowed to access points that back any components
     * - that the points that back components still have valid data types for
     * the components that render them
     *
     * @param makeReadOnly
     */
    public void validateViewComponents(boolean makeReadOnly) {
        User owner = userDao.getUser(userId);
        for (ViewComponent viewComponent : viewComponents) {
            viewComponent.validateDataPoint(owner, makeReadOnly);
        }
    }

    public String getBackgroundFilename() {
        return backgroundFilename;
    }

    public void setBackgroundFilename(String backgroundFilename) {
        this.backgroundFilename = backgroundFilename;
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
        this.name = name;
    }

    public List<ViewComponent> getViewComponents() {
        return viewComponents;
    }

    public int getAnonymousAccess() {
        return anonymousAccess;
    }

    public void setAnonymousAccess(int anonymousAccess) {
        this.anonymousAccess = anonymousAccess;
    }

    public int getUserId() {
        return userId;
    }

    public void setUserId(int userId) {
        this.userId = userId;
    }

    public List<ShareUser> getViewUsers() {
        return viewUsers;
    }

    public void setViewUsers(List<ShareUser> viewUsers) {
        this.viewUsers = viewUsers;
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
        out.writeObject(viewComponents);
    }

    @SuppressWarnings("unchecked")
    private void readObject(ObjectInputStream in) throws IOException,
            ClassNotFoundException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be
        // elegantly handled.
        if (ver == 1) {
            viewComponents = new CopyOnWriteArrayList<>(
                    (List<ViewComponent>) in.readObject());
        }
    }

}
