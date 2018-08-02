package br.org.scadabr.vo.scripting;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import br.org.scadabr.rt.scripting.ScriptRT;

import br.org.scadabr.json.JsonException;
import br.org.scadabr.json.JsonObject;
import br.org.scadabr.json.JsonReader;

import br.org.scadabr.json.JsonSerializable;
import com.serotonin.mango.Common;
import com.serotonin.mango.db.dao.DataSourceDao;
import com.serotonin.mango.db.dao.UserDao;
import com.serotonin.mango.vo.User;
import br.org.scadabr.util.StringUtils;
import br.org.scadabr.web.dwr.DwrResponseI18n;

public abstract class ScriptVO<T extends ScriptVO<?>> implements Serializable,
        JsonSerializable {

    abstract public Type getType();

    abstract public ScriptRT createScriptRT();

    public static final String XID_PREFIX = "SC_";

    public enum Type {

        CONTEXTUALIZED_SCRIPT(1, "scripting.systemCommands", true) {
                    @Override
                    public ScriptVO<?> createScriptVO() {
                        return new ContextualizedScriptVO();
                    }
                };

        private Type(int id, String key, boolean display) {
            this.id = id;
            this.key = key;
            this.display = display;
        }

        
        private final int id;
        
        private final String key;
        
        private final boolean display;

        public int getId() {
            return id;
        }

        public String getKey() {
            return key;
        }

        public boolean isDisplay() {
            return display;
        }

        public abstract ScriptVO<?> createScriptVO();

        public static Type valueOf(int id) {
            for (Type type : values()) {
                if (type.id == id) {
                    return type;
                }
            }
            return null;
        }

        public static Type valueOfIgnoreCase(String text) {
            for (Type type : values()) {
                if (type.name().equalsIgnoreCase(text)) {
                    return type;
                }
            }
            return null;
        }

        public static List<String> getTypeList() {
            List<String> result = new ArrayList<>();
            for (Type type : values()) {
                result.add(type.name());
            }
            return result;
        }
    }

    private int id = Common.NEW_ID;
    
    private String xid;
    
    private String name;
    
    private String script;
    private int userId;

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getXid() {
        return xid;
    }

    public void setXid(String xid) {
        this.xid = xid;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void validate(DwrResponseI18n response) {
        if (xid.isEmpty()) {
            response.addContextual("xid", "validate.required");
        } else if (!DataSourceDao.getInstance().isXidUnique(xid, id)) {
            response.addContextual("xid", "validate.xidUsed");
        } else if (xid.length() > 50) {
            response.addContextual("xid", "validate.notLongerThan", 50);
        }

        if (name.isEmpty()) {
            response.addContextual("name", "validate.nameRequired");
        }
        if (name.length() > 40) {
            response.addContextual("name", "validate.nameTooLong");
        }
    }

	//
    // /
    // / Serialization
    // /
    //
    private static final long serialVersionUID = -1;
    private static final int version = 1;

    private void writeObject(ObjectOutputStream out) throws IOException {
		// out.writeInt(version);
        // out.writeBoolean(enabled);
    }

    @SuppressWarnings("unchecked")
    private void readObject(ObjectInputStream in) throws IOException,
            ClassNotFoundException {
		// int ver = in.readInt();
        // Switch on the version of the class so that version changes can be
        // elegantly handled.
        // if (ver == 1) {
        // enabled = in.readBoolean();
        // }
    }

    @Override
    public void jsonDeserialize(JsonReader reader, JsonObject object)
            throws JsonException {
        String username = object.getString("user");
        User user = UserDao.getInstance().getUser(username);
        this.userId = user.getId();
    }

    @Override
    public void jsonSerialize(Map<String, Object> map) {
        map.put("type", getType().name());
        map.put("user", UserDao.getInstance().getUser(userId).getUsername());
    }

    public static ScriptVO<?> createScriptVO(int typeId) {
        return Type.valueOf(typeId).createScriptVO();
    }

    public static String generateXid() {
        return Common.generateXid("SC_");
    }

    public void setScript(String script) {
        this.script = script;
    }

    public String getScript() {
        return script;
    }

    public void setUserId(int userId) {
        this.userId = userId;
    }

    public int getUserId() {
        return userId;
    }

    public boolean isNew() {
        return id == Common.NEW_ID;
    }

}
