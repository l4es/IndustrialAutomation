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

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.springframework.web.servlet.ModelAndView;
import org.springframework.web.servlet.mvc.ParameterizableViewController;

import br.org.scadabr.db.IntValuePair;
import com.serotonin.mango.Common;
import com.serotonin.mango.db.dao.ViewDao;
import com.serotonin.mango.view.ShareUser;
import com.serotonin.mango.view.View;
import com.serotonin.mango.vo.User;
import com.serotonin.mango.vo.permission.Permissions;
import javax.inject.Inject;
import org.springframework.stereotype.Controller;

@Controller
public class ViewsController extends ParameterizableViewController {

    @Inject
    private ViewDao viewDao;
    
    @Override
    protected ModelAndView handleRequestInternal(HttpServletRequest request, HttpServletResponse response)
            throws Exception {
        Map<String, Object> model = new HashMap<>();
        User user = Common.getUser(request);

        List<IntValuePair> views = viewDao.getViewNames(user.getId());
        model.put("views", views);

        // Set the current view.
        View currentView = null;
        String vid = request.getParameter("viewId");
        try {
            currentView = viewDao.getView(Integer.parseInt(vid));
        } catch (NumberFormatException e) {
            // no op
        }

        if (currentView == null && views.size() > 0) {
            currentView = viewDao.getView(views.get(0).getKey());
        }

        if (currentView != null) {
            Permissions.ensureViewPermission(user, currentView);

            // Make sure the owner still has permission to all of the points in the view, and that components are
            // otherwise valid.
            currentView.validateViewComponents(false);

            // Add the view to the session for the dwr access stuff.
            model.put("currentView", currentView);
            model.put("owner", currentView.getUserAccess(user) == ShareUser.ACCESS_OWNER);
            user.setView(currentView);
        }

        return new ModelAndView(getViewName(), model);
    }
}
