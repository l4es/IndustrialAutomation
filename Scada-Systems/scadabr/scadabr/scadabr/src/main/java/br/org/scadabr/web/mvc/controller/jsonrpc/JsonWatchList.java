package br.org.scadabr.web.mvc.controller.jsonrpc;

import br.org.scadabr.dao.DataPointDao;
import br.org.scadabr.l10n.Localizer;
import com.serotonin.mango.rt.RuntimeManager;
import com.serotonin.mango.vo.DataPointVO;
import com.serotonin.mango.vo.WatchList;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

/**
 *
 * @author aploese
 */
public class JsonWatchList implements Serializable, Iterable<JsonWatchListPoint> {

    private String name;
    private int id;
    private List<JsonWatchListPoint> points = new ArrayList<>();

    public JsonWatchList(WatchList watchList, DataPointDao dataPointDao, RuntimeManager runtimeManager, final Localizer localizer) {
        id = watchList.getId();
        name = watchList.getName();
        for (DataPointVO dp : watchList.getPointList()) {
            points.add(new JsonWatchListPoint(dp, runtimeManager.getDataPoint(dp.getId()), dataPointDao, localizer));
        }
    }
    
    /**
     * @return the name
     */
    public String getName() {
        return name;
    }

    /**
     * @param name the name to set
     */
    public void setName(String name) {
        this.name = name;
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
     * @return the points
     */
    public List<JsonWatchListPoint> getPoints() {
        return points;
    }

    /**
     * @param points the points to set
     */
    public void setPoints(List<JsonWatchListPoint> points) {
        this.points = points;
    }

    @Override
    public Iterator<JsonWatchListPoint> iterator() {
        return points.iterator();
    }

}
