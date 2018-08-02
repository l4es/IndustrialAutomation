package br.org.scadabr.vo.exporter.util;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import br.org.scadabr.json.JsonException;
import br.org.scadabr.json.JsonObject;
import br.org.scadabr.json.JsonReader;
import br.org.scadabr.json.JsonSerializable;
import com.serotonin.mango.rt.dataImage.PointValueTime;

public class PointValueJSONWrapper implements JsonSerializable {

    private String pointXid;
    private PointValueTime pointValue;

    public PointValueJSONWrapper(String pointXid, PointValueTime pointValue) {
        this.pointXid = pointXid;
        this.pointValue = pointValue;
    }

    @Override
    public void jsonDeserialize(JsonReader arg0, JsonObject arg1)
            throws JsonException {

    }

    @Override
    public void jsonSerialize(Map<String, Object> map) {
        map.put("pointXid", pointXid);
        map.put("timestamp", pointValue.getTime());
        map.put("value", "" + pointValue.getValue().getObjectValue());
    }

    public static List<PointValueJSONWrapper> wrapPointValues(
            String dataPointXid, List<PointValueTime> values) {
        List<PointValueJSONWrapper> wrapped = new ArrayList<PointValueJSONWrapper>();
        for (PointValueTime pointValueTime : values) {
            wrapped.add(new PointValueJSONWrapper(dataPointXid, pointValueTime));
        }
        return wrapped;
    }
}
