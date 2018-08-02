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
package com.serotonin.mango.vo.permission;

import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.rt.UserRT;
import com.serotonin.mango.rt.event.type.EventType;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.event.EventTypeVO;

/**
 * @author Matthew Lohbihler
 *
 */
@Deprecated //TODO make a bean?? or see comment @DataPointAccess
public class Permissions {

    public interface DataPointAccessTypes {

        int NONE = 0;
        int READ = 1;
        int SET = 2;
        int DATA_SOURCE = 3;
        int ADMIN = 4;
    }

    private Permissions() {
        // no op
    }

    public static void ensureValidUser(UserRT user) throws PermissionException {
        if (user == null) {
            throw new PermissionException("Not logged in", null);
        }
        if (user.isDisabled()) {
            throw new PermissionException("User is disabled", user);
        }
    }

    public static boolean hasAdmin(UserRT user) throws PermissionException {
        ensureValidUser(user);
        return user.isAdmin();
    }

    public static void ensureAdmin(UserRT user) throws PermissionException {
        if (!hasAdmin(user)) {
            throw new PermissionException("User is not an administrator", user);
        }
    }

    //
    // / Data source admin
    //
    public static void ensureDataSourcePermission(UserRT user, int dataSourceId) throws PermissionException {
        if (!hasDataSourcePermission(user, dataSourceId)) {
            throw new PermissionException("User does not have permission to data source", user);
        }
    }

    public static void ensureDataSourcePermission(UserRT user) throws PermissionException {
        if (!hasDataSourcePermission(user)) {
            throw new PermissionException("User does not have permission to any data sources", user);
        }
    }

    public static boolean hasDataSourcePermission(UserRT user, int dataSourceId) throws PermissionException {
        throw new ImplementMeException();
        /*
        ensureValidUser(user);
        if (user.isAdmin()) {
            return true;
        }
        return user.getDataSourcePermissions().contains(dataSourceId);
*/    }

    public static boolean hasDataSourcePermission(UserRT user) throws PermissionException {
        throw new ImplementMeException();
        /*
        ensureValidUser(user);
        if (user.isAdmin()) {
            return true;
        }
        return user.getDataSourcePermissions().size() > 0;
    */
    }

    //
    // / Data point access
    //
    public static void ensureDataPointReadPermission(UserRT user, DataPointVO point) throws PermissionException {
        if (!hasDataPointReadPermission(user, point)) {
            throw new PermissionException("User does not have read permission to point", user);
        }
    }

    public static boolean hasDataPointReadPermission(UserRT user, DataPointVO point) throws PermissionException {
        throw new ImplementMeException(); //return hasDataPointReadPermission(user, point.getDataSourceId(), point.getId());
    }

    private static boolean hasDataPointReadPermission(UserRT user, int dataSourceId, int dataPointId)
            throws PermissionException {
        if (hasDataSourcePermission(user, dataSourceId)) {
            return true;
        }
        DataPointAccess a = getDataPointAccess(user, dataPointId);
        if (a == null) {
            return false;
        }
        return a.getPermission() == DataPointAccess.READ || a.getPermission() == DataPointAccess.SET;
    }

    public static void ensureDataPointSetPermission(UserRT user, DataPointVO point) throws PermissionException {
        throw new ImplementMeException();
        /*
        if (!point.getPointLocator().isSettable()) {
            throw new ShouldNeverHappenException("Point is not settable");
        }
        if (!hasDataPointSetPermission(user, point)) {
            throw new PermissionException("User does not have set permission to point", user);
        }
        */
    }

    public static boolean hasDataPointSetPermission(UserRT user, DataPointVO point) throws PermissionException {
        throw new ImplementMeException();
        /*
        if (hasDataSourcePermission(user, point.getDataSourceId())) {
            return true;
        }
        DataPointAccess a = getDataPointAccess(user, point.getId());
        if (a == null) {
            return false;
        }
        return a.getPermission() == DataPointAccess.SET;
        */
    }

    private static DataPointAccess getDataPointAccess(UserRT user, int dataPointId) {
        throw new ImplementMeException();
        /*
        for (DataPointAccess a : user.getDataPointPermissions()) {
            if (a.getDataPointId() == dataPointId) {
                return a;
            }
        }
        return null;
*/    }

    public static int getDataPointAccessType(UserRT user, DataPointVO point) {
        throw new ImplementMeException();
        /*
        if (user == null || user.isDisabled()) {
            return DataPointAccessTypes.NONE;
        }
        if (user.isAdmin()) {
            return DataPointAccessTypes.ADMIN;
        }
        if (user.getDataSourcePermissions().contains(point.getDataSourceId())) {
            return DataPointAccessTypes.DATA_SOURCE;
        }
        DataPointAccess a = getDataPointAccess(user, point.getId());
        if (a == null) {
            return DataPointAccessTypes.NONE;
        }
        if (a.getPermission() == DataPointAccess.SET) {
            return DataPointAccessTypes.SET;
        }
        if (a.getPermission() == DataPointAccess.READ) {
            return DataPointAccessTypes.READ;
        }
        return DataPointAccessTypes.NONE;
        */
    }

    //
    // / Report access
    // TODO do this in a generic fashion
/*    public static void ensureReportPermission(User user, ReportVO report) throws PermissionException {
        if (user == null) {
            throw new PermissionException("User is null", user);
        }
        if (report == null) {
            throw new PermissionException("Report is null", user);
        }
        if (report.getUserId() != user.getId()) {
            throw new PermissionException("User does not have permission to access the report", user);
        }
    }

    public static void ensureReportInstancePermission(User user, ReportInstance instance) throws PermissionException {
        if (user == null) {
            throw new PermissionException("User is null", user);
        }
        if (instance == null) {
            throw new PermissionException("Report instance is null", user);
        }
        if (instance.getUserId() != user.getId()) {
            throw new PermissionException("User does not have permission to access the report instance", user);
        }
    }
*/
    //
    // / Event access
    //
    public static boolean hasEventTypePermission(UserRT user, EventType eventType) {
        switch (eventType.getEventSource()) {
            case DATA_POINT:
                return hasDataPointReadPermission(user, eventType.getDataSourceId(), eventType.getDataPointId());
            case DATA_SOURCE:
                return hasDataSourcePermission(user, eventType.getDataSourceId());
            case SYSTEM:
            case COMPOUND:
            case SCHEDULED:
            case PUBLISHER:
            case AUDIT:
            case MAINTENANCE:
                return hasAdmin(user);
        }
        return false;
    }

    public static void ensureEventTypePermission(UserRT user, EventType eventType) throws PermissionException {
        if (!hasEventTypePermission(user, eventType)) {
            throw new PermissionException("User does not have permission to the view", user);
        }
    }

    public static void ensureEventTypePermission(UserRT user, EventTypeVO eventType) throws PermissionException {
        ensureEventTypePermission(user, eventType.createEventType());
    }
}
