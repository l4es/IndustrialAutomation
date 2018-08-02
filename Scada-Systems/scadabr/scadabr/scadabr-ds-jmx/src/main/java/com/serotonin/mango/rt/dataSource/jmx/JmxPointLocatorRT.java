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
package com.serotonin.mango.rt.dataSource.jmx;

import javax.management.ObjectName;

import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.rt.dataSource.PointLocatorRT;
import com.serotonin.mango.vo.dataSource.jmx.JmxPointLocatorVO;
import br.org.scadabr.util.ArrayUtils;

/**
 * @author Matthew Lohbihler
 */
public class JmxPointLocatorRT extends PointLocatorRT<JmxPointLocatorVO> {

    private ObjectName objectName;
    private String type;

    public JmxPointLocatorRT(JmxPointLocatorVO vo) {
        super(vo);
    }

    public ObjectName getObjectName() {
        return objectName;
    }

    public void setObjectName(ObjectName objectName) {
        this.objectName = objectName;
    }

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }

    public boolean isComposite() {
        return !vo.getCompositeItemName().isEmpty();
    }

    public static boolean isValidType(String type) {
        return ArrayUtils.contains(validTypes, type);
    }

    public Object mangoValueToManagementValue(MangoValue value) {
        if (value == null) {
            return null;
        }

        if ("int".equals(type) || "java.lang.Integer".equals(type)) {
            return value.getIntegerValue();
        }
        if ("long".equals(type) || "java.lang.Long".equals(type)) {
            return (long) value.getIntegerValue();
        }
        if ("java.lang.String".equals(type)) {
            return value.getStringValue();
        }
        if ("double".equals(type)) {
            return value.getDoubleValue();
        }
        if ("boolean".equals(type)) {
            return value.getBooleanValue();
        }

        return null;
    }

    public MangoValue managementValueToMangoValue(Object value) {
        String s = null;
        if (value != null) {
            s = value.toString();
        }
        return MangoValue.stringToValue(s, vo.getDataType());
    }

    static String[] validTypes = {"int", "java.lang.Integer", "long", "java.lang.Long", "java.lang.String", "double",
        "boolean",};
}
