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
package com.serotonin.mango.util;

import br.org.scadabr.json.JsonException;
import br.org.scadabr.utils.i18n.LocalizableMessage;

/**
 * @author Matthew Lohbihler
 */
public class LocalizableJsonException extends JsonException implements LocalizableMessage {

    private final String i18nKey;
    private final Object[] args;

    public LocalizableJsonException(String i18nKey, Object... args) {
        this.i18nKey = i18nKey;
        this.args = args;
    }

    @Override
    public String getI18nKey() {
        return i18nKey;
    }

    @Override
    public Object[] getArgs() {
        return args;
    }

}
