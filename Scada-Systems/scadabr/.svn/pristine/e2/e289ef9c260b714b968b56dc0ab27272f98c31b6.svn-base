/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo.datasource.fhz4j;

import br.org.scadabr.utils.ImplementMeException;
import br.org.scadabr.json.JsonException;
import br.org.scadabr.json.JsonObject;
import br.org.scadabr.json.JsonReader;
import br.org.scadabr.json.JsonSerializable;
import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import com.serotonin.mango.rt.event.type.AuditEventType;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.util.List;
import java.util.Map;
import java.util.logging.Logger;
import net.sf.fhz4j.FhzProtocol;
import net.sf.fhz4j.scada.ScadaProperty;

/**
 *
 * @author aploese
 */
public class ProtocolLocator<T extends ScadaProperty> implements Serializable, JsonSerializable {

    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCARABR_DS_FHZ4J);
    private T property;
    private boolean settable = false;

    void addProperties(List<LocalizableMessage> list) {
        AuditEventType.addPropertyMessage(list, "dsEdit.settable", settable);
        AuditEventType.addPropertyMessage(list, "dsEdit.fhz4j.property", property.getLabel());
    }

    void addPropertyChanges(List<LocalizableMessage> list, ProtocolLocator<T> from) {
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.setable", from.isSettable(), settable);
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.fhz4j.property", from.getProperty().getLabel(), property.getLabel());
    }

    /**
     * @return the property
     */
    public T getProperty() {
        return property;
    }

    /**
     * @param property the property to set
     */
    public void setProperty(T property) {
        this.property = property;
    }

    public FhzProtocol getFhzProtocol() {
        return FhzProtocol.UNKNOWN;
    }
    //
    // /
    // / Serialization
    // /
    //
    private static final long serialVersionUID = -1;
    private static final int serialVersion = 1;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(serialVersion);
        out.writeBoolean(settable);
        out.writeObject(property);
    }

    private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        switch (ver) {
            case 1:
                settable = in.readBoolean();
                property = (T) in.readObject();
                break;
            default:
                throw new RuntimeException("Cant handle version");
        }
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

    @Override
    public void jsonSerialize(Map<String, Object> paramMap) {
        throw new ImplementMeException(); 
    }

    @Override
    public void jsonDeserialize(JsonReader paramJsonReader, JsonObject paramJsonObject) throws JsonException {
        throw new ImplementMeException(); 
    }
}
