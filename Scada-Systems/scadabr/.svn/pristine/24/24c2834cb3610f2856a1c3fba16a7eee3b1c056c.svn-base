package br.org.scadabr.vo.exporter.util;

import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import br.org.scadabr.json.JsonException;
import br.org.scadabr.json.JsonObject;
import br.org.scadabr.json.JsonReader;

import br.org.scadabr.json.JsonSerializable;
import com.serotonin.mango.db.dao.SystemSettingsDao;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Configurable;


@Configurable
public class SystemSettingsJSONWrapper implements JsonSerializable {

    @Autowired
    private SystemSettingsDao systemSettingsDao;
    
    public SystemSettingsJSONWrapper() {
    }

    @Override
    public void jsonSerialize(Map<String, Object> map) {

        Set<Entry<String, Object>> defaultValues = SystemSettingsDao.DEFAULT_VALUES
                .entrySet();

        for (Entry<String, Object> entry : defaultValues) {
            if (entry.getValue() instanceof Integer) {
                map.put(entry.getKey(), systemSettingsDao.getIntValue(entry.getKey()));
            } else if (entry.getValue() instanceof Boolean) {
                map.put(entry.getKey(), systemSettingsDao.getBooleanValue(entry.getKey()));
            } else if (entry.getValue() instanceof String) {
                map.put(entry.getKey(), systemSettingsDao.getValue(entry.getKey()));
            }
        }

    }

    @Override
    public void jsonDeserialize(JsonReader reader, JsonObject json)
            throws JsonException {

        Set<Entry<String, Object>> defaultValues = SystemSettingsDao.DEFAULT_VALUES
                .entrySet();

        for (Entry<String, Object> entry : defaultValues) {
            String key = entry.getKey();

            if (entry.getValue() instanceof Integer) {
                systemSettingsDao.setIntValue(key, json.getInt(key));
            } else if (entry.getValue() instanceof Boolean) {
                systemSettingsDao.setBooleanValue(key, json.getBoolean(key));
            } else if (entry.getValue() instanceof String) {
                systemSettingsDao.setValue(key, json.getString(key));
            }
        }
    }

}
