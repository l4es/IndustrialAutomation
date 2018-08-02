/*
 *   Mango - Open Source M2M - http://mango.serotoninsoftware.com
 *   Copyright (C) 2010 Arne Pl\u00f6se
 *   @author Arne Pl\u00f6se
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
package com.serotonin.mango.web.dwr.beans;

import net.sf.mbus4j.dataframes.datablocks.DataBlock;
import net.sf.mbus4j.dataframes.datablocks.dif.DataFieldCode;
import net.sf.mbus4j.dataframes.datablocks.vif.Vif;
import net.sf.mbus4j.dataframes.datablocks.vif.VifAscii;
import net.sf.mbus4j.dataframes.datablocks.vif.VifPrimary;

public class MBusDataBlockBean {

    private final int dbIndex;
    private final int devIndex;
    private final int rsIndex;
    private final DataBlock db;

    MBusDataBlockBean(int devIndex, int rsIndex, int dbIndex, DataBlock db) {
        this.devIndex = devIndex;
        this.rsIndex = rsIndex;
        this.dbIndex = dbIndex;
        this.db = db;
    }

    public String getName() {
        return db.getParamDescr();
    }

    public String getUiName() {
        final boolean highlight = (db.getStorageNumber() == 0)
                && (db.getVif() != VifPrimary.FABRICATION_NO)
                && (db.getVif() != VifPrimary.TIMEPOINT_DATE)
                && (db.getVif() != VifPrimary.TIMEPOINT_TIME_AND_DATE)
                && (!(db.getVif() instanceof VifAscii))
                && (db.getVif() != VifPrimary.BUS_ADDRESS);
        if (highlight) {
            StringBuilder sb = new StringBuilder();
            sb.append("<big><em>");
            sb.append(db.getParamDescr());
            sb.append("</em></big>");
            return sb.toString();
        } else {
            return db.getParamDescr();
        }
    }

    public int getDbIndex() {
        return dbIndex;
    }

    public int getRsIndex() {
        return rsIndex;
    }

    public int getDevIndex() {
        return devIndex;
    }

    /**
     * Reformat the generic output omit value and replace LF with tag br.
     *
     * @return
     */
    public String getParams() {
        String[] splitted = db.toString().split("\n");
        StringBuilder sb = new StringBuilder();
        for (String line : splitted) {
            if (line.startsWith("value")) {
                //skip
            } else {
                String[] subLine = line.split(" = ");
                sb.append("<strong>").append(subLine[0]).append(" = </strong>\"").append(subLine[1]).append("\", ");
            }
        }
        sb.delete(sb.length() - 3, sb.length() - 1);
        return sb.toString();
    }

    public String getValue() {
        return db.getValueAsString();
    }
}
