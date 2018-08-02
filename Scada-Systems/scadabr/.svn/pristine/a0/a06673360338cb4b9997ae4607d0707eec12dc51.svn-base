/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.util;

import java.io.IOException;
import java.io.InputStream;
import java.io.ObjectInputStream;
import java.io.ObjectStreamClass;

/**
 *
 * @author aploese
 */
@Deprecated
class ScadaBrObjectInputStream extends ObjectInputStream {

    public ScadaBrObjectInputStream(InputStream in) throws IOException {
        super(in);
    }

    @Override
    protected ObjectStreamClass readClassDescriptor() throws IOException, ClassNotFoundException {
        final ObjectStreamClass osc = super.readClassDescriptor();
        switch (osc.getName()) {
            case "com.serotonin.mango.vo.datasource.mbus.VMStatDataSourceVO":
                return ObjectStreamClass.lookup(Class.forName("br.org.scadabr.vo.datasource.vmstat.VMStatDataSourceVO"));
            case "com.serotonin.mango.vo.datasource.mbus.VMStatPointLocatorVO":
                return ObjectStreamClass.lookup(Class.forName("br.org.scadabr.vo.datasource.vmstat.VMStatPointLocatorVO"));
            default:
                //This is a mean workaround for changed Object fields ...
                // We changed some field types from int to I.E. from int to TimePeriods in DataPointVO, so we must discart the streamed ObjectInfo as well...
                //TODO change the upgrade to do this once ... and then simply load the updated streams ...
                return ObjectStreamClass.lookup(Class.forName(osc.getName()));
        }
    }
}
