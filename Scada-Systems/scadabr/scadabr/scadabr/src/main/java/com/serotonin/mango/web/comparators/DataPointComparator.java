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
package com.serotonin.mango.web.comparators;

import br.org.scadabr.l10n.AbstractLocalizer;
import br.org.scadabr.l10n.Localizer;
import br.org.scadabr.utils.ImplementMeException;
import java.util.ResourceBundle;

import com.serotonin.mango.vo.DataPointVO;

public class DataPointComparator extends BaseComparator<DataPointVO> {

    private static final int SORT_NAME = 1;
    private static final int SORT_DS_NAME = 2;
    private static final int SORT_ENABLED = 4;
    private static final int SORT_DATA_TYPE = 5;
    private static final int SORT_CONFIG = 6;

    private final ResourceBundle bundle;

    public DataPointComparator(ResourceBundle bundle, String sortField, boolean descending) {
        this.bundle = bundle;

        if (null != sortField) {
            switch (sortField) {
                case "name":
                    sortType = SORT_NAME;
                    break;
                case "dsName":
                    sortType = SORT_DS_NAME;
                    break;
                case "enabled":
                    sortType = SORT_ENABLED;
                    break;
                case "dataType":
                    sortType = SORT_DATA_TYPE;
                    break;
                case "config":
                    sortType = SORT_CONFIG;
                    break;
            }
        }
        this.descending = descending;
    }

    @Override
    public int compare(DataPointVO dp1, DataPointVO dp2) {
        throw new ImplementMeException();
        /*
        int result = 0;
        if (sortType == SORT_NAME) {
            result = dp1.getName().compareTo(dp2.getName());
        } else if (sortType == SORT_DS_NAME) {
            result = dp1.getDataSourceName().compareTo(dp2.getDataSourceName());
        } else if (sortType == SORT_ENABLED) {
            result = Boolean.compare(dp1.isEnabled(), dp2.isEnabled());
        } else if (sortType == SORT_DATA_TYPE) {
            String s1 = AbstractLocalizer.localizeMessage(dp1.getDataType(), bundle);
            String s2 = AbstractLocalizer.localizeMessage(dp2.getDataType(), bundle);
            result = s1.compareTo(s2);
        } else if (sortType == SORT_CONFIG) {
            String s1 = AbstractLocalizer.localizeMessage(dp1.getConfigurationDescription(), bundle);
            String s2 = AbstractLocalizer.localizeMessage(dp2.getConfigurationDescription(), bundle);
            result = s1.compareTo(s2);
        }

        if (descending) {
            return -result;
        }
        return result;
                */
    }
}
