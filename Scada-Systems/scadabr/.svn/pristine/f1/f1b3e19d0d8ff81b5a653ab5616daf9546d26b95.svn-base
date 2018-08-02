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
package br.org.scadabr.web.mvc.controller;

import br.org.scadabr.dao.WatchListDao;
import java.util.ArrayList;
import java.util.List;

import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.WatchList;
import com.serotonin.mango.vo.permission.Permissions;
import br.org.scadabr.i18n.MessageSource;
import br.org.scadabr.rt.UserRT;
import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.view.SharedUserAcess;
import com.serotonin.mango.web.UserSessionContextBean;
import javax.inject.Inject;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.ModelAttribute;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.servlet.support.RequestContext;

@Controller
@RequestMapping("/watchList")
@Scope("request")
public class WatchListController {

    @Inject
    private UserSessionContextBean userSessionContextBean;
    @Inject
    private WatchListDao watchListDao;
    @Inject
    private MessageSource messageSource;

    public static final String KEY_WATCHLISTS = "watchLists";
    public static final String KEY_SELECTED_WATCHLIST = "selectedWatchList";

    @RequestMapping(method = RequestMethod.GET)
    public String initializeForm(ModelMap model) {
        return "watchList";
    }

    @ModelAttribute
    protected void createModel(ModelMap modelMap, RequestContext requestContext) {
        throw new ImplementMeException();
        /*
        final UserRT user = userSessionContextBean.getUser();

        // The user's permissions may have changed since the last session, so make sure the watch lists are correct.
        final List<WatchList> watchLists = watchListDao.getWatchLists(user.getId());

        if (watchLists.isEmpty()) {
            // Add a default watch list if none exist.
            final WatchList watchList = new WatchList();
            watchList.setName(messageSource.getMessage("common.newName", requestContext.getLocale()));
            watchLists.add(watchListDao.createNewWatchList(watchList, user.getId()));
        }

        WatchListRT selected = user.getSelectedWatchList();
        boolean found = false;

        Map<String, String> watchListNames = new LinkedHashMap<>();
        for (WatchList watchList : watchLists) {
            if (watchList.getId() == selected.getId()) {
                found = true;
            }

            if (watchList.getUserAccess(user) == SharedUserAcess.OWNER) {
                // If this is the owner, check that the user still has access to the points. If not, remove the
                // unauthorized points, resave, and continue.
                boolean changed = false;
                List<DataPointVO> list = watchList.getPointList();
                List<DataPointVO> copy = new ArrayList<>(list);
                for (DataPointVO point : copy) {
                    if (point == null || !Permissions.hasDataPointReadPermission(user, point)) {
                        list.remove(point);
                        changed = true;
                    }
                }

                if (changed) {
                    watchListDao.saveWatchList(watchList);
                }
            }

            watchListNames.put(String.valueOf(watchList.getId()), watchList.getName());
        }

        if (!found) {
            // The user's default watch list was not found. It was either deleted or unshared from them. Find a new one.
            // The list will always contain at least one, so just use the id of the first in the list.
            selected = watchLists.get(0).getId();
            user.setSelectedWatchList(selected);
            watchListDao.saveSelectedWatchList(user.getId(), selected);
        }

        modelMap.put(KEY_WATCHLISTS, watchListNames);
        modelMap.put(KEY_SELECTED_WATCHLIST, selected);
*/
    }

    public static class JsonWatchList {

        private JsonWatchList(WatchList watchList) {
            throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
        }

    }

    public JsonWatchList setSelectedWatchList(int watchListId) {
        throw new ImplementMeException();
        /*
        UserRT user = userSessionContextBean.getUser();

        WatchList watchList = watchListDao.getWatchList(watchListId);
//TODO        Permissions.ensureWatchListPermission(user, watchList);
        prepareWatchList(watchList, user);

        watchListDao.saveSelectedWatchList(user.getId(), watchList.getId());
        user.setSelectedWatchList(watchListId);

        JsonWatchList data = getWatchListData(user, watchList);

        return data;
*/
    }

    private JsonWatchList getWatchListData(UserRT user, WatchList watchList) {
        JsonWatchList data = new JsonWatchList(watchList);

        List<DataPointVO> points = watchList.getPointList();
        List<Integer> pointIds = new ArrayList<>(points.size());
        for (DataPointVO point : points) {
            if (Permissions.hasDataPointReadPermission(user, point)) {
                pointIds.add(point.getId());
            }
        }

//        data.put("points", pointIds);
//        data.put("users", watchList.getWatchListUsers());
//        data.put("access", watchList.getUserAccess(user));
        return data;
    }

    private void prepareWatchList(WatchList watchList, UserRT user) {
        throw new ImplementMeException();
        /*
        SharedUserAcess access = watchList.getUserAccess(user);
        UserRT owner = userDao.getUser(watchList.getUserId());
        for (DataPointVO point : watchList.getPointList()) {
            updateSetPermission(point, access, owner);
        }
*/
    }

    private void updateSetPermission(DataPointVO point, SharedUserAcess access, UserRT owner) {
        // Point isn't settable
        if (!point.isSettable()) {
            return;
        }

        // Read-only access
        if (access != SharedUserAcess.OWNER && access != SharedUserAcess.SET) {
            return;
        }

        // Watch list owner doesn't have set permission
        if (!Permissions.hasDataPointSetPermission(owner, point)) {
            return;
        }

        // All good.
        point.setSettable(true);
    }

	//
}
