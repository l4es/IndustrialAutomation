package br.org.scadabr.web.mvc.controller.jsonrpc;

import br.org.scadabr.dao.DataPointDao;
import br.org.scadabr.l10n.Localizer;
import com.serotonin.mango.rt.dataImage.DataPointRT;
import com.serotonin.mango.rt.dataImage.PointValueTime;
import com.serotonin.mango.vo.DataPointVO;
import java.io.Serializable;

/**
 *
 * @author aploese
 */
public class JsonWatchListPoint implements Serializable {

    private String canonicalName;
    private int id;
    private boolean settable;
    private long timestamp;
    private String value;
    private boolean changed;

    //TODO remove changed to more sophisticated Websocket, transfer raw value (formatting should be done on the client side) transfer only timestamp as long 
    JsonWatchListPoint(final DataPointVO dp, final DataPointRT dpRt, final DataPointDao dataPointDao, final Localizer localizer) {
        id = dp.getId();
        settable = dp.isSettable();
        canonicalName = dataPointDao.getCanonicalPointName(dp);
        if (dpRt != null) {
            PointValueTime pvt = dpRt.getPointValue();

            timestamp = pvt.getTimestamp();
            changed = timestamp + 30000 > System.currentTimeMillis();
            value = localizer.formatValue(dp, pvt);
        }
    }

    /**
     * @return the canonicalName
     */
    public String getCanonicalName() {
        return canonicalName;
    }

    /**
     * @param canonicalName the canonicalName to set
     */
    public void setCanonicalName(String canonicalName) {
        this.canonicalName = canonicalName;
    }

    /**
     * @return the id
     */
    public int getId() {
        return id;
    }

    /**
     * @param id the id to set
     */
    public void setId(int id) {
        this.id = id;
    }

    /**
     * @return the settable
     */
    public boolean isSettable() {
        return settable;
    }

    /**
     * @param settable the settable to set
     */
    public void setSettable(boolean settable) {
        this.settable = settable;
    }

    /**
     * @return the timestamp
     */
    public long getTimestamp() {
        return timestamp;
    }

    /**
     * @param timestamp the timestamp to set
     */
    public void setTimestamp(long timestamp) {
        this.timestamp = timestamp;
    }

    /**
     * @return the value
     */
    public String getValue() {
        return value;
    }

    /**
     * @param value the value to set
     */
    public void setValue(String value) {
        this.value = value;
    }

    /**
     * @return the changed
     */
    public boolean isChanged() {
        return changed;
    }

    /**
     * @param changed the changed to set
     */
    public void setChanged(boolean changed) {
        this.changed = changed;
    }

}
