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

import com.serotonin.mango.util.ExportCodes;

/**
 * @author Matthew Lohbihler
 *
 */
@Deprecated //TODO Use a differnt aproach maybe th ds holds lists of nodes that can read,write,....
public class DataPointAccess {

    public static final int READ = 1;
    public static final int SET = 2;

    private static final ExportCodes ACCESS_CODES = new ExportCodes();

    static {
        ACCESS_CODES.addElement(READ, "READ", "common.access.read");
        ACCESS_CODES.addElement(SET, "SET", "common.access.set");
    }

    private int dataPointId;
    private int permission;

    public int getDataPointId() {
        return dataPointId;
    }

    public void setDataPointId(int dataPointId) {
        this.dataPointId = dataPointId;
    }

    public int getPermission() {
        return permission;
    }

    public void setPermission(int permission) {
        this.permission = permission;
    }

}
