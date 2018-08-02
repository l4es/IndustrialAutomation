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
package com.serotonin.mango.db.upgrade;

import java.io.OutputStream;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

/**
 * @author Matthew Lohbihler
 */
public class Upgrade1_1_0 extends DBUpgrade {

    private final Log log = LogFactory.getLog(getClass());

    @Override
    public void upgrade() throws Exception {
        OutputStream out = createUpdateLogOutputStream("1_1_0");

        // Run the script.
        log.info("Running script");
        runScript(script, out);

        out.flush();
        out.close();
    }

    @Override
    protected String getNewSchemaVersion() {
        return "1.1.1";
    }

    private static String[] script = {
        "alter table pointValueAnnotations add column textPointValueShort varchar(128);",
        "alter table pointValueAnnotations add column textPointValueLong clob;",
        "update pointValueAnnotations set textPointValueShort=textPointValue where textPointValue is not null and length(textPointValue) <= 128;",
        "update pointValueAnnotations set textPointValueLong=textPointValue where textPointValue is not null and length(textPointValue) > 128;",
        "alter table pointValueAnnotations drop textPointValue;",
        "alter table reportInstanceDataAnnotations add column textPointValueShort varchar(128);",
        "alter table reportInstanceDataAnnotations add column textPointValueLong clob;",
        "update reportInstanceDataAnnotations set textPointValueShort=textPointValue where textPointValue is not null and length(textPointValue) <= 128;",
        "update reportInstanceDataAnnotations set textPointValueLong=textPointValue where textPointValue is not null and length(textPointValue) > 128;",
        "alter table reportInstanceDataAnnotations drop textPointValue;",};
}
