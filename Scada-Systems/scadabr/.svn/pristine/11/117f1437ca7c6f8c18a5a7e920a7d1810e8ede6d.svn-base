package br.org.scadabr.rt.scripting.context;

import com.serotonin.mango.Common;
import com.serotonin.mango.db.dao.DataPointDao;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.rt.dataImage.types.MangoValue;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.permission.Permissions;

public class DPCommandsScriptContextObject extends ScriptContextObject {

    public static final Type TYPE = Type.DATAPOINT_COMMANDS;

    @Override
    public Type getType() {
        return TYPE;
    }

    public void writeDataPoint(String xid, String stringValue) {
        DataPointVO dataPoint = DataPointDao.getInstance().getDataPoint(xid);
        if (dataPoint != null) {
            Permissions.ensureDataPointSetPermission(user, dataPoint);
            RuntimeManager runtimeManager = Common.ctx.getRuntimeManager();
            MangoValue value = MangoValue.stringToValue(stringValue, dataPoint.getDataType());
            runtimeManager.setDataPointValue(dataPoint.getId(), value, this.user);
        }
    }

    public void enableDataPoint(String xid) {
        DataPointVO dataPoint = DataPointDao.getInstance().getDataPoint(xid);
        if (dataPoint != null) {
            Permissions.ensureDataPointReadPermission(user, dataPoint);
            RuntimeManager runtimeManager = Common.ctx.getRuntimeManager();
            dataPoint.setEnabled(true);
            runtimeManager.saveDataPoint(dataPoint);
        }

    }

    public void disableDataPoint(String xid) {
        DataPointVO dataPoint = DataPointDao.getInstance().getDataPoint(xid);
        if (dataPoint != null) {
            Permissions.ensureDataPointReadPermission(user, dataPoint);
            RuntimeManager runtimeManager = Common.ctx.getRuntimeManager();
            dataPoint.setEnabled(false);
            runtimeManager.saveDataPoint(dataPoint);
        }

    }
}
