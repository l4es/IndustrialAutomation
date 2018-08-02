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
package com.serotonin.mango.rt.dataSource.pop3;

import br.org.scadabr.DataType;
import java.text.DecimalFormat;
import java.text.SimpleDateFormat;
import java.util.regex.Pattern;

import com.serotonin.mango.rt.dataSource.PointLocatorRT;
import com.serotonin.mango.vo.dataSource.pop3.Pop3PointLocatorVO;

/**
 * @author Matthew Lohbihler
 */
public class Pop3PointLocatorRT extends PointLocatorRT<Pop3PointLocatorVO> {

    private final Pattern valuePattern;
    private String binary0Value;
    private DecimalFormat valueFormat;
    private final Pattern timePattern;
    private final boolean useReceivedTime;
    private final SimpleDateFormat timeFormat;

    public Pop3PointLocatorRT(Pop3PointLocatorVO vo) {
        super(vo);
        valuePattern = Pattern.compile(vo.getValueRegex());

        if (getDataType() == DataType.BINARY) {
            binary0Value = vo.getValueFormat();
        } else if (getDataType() == DataType.NUMERIC && !vo.getValueFormat().isEmpty()) {
            valueFormat = new DecimalFormat(vo.getValueFormat());
        }

        useReceivedTime = vo.isUseReceivedTime();
        if (!useReceivedTime && !vo.getTimeRegex().isEmpty()) {
            timePattern = Pattern.compile(vo.getTimeRegex());
            timeFormat = new SimpleDateFormat(vo.getTimeFormat());
        } else {
            timePattern = null;
            timeFormat = null;
        }

    }

    public boolean isFindInSubject() {
        return vo.isFindInSubject();
    }

    public Pattern getValuePattern() {
        return valuePattern;
    }

    public boolean isIgnoreIfMissing() {
        return vo.isIgnoreIfMissing();
    }

    public DecimalFormat getValueFormat() {
        return valueFormat;
    }

    public String getBinary0Value() {
        return binary0Value;
    }

    public boolean isUseReceivedTime() {
        return useReceivedTime;
    }

    public Pattern getTimePattern() {
        return timePattern;
    }

    public SimpleDateFormat getTimeFormat() {
        return timeFormat;
    }
}
