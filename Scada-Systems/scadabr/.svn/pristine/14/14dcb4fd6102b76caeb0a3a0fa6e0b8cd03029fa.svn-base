/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.vo.datasource.fhz4j;

import br.org.scadabr.logger.LogUtils;
import br.org.scadabr.web.i18n.LocalizableI18nKey;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import com.serotonin.mango.rt.event.type.AuditEventType;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;
import java.util.logging.Logger;
import net.sf.fhz4j.Fhz1000;
import net.sf.fhz4j.FhzProtocol;

import net.sf.fhz4j.fht.FhtDeviceType;
import net.sf.fhz4j.fht.FhtProperty;

/**
 *
 * @author aploese
 */
public class FhtPointLocator extends ProtocolLocator<FhtProperty> {

    private final static Logger LOG = Logger.getLogger(LogUtils.LOGGER_SCARABR_DS_FHZ4J);
    private FhtDeviceType fhtDeviceType;
    private short housecode;
    private String fhtDeviceTypeLabel;
    private String propertyLabel;

    /**
     * @return the housecode
     */
    public short getHousecode() {
        return housecode;
    }

    public String defaultName() {
        return getProperty() == null ? "FHT unknown" : getProperty().getLabel();
    }
    
    /**
     * @param housecode the housecode to set
     */
    public void setHousecode(short housecode) {
        this.housecode = housecode;
    }

    /**
     * @return the fhtDeviceType
     */
    public FhtDeviceType getFhtDeviceType() {
        return fhtDeviceType;
    }

    /**
     * @param fhtDeviceType the fhtDeviceType to set
     */
    public void setFhtDeviceType(FhtDeviceType fhtDeviceType) {
        this.fhtDeviceType = fhtDeviceType;
    }

    public void setHousecodeStr(String deviceHousecode) {
        this.housecode = Fhz1000.parseHouseCode(deviceHousecode);
    }

    public String getHousecodeStr() {
        return Fhz1000.houseCodeToString(housecode);
    }

    public String getFhtDeviceTypeLabel() {
        return fhtDeviceType.getLabel();
    }

    public void setFhtDeviceTypeLabel(String label) {
        fhtDeviceTypeLabel = label;
        tryFromDwr();
    }

    public String getPropertyLabel() {
        return getProperty().getLabel();
    }

    public void setPropertyLabel(String label) {
        propertyLabel = label;
        tryFromDwr();
    }

    private void tryFromDwr() {
        if ((propertyLabel != null) && (fhtDeviceTypeLabel != null)) {
            fhtDeviceType = FhtDeviceType.fromLabel(fhtDeviceTypeLabel);
            fhtDeviceTypeLabel = null;
            propertyLabel = null;
        }
    }

    @Override
    public void addProperties(List<LocalizableMessage> list) {
        super.addProperties(list);
        AuditEventType.addPropertyMessage(list, "dsEdit.fhz4j.fht.housecode", getHousecodeStr());
        AuditEventType.addPropertyMessage(list, "dsEdit.fhz4j.fht.devicetype", fhtDeviceType);
    }

    @Override
    public void addPropertyChanges(List<LocalizableMessage> list, ProtocolLocator<FhtProperty> o) {
        super.addPropertyChanges(list, o);
        FhtPointLocator from = (FhtPointLocator)o;
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.fhz4j.fht.housecode", from.getHousecodeStr(), getHousecodeStr());
        AuditEventType.maybeAddPropertyChangeMessage(list, "dsEdit.fhz4j.fht.devicetype", from.fhtDeviceType, fhtDeviceType);
    }
    //
    // /
    // / Serialization
    // /
    //
    private static final long serialVersionUID = -1;
    private static final int SERIAL_VERSION = 1;

    private void writeObject(ObjectOutputStream out) throws IOException {
        out.writeInt(SERIAL_VERSION);
        out.writeShort(housecode);
        out.writeObject(fhtDeviceType);
    }

    private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
        int ver = in.readInt();

        // Switch on the version of the class so that version changes can be elegantly handled.
        switch (ver) {
            case 1:
                housecode = in.readShort();
                fhtDeviceType = (FhtDeviceType) in.readObject();
         break;
            default:
                throw new RuntimeException("Cant handle version");
        }
    }


    @Override
    public FhzProtocol getFhzProtocol() {
        return FhzProtocol.FHT;
    }
    
    @Override
    public boolean isSettable() {
        return true;
    }

}
